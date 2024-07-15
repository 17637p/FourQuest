#include "Process.h"

//#include "GUIManager.h"
#include "InputManager.h"
#include "DemoUtils.h"

#include "../FQGraphics/IFQGraphics.h"

#include "../FQLoader/ModelConverter.h"
#include "../FQLoader/ModelLoader.h"

#include <FQCommonGraphics.h>

using namespace fq::utils;

Process::Process()
	:
	mWindowPosX(0),
	mWindowPosY(0),
	mScreenWidth(1920),
	mScreenHeight(1080),
	mResizing(false),
	mTestGraphics(nullptr)
{
	HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

	CreateHWND(L"FQGraphicsDemo", WS_OVERLAPPEDWINDOW, mWindowPosX, mWindowPosY, mScreenWidth, mScreenHeight);
	ShowWindow(mHwnd, SW_SHOWNORMAL);

	mEngineExporter = std::make_shared<fq::graphics::EngineExporter>();
}

Process::~Process()
{
	for (fq::graphics::IStaticMeshObject* iobj : mStaticMeshObjects)
	{
		mTestGraphics->DeleteStaticMeshObject(iobj);
	}

	for (fq::graphics::ISkinnedMeshObject* iobj : mSkinnedMeshObjects)
	{
		mTestGraphics->DeleteSkinnedMeshObject(iobj);
	}

	for (fq::graphics::IParticleObject* iobj : mParticleObjects)
	{
		mTestGraphics->DeleteParticleObject(iobj);
	}

	for (fq::graphics::IDecalObject* iobj : mDecalObjects)
	{
		mTestGraphics->DeleteDecalObject(iobj);
	}

	for (fq::graphics::ITrailObject* iobj : mTrailObjects)
	{
		mTestGraphics->DeleteTrailObject(iobj);
	}

	//mTestGraphics->DeleteLight(1);
	//mTestGraphics->DeleteLight(2);
	//mTestGraphics->DeleteLight(3);
	//mTestGraphics->DeleteLight(4);

	mEngineExporter->DeleteEngine(mTestGraphics);

	CoUninitialize();
}

bool Process::Init(HINSTANCE hInstance)
{
	using namespace fq::graphics;

	InputManager::GetInstance().Init(mHwnd);
	//m_timer = std::make_unique<GameTimer>();
	mTimeManager.Init();

	mTestGraphics = mEngineExporter->GetEngine();

	mTestGraphics->Initialize(mHwnd, mScreenWidth, mScreenHeight, fq::graphics::EPipelineType::Deferred);

	// 카메라 초기화
	AddDefaultCamera(mTestGraphics);

	/// camera 초기화
	cameraTransform.worldPosition = { 0, 0, 0 };
	cameraTransform.worldRotation = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(DirectX::XMConvertToRadians(0.0f), DirectX::XMConvertToRadians(0.0f), DirectX::XMConvertToRadians(0.0f));
	cameraTransform.worldScale = { 1, 1, 1 };

	cameraTransform.worldMatrix =
		DirectX::SimpleMath::Matrix::CreateScale(cameraTransform.worldScale) *
		DirectX::SimpleMath::Matrix::CreateFromQuaternion(cameraTransform.worldRotation) *
		DirectX::SimpleMath::Matrix::CreateTranslation(cameraTransform.worldPosition);

	//-------------------------------------

	/// Light 초기화 
	fq::graphics::LightInfo directionalLightInfo;

	directionalLightInfo.type = fq::graphics::ELightType::Directional;
	directionalLightInfo.color = { 1, 1, 1, 1 };
	directionalLightInfo.intensity = 2;
	directionalLightInfo.direction = { 0, -0.7, -0.7 };
	directionalLightInfo.direction.Normalize();

	mTestGraphics->AddLight(1, directionalLightInfo);

	directionalLightInfo.type = fq::graphics::ELightType::Directional;
	directionalLightInfo.color = { 1,1,1, 1 };
	directionalLightInfo.intensity = 1;
	directionalLightInfo.direction = { 1 ,-1, 0 };
	directionalLightInfo.direction.Normalize();

	mTestGraphics->AddLight(2, directionalLightInfo);

	directionalLightInfo.type = fq::graphics::ELightType::Directional;
	directionalLightInfo.color = { 1, 1 ,1, 1 };
	directionalLightInfo.intensity = 1;
	directionalLightInfo.direction = { -1, -1, 0 };
	directionalLightInfo.direction.Normalize();

	mTestGraphics->AddLight(3, directionalLightInfo);

	directionalLightInfo.type = fq::graphics::ELightType::Directional;
	directionalLightInfo.color = { 1, 1 ,1, 1 };
	directionalLightInfo.intensity = 1;
	directionalLightInfo.direction = { 0, -1, -1 };
	directionalLightInfo.direction.Normalize();

	mTestGraphics->AddLight(4, directionalLightInfo);

	//directionalLightInfo.type = fq::graphics::ELightType::Directional;
	//directionalLightInfo.color = { 1,1,1, 1 };
	//directionalLightInfo.intensity = 1;
	//directionalLightInfo.direction = { 1 ,-1, 0 };
	//directionalLightInfo.direction.Normalize();
	//
	//mTestGraphics->AddLight(2, directionalLightInfo);
	//
	//directionalLightInfo.type = fq::graphics::ELightType::Directional;
	//directionalLightInfo.color = { 1, 1 ,1, 1 };
	//directionalLightInfo.intensity = 1;
	//directionalLightInfo.direction = { -1, -1, 0 };
	//directionalLightInfo.direction.Normalize();
	//
	//mTestGraphics->AddLight(3, directionalLightInfo);
	//
	//directionalLightInfo.type = fq::graphics::ELightType::Directional;
	//directionalLightInfo.color = { 1, 1 ,1, 1 };
	//directionalLightInfo.intensity = 1;
	//directionalLightInfo.direction = { 0, -1, -1 };
	//directionalLightInfo.direction.Normalize();
	//
	//mTestGraphics->AddLight(4, directionalLightInfo);
	//directionalLightInfo.type = fq::graphics::ELightType::Spot;
	//directionalLightInfo.color = { 1,0,0, 1 };
	//directionalLightInfo.intensity = 1000;
	//directionalLightInfo.range = 1000;
	//directionalLightInfo.direction = { 0, 0, 1 };
	//directionalLightInfo.position = { 0, 0 ,-500 };
	//directionalLightInfo.attenuation = { 0, 1, 0 };
	//directionalLightInfo.spot = 8;
	//
	//mTestGraphics->AddLight(4, directionalLightInfo);
	//
	fq::graphics::LightInfo pointLightInfo;
	pointLightInfo.type = fq::graphics::ELightType::Point;
	pointLightInfo.color = { 1, 0, 0, 1 };
	pointLightInfo.intensity = 500;
	pointLightInfo.range = 100;
	pointLightInfo.attenuation = { 0, 1, 0 };
	pointLightInfo.position = { 10.f, 10.f, 0.f };

	// mTestGraphics->AddLight(5, pointLightInfo);

	renderObjectInit();
	VFXInit();

	return true;
}

void Process::Loop()
{
	MSG msg;

	while (true)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				break;
			}

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			Update();
			Render();
		}
	}
}

void Process::Finalize()
{

}

//extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT Process::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HDC			hdc;
	PAINTSTRUCT ps;

	//if (ImGui_ImplWin32_WndProcHandler(m_hWnd, uMsg, wParam, lParam))
	//	return true;

	switch (uMsg)
	{
	case WM_SIZE:
	{
		//if (m_pRenderer == nullptr)
		//{
		//	return 0;
		//}
		//m_pRenderer->SetClientSize(LOWORD(lParam), HIWORD(lParam));

		mScreenWidth = LOWORD(lParam);
		mScreenHeight = HIWORD(lParam);
		mScreenWidth = max(200, mScreenWidth);
		mScreenHeight = max(200, mScreenHeight);

		break;
	}
	case WM_PAINT:
	{
		hdc = BeginPaint(mHwnd, &ps);
		EndPaint(mHwnd, &ps);
		break;
	}
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		break;
	}
	}

	return DefWindowProc(mHwnd, uMsg, wParam, lParam);
}

void Process::Update()
{
	mTimeManager.Update();
	InputManager::GetInstance().Update();

	calculateFrameStats();

	if (GetAsyncKeyState(VK_F1) & 0x8000)
	{
		mTestGraphics->SetWindowSize(mScreenWidth, mScreenHeight);
	}
	// ESC 버튼 누르면 프로그램 종료
	if (GetAsyncKeyState(VK_ESCAPE))
	{
		PostQuitMessage(0);
	}

	// 카메라 조작
	float speed = mTimeManager.GetDeltaTime() * 1000.f;
	if (InputManager::GetInstance().IsGetKey(VK_SHIFT))
	{
		speed = mTimeManager.GetDeltaTime() * 1000;
	}
	else
	{
		speed = mTimeManager.GetDeltaTime() * 100.f;
	}

	if (InputManager::GetInstance().IsGetKey('W'))
	{
		Walk(cameraTransform, speed);
	}
	if (InputManager::GetInstance().IsGetKey('S'))
	{
		Walk(cameraTransform, -speed);
	}
	if (InputManager::GetInstance().IsGetKey('D'))
	{
		Strafe(cameraTransform, speed);
	}
	if (InputManager::GetInstance().IsGetKey('A'))
	{
		Strafe(cameraTransform, -speed);
	}
	if (InputManager::GetInstance().IsGetKey('E'))
	{
		WorldUpdown(cameraTransform, speed);
	}
	if (InputManager::GetInstance().IsGetKey('Q'))
	{
		WorldUpdown(cameraTransform, -speed);
	}

	//// Test용 두 번째 카메라
	//if (InputManager::GetInstance().IsGetKey('I'))
	//{
	//	Walk(cameraTransform2, speed);
	//}
	//if (InputManager::GetInstance().IsGetKey('K'))
	//{
	//	Walk(cameraTransform2, -speed);
	//}
	//if (InputManager::GetInstance().IsGetKey('L'))
	//{
	//	strafe(cameraTransform2, speed);
	//}
	//if (InputManager::GetInstance().IsGetKey('J'))
	//{
	//	strafe(cameraTransform2, -speed);
	//}
	//if (InputManager::GetInstance().IsGetKey('O'))
	//{
	//	worldUpdown(cameraTransform2, speed);
	//}
	//if (InputManager::GetInstance().IsGetKey('U'))
	//{
	//	worldUpdown(cameraTransform2, -speed);
	//}

	mTestGraphics->UpdateCamera(cameraTransform);
	//mTestGraphics->UpdateColCamera(cameraTransform2); 이거 키면 두 번째 카메라로 컬링 테스트 가능

	if (InputManager::GetInstance().IsGetKey(VK_RBUTTON))
	{
		float dx = (0.25f * static_cast<float>(InputManager::GetInstance().GetDeltaPosition().x) * (3.141592f / 180.0f));
		float dy = (0.25f * static_cast<float>(InputManager::GetInstance().GetDeltaPosition().y) * (3.141592f / 180.0f));

		Pitch(cameraTransform, dy);
		Yaw(cameraTransform, dx);
	}

	// 스카이박스 
	if (InputManager::GetInstance().IsGetKeyDown('K'))
	{
		mTestGraphics->SetSkyBox(L"./resource/example/texture/defaultEnvHDR.dds", true, 1.f);
	}
	if (InputManager::GetInstance().IsGetKeyDown('O'))
	{
		// for (const auto& object : mStaticMeshObjects)
		// {
		// 	mTestGraphics->DeleteStaticMeshObject(object);
		// }
		//mTestGraphics->SetSkyBox(L"./resource/example/texture/custom1.dds", true, 1.f);
		mTestGraphics->SetSkyBox(L"./resource/example/texture/defaultEnvHDR.dds");
		mTestGraphics->SetIBLTexture(L"./resource/example/texture/defaultDiffuseHDR.dds",
			L"./resource/example/texture/defaultSpecularHDR.dds",
			L"./resource/example/texture/defaultBrdf.dds");

		//mTestGraphics->SetSkyBox(L"CubeProbe0.dds");
		//mTestGraphics->SetIBLTexture(L"CubeProbe0.dds",
		//	L"CubeProbe0.dds",
		//	L"CubeProbe0.dds");
	}

	shadowTest();
	renderObjectUpdate();
	VFXUpdate();

	InputManager::GetInstance().Update();
}

void Process::Render()
{
	using namespace fq::graphics;

	mTestGraphics->BeginRender();
	debugRender();
	mTestGraphics->Render();

	// --------------------font Test-------------------------------
	//DirectX::SimpleMath::Rectangle drawRect;
	//drawRect.x = 600;
	//drawRect.y = 600;
	//drawRect.width = 1000;
	//drawRect.height = 1000;
	//mTestGraphics->DrawText(L"집가고싶당", drawRect, 32, L"DungGeunMo", { 0.1,0.8,0.4,1 });
	//
	//drawRect.x = 600;
	//drawRect.y = 700;
	//mTestGraphics->DrawText(L"집가고싶당", drawRect, 50, L"Verdana", { 0.8,0.8,0.4,1 });
	// ---------------------------------------------------

	mTestGraphics->EndRender();

	if (GetAsyncKeyState(VK_F2) & 0x8000)
	{
		mTestGraphics->SetPipelineType(fq::graphics::EPipelineType::Forward);
	}
	else if (GetAsyncKeyState(VK_F3) & 0x8000)
	{
		mTestGraphics->SetPipelineType(fq::graphics::EPipelineType::Deferred);
	}
}

void Process::debugRender()
{
	return;
	using namespace fq::graphics::debug;

	SphereInfo sphereInfo;
	sphereInfo.Sphere.Center = { 0, 0, 0 };
	sphereInfo.Sphere.Radius = 500;
	sphereInfo.Color = { 0,0,0,1 };
	mTestGraphics->DrawSphere(sphereInfo);

	AABBInfo aabbInfo;
	aabbInfo.AABB.Center = { 0, 0, 0 };
	aabbInfo.AABB.Extents = { 500, 500, 500 };
	aabbInfo.Color = { 1, 0, 0, 1 };
	mTestGraphics->DrawBox(aabbInfo);

	OBBInfo obbInfo;
	obbInfo.OBB.Center = { 0,0,0 };
	obbInfo.OBB.Extents = { 500, 500, 500 };
	obbInfo.OBB.Orientation = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll({ 3.14 * 0.25f, 3.14 * 0.25f, 0 });
	obbInfo.Color = { 0, 1, 0, 1 };
	mTestGraphics->DrawOBB(obbInfo);

	FrustumInfo frustumInfo;
	frustumInfo.Frustum.Origin = cameraTransform2.worldPosition;
	frustumInfo.Frustum.Orientation = cameraTransform2.worldRotation;
	frustumInfo.Frustum.Near = 0.03f;
	frustumInfo.Frustum.Far = 3000.f;
	frustumInfo.Frustum.LeftSlope = -0.736355f;
	frustumInfo.Frustum.RightSlope = 0.736355f;
	frustumInfo.Frustum.TopSlope = 0.4142f;
	frustumInfo.Frustum.BottomSlope = -0.4142;
	frustumInfo.Color = { 0, 1, 0, 1 };
	mTestGraphics->DrawFrustum(frustumInfo);

	GridInfo gridInfo;
	gridInfo.Origin = { 0, 300,0 };
	gridInfo.XAxis = { 1, 0, 0 };
	gridInfo.YAxis = { 0, 0, 1 };
	gridInfo.XDivision = 10;
	gridInfo.YDivision = 10;
	gridInfo.GridSize = 1000.f;
	gridInfo.Color = { 1, 0, 1, 1 };
	mTestGraphics->DrawGrid(gridInfo);

	RingInfo ringInfo;
	ringInfo.Origin = { 0, 0, 0 };
	ringInfo.MajorAxis = { 300, 0, 0 };
	ringInfo.MinorAxis = { 0, 300, 0 };
	ringInfo.Color = { 1, 1, 0, 1 };
	mTestGraphics->DrawRing(ringInfo);

	RayInfo rayInfo;
	rayInfo.Origin = { 0,0,-100 };
	rayInfo.Direction = { 0, 100, 100 };
	rayInfo.Normalize = false;
	rayInfo.Color = { 0.5f, 0.5f, 0.5f, 1 };
	mTestGraphics->DrawRay(rayInfo);

	PolygonInfo polygonInfo;
	polygonInfo.Points.push_back({ 200, 200, 0 });
	polygonInfo.Points.push_back({ 400, 200, 0 });
	polygonInfo.Points.push_back({ 400, 400, 0 });
	polygonInfo.Color = { 0.1f, 0.2f, 0.3f, 1.f };
	mTestGraphics->DrawPolygon(polygonInfo);

	polygonInfo.Points.clear();
	polygonInfo.Points.push_back({ -200, 200, 0 });
	polygonInfo.Points.push_back({ -400, 200, 0 });
	polygonInfo.Points.push_back({ -400, 400, 0 });
	polygonInfo.Points.push_back({ -300, 500, 0 });
	polygonInfo.Points.push_back({ -200, 400, 0 });
	mTestGraphics->DrawPolygon(polygonInfo);
}

void Process::shadowTest()
{
	if (GetAsyncKeyState('5') & 0x8000)
	{
		mTestGraphics->UseShadow(1, true);
		mTestGraphics->UseShadow(4, true);
	}
	else
	{
		mTestGraphics->UseShadow(1, false);
		mTestGraphics->UseShadow(4, false);
	}

	if (GetAsyncKeyState('6') & 0x8000)
	{
		mTestGraphics->UseShadow(2, true);
	}
	else
	{
		mTestGraphics->UseShadow(2, false);
	}
	if (GetAsyncKeyState('7') & 0x8000)
	{
		mTestGraphics->UseShadow(3, true);
	}
	else
	{
		mTestGraphics->UseShadow(3, false);
	}
}

void Process::particleInit()
{
	using namespace fq::graphics;
	using namespace DirectX::SimpleMath;

	ParticleMaterialInfo particleMaterialInfo;
	particleMaterialInfo.BaseColorFileName = L"./resource/example/texture/Particle02.png";
	std::shared_ptr<IParticleMaterial> particleMaterial = mTestGraphics->CreateParticleMaterial(particleMaterialInfo);

	{
		ParticleInfo particleInfo = { };
		IParticleObject* obj;

		particleInfo.MainData.StartSize.x = 10.f;
		particleInfo.MainData.StartSpeed.x = 50.f;
		particleInfo.MainData.StartColor0 = { 1, 0.3, 0.3, 1 };
		particleInfo.ShapeData.ShapeType = ParticleInfo::Shape::EShape::Sphere;
		particleInfo.ShapeData.Scale = { 50, 50, 50 };
		particleInfo.ShapeData.ArcInDegree = 242;
		particleInfo.ShapeData.Rotation = { 10, 10, 10 };
		particleInfo.ShapeData.Scale = { 2, 4, 3 };
		particleInfo.ShapeData.Radius = 10.f;

		particleInfo.LimitVelocityOverLifetimeData.bIsUsed = true;
		particleInfo.LimitVelocityOverLifetimeData.Speed = 5.f;
		particleInfo.LimitVelocityOverLifetimeData.Dampen = 0.5f;

		auto transform = Matrix::CreateScale(1, 2, 3) * Matrix::CreateFromYawPitchRoll(1.24, 2.46, 3.68) * DirectX::SimpleMath::Matrix::CreateTranslation({ -300, 300,0 });
		obj = mTestGraphics->CreateParticleObject(particleMaterial, particleInfo, transform);
		mParticleObjects.push_back(obj);

		particleInfo.EmissionData.ParticlesPerSecond = 200.f;
		transform = DirectX::SimpleMath::Matrix::CreateTranslation({ -300, 100,0 });
		obj = mTestGraphics->CreateParticleObject(particleMaterial, particleInfo, transform);
		mParticleObjects.push_back(obj);
	}

	{
		ParticleInfo particleInfo = { };
		IParticleObject* obj;

		particleInfo.MainData.StartSize.x = 10.f;
		particleInfo.MainData.StartSpeed.x = 50.f;
		particleInfo.MainData.StartColor0 = { 1, 0.3, 0.3, 1 };
		particleInfo.ShapeData.ShapeType = ParticleInfo::Shape::EShape::Hemisphere;
		particleInfo.ShapeData.Scale = { 50, 50, 50 };
		particleInfo.ShapeData.ArcInDegree = 242;
		particleInfo.ShapeData.Rotation = { 10, 10, 10 };
		particleInfo.ShapeData.Scale = { 2, 4, 3 };
		particleInfo.ShapeData.Radius = 10.f;

		//particleInfo.ForceOverLifeTimeData.bIsUsed = true;
		//particleInfo.ForceOverLifeTimeData.Force = { 5, -50, 0 };

		auto transform = Matrix::CreateScale(1, 2, 3) * Matrix::CreateFromYawPitchRoll(1.24, 2.46, 3.68) * DirectX::SimpleMath::Matrix::CreateTranslation({ -200, 300,0 });
		obj = mTestGraphics->CreateParticleObject(particleMaterial, particleInfo, transform);
		mParticleObjects.push_back(obj);

		particleInfo.EmissionData.ParticlesPerSecond = 200.f;
		transform = DirectX::SimpleMath::Matrix::CreateTranslation({ -200, 100,0 });
		obj = mTestGraphics->CreateParticleObject(particleMaterial, particleInfo, transform);
		mParticleObjects.push_back(obj);
	}

	// 
	{
		ParticleInfo particleInfo = { };
		IParticleObject* obj;

		particleInfo.MainData.StartSize.x = 10.f;
		particleInfo.MainData.StartSpeed.x = 50.f;
		particleInfo.MainData.StartColor0 = { 0.3, 0.3, 1, 1 };
		particleInfo.ShapeData.ShapeType = ParticleInfo::Shape::EShape::Cone;
		particleInfo.ShapeData.AngleInDegree = 45;
		particleInfo.ShapeData.ArcInDegree = 242;
		particleInfo.ShapeData.Rotation = { 10, 10, 10 };
		particleInfo.ShapeData.Scale = { 2, 4, 3 };
		particleInfo.ShapeData.Radius = 10.f;

		auto transform = Matrix::CreateScale(1, 2, 3) * Matrix::CreateFromYawPitchRoll(1.24, 2.46, 3.68) * DirectX::SimpleMath::Matrix::CreateTranslation({ -200, 300,0 });
		obj = mTestGraphics->CreateParticleObject(particleMaterial, particleInfo, transform);
		mParticleObjects.push_back(obj);

		particleInfo.EmissionData.ParticlesPerSecond = 200.f;
		transform = DirectX::SimpleMath::Matrix::CreateTranslation({ -200, 100,0 });
		obj = mTestGraphics->CreateParticleObject(particleMaterial, particleInfo, transform);
		mParticleObjects.push_back(obj);
	}

	{
		ParticleInfo particleInfo = { };
		IParticleObject* obj;

		particleInfo.MainData.StartSize.x = 10.f;
		particleInfo.MainData.StartSpeed.x = 50.f;
		particleInfo.ShapeData.Radius = 100.f;
		particleInfo.ShapeData.ShapeType = ParticleInfo::Shape::EShape::Donut;
		particleInfo.ShapeData.ArcInDegree = 92;
		// particleInfo.ShapeData.Rotation = { 10, 10, 10 };
		// particleInfo.ShapeData.Scale = { 2, 4, 3 };
		particleInfo.ShapeData.Radius = 10.f;
		particleInfo.ShapeData.DountRadius = 5.f;

		particleInfo.SizeOverLifetimeData.bIsUsed = true;
		particleInfo.SizeOverLifetimeData.PointA = { 50, 0.f };
		particleInfo.SizeOverLifetimeData.PointB = { 1, 0.5f };
		particleInfo.SizeOverLifetimeData.PointC = { 1, 0.9f };
		particleInfo.SizeOverLifetimeData.PointD = { 20, 1.f };

		//particleInfo.ColorOverLifetimeData.bIsUsed = true;
		//particleInfo.ColorOverLifetimeData.AlphaRatios[0] = { 1.f, 0 };
		//particleInfo.ColorOverLifetimeData.AlphaRatios[1] = { 0.5f, 0.25f };
		//particleInfo.ColorOverLifetimeData.AlphaRatios[2] = { 0.0f, 0.75f };
		//particleInfo.ColorOverLifetimeData.AlphaRatios[3] = { 1, 1 };
		//particleInfo.ColorOverLifetimeData.AlphaRatioCount = 4;
		//particleInfo.ColorOverLifetimeData.ColorRatios[0] = { 1, 0, 0, 0 };
		//particleInfo.ColorOverLifetimeData.ColorRatios[1] = { 0, 1, 0, 0.5 };
		//particleInfo.ColorOverLifetimeData.ColorRatios[2] = { 0, 0, 1, 0.9 };
		//particleInfo.ColorOverLifetimeData.ColorRatios[3] = { 1, 1, 1, 1 };
		//particleInfo.ColorOverLifetimeData.ColorRatioCount = 4;
		//
		auto transform = Matrix::CreateScale(1, 2, 3) * Matrix::CreateFromYawPitchRoll(1.24, 2.46, 3.68) * DirectX::SimpleMath::Matrix::CreateTranslation({ -200, 300,0 });
		obj = mTestGraphics->CreateParticleObject(particleMaterial, particleInfo, transform);
		mParticleObjects.push_back(obj);

		particleInfo.EmissionData.ParticlesPerSecond = 200.f;
		transform = DirectX::SimpleMath::Matrix::CreateTranslation({ -200, 100,0 });
		obj = mTestGraphics->CreateParticleObject(particleMaterial, particleInfo, transform);
		mParticleObjects.push_back(obj);
	}

	{
		ParticleInfo particleInfo = { };
		IParticleObject* obj;

		particleInfo.MainData.StartSize.x = 10.f;
		particleInfo.MainData.StartSpeed.x = 50.f;
		particleInfo.MainData.StartLifeTime.x = 1.f;
		// particleInfo.MainData.StartColor[0] = { 1, 0, 1, 1 };
		particleInfo.ShapeData.ShapeType = ParticleInfo::Shape::EShape::Box;
		particleInfo.ShapeData.Scale = { 20, 20, 20 };
		particleInfo.ShapeData.ArcInDegree = 242;
		particleInfo.ShapeData.Rotation = { 10, 10, 10 };
		particleInfo.ShapeData.Radius = 10.f;

		particleInfo.SizeOverLifetimeData.bIsUsed = true;
		particleInfo.SizeOverLifetimeData.PointA = { 50, 0.f };
		particleInfo.SizeOverLifetimeData.PointB = { 1, 0.5f };
		particleInfo.SizeOverLifetimeData.PointC = { 1, 0.9f };
		particleInfo.SizeOverLifetimeData.PointD = { 20, 1.f };

		// particleInfo.VelocityOverLifetimeData.bIsUsed = true;
		// particleInfo.VelocityOverLifetimeData.Velocity = { 0, 10, 0 };
		// particleInfo.VelocityOverLifetimeData.Orbital = { 0, 1, 0 };
		// particleInfo.VelocityOverLifetimeData.Offset = { 10, 0, 0 };

		particleInfo.ColorOverLifetimeData.bIsUsed = true;
		particleInfo.ColorOverLifetimeData.AlphaRatios.push_back({ 1.f, 0 });
		particleInfo.ColorOverLifetimeData.AlphaRatios.push_back({ 1.f, 1 });
		particleInfo.ColorOverLifetimeData.ColorRatios.push_back({ 1, 0.7, 0.6, 0 });
		particleInfo.ColorOverLifetimeData.ColorRatios.push_back({ 1, 0.2, 0.2, 0.05 });
		particleInfo.ColorOverLifetimeData.ColorRatios.push_back({ 1, 0.2, 0.2, 0.9f });
		particleInfo.ColorOverLifetimeData.ColorRatios.push_back({ 0.1, 0.1, 0.1, 1 });
		auto transform = Matrix::CreateScale(1, 2, 3) * Matrix::CreateFromYawPitchRoll(1.24, 2.46, 3.68) * DirectX::SimpleMath::Matrix::CreateTranslation({ -200, 300,0 });
		obj = mTestGraphics->CreateParticleObject(particleMaterial, particleInfo, transform);
		mParticleObjects.push_back(obj);

		particleInfo.EmissionData.ParticlesPerSecond = 200.f;
		transform = DirectX::SimpleMath::Matrix::CreateTranslation({ -200, 100,0 });
		obj = mTestGraphics->CreateParticleObject(particleMaterial, particleInfo, transform);
		mParticleObjects.push_back(obj);
	}

	{
		ParticleInfo particleInfo = { };
		IParticleObject* obj;

		particleInfo.MainData.StartSize.x = 10.f;
		particleInfo.MainData.StartSpeed.x = 50.f;
		// particleInfo.MainData.StartColor[0] = { 0, 1, 1, 1 };
		particleInfo.ShapeData.ShapeType = ParticleInfo::Shape::EShape::Circle;
		particleInfo.ShapeData.ArcInDegree = 242;
		particleInfo.ShapeData.Scale = { 10, 10, 10 };
		particleInfo.ShapeData.Radius = 10.f;

		particleInfo.SizeOverLifetimeData.bIsUsed = true;
		particleInfo.SizeOverLifetimeData.PointA = { 50, 0.f };
		particleInfo.SizeOverLifetimeData.PointB = { 1, 0.5f };
		particleInfo.SizeOverLifetimeData.PointC = { 1, 0.9f };
		particleInfo.SizeOverLifetimeData.PointD = { 20, 1.f };
		auto transform = Matrix::CreateScale(1, 2, 3) * Matrix::CreateFromYawPitchRoll(1.24, 2.46, 3.68) * DirectX::SimpleMath::Matrix::CreateTranslation({ -200, 300,0 });
		obj = mTestGraphics->CreateParticleObject(particleMaterial, particleInfo, transform);
		mParticleObjects.push_back(obj);

		particleInfo.EmissionData.ParticlesPerSecond = 200.f;
		transform = DirectX::SimpleMath::Matrix::CreateTranslation({ -200, 100,0 });
		obj = mTestGraphics->CreateParticleObject(particleMaterial, particleInfo, transform);
		mParticleObjects.push_back(obj);
	}

	{
		ParticleInfo particleInfo = { };
		IParticleObject* obj;

		particleInfo.MainData.StartSize.x = 10.f;
		particleInfo.MainData.StartSpeed.x = 50.f;
		// particleInfo.MainData.StartColor[0] = { 1, 1, 1, 1 };
		particleInfo.ShapeData.ArcInDegree = 242;
		particleInfo.ShapeData.Rotation = { 10, 10, 10 };
		particleInfo.ShapeData.Scale = { 10, 10, 10 };
		particleInfo.ShapeData.Radius = 10.f;
		particleInfo.ShapeData.ShapeType = ParticleInfo::Shape::EShape::Rectangle;
		auto transform = Matrix::CreateScale(1, 2, 3) * Matrix::CreateFromYawPitchRoll(1.24, 2.46, 3.68) * DirectX::SimpleMath::Matrix::CreateTranslation({ -200, 300,0 });
		obj = mTestGraphics->CreateParticleObject(particleMaterial, particleInfo, transform);
		mParticleObjects.push_back(obj);

		particleInfo.EmissionData.ParticlesPerSecond = 200.f;
		transform = DirectX::SimpleMath::Matrix::CreateTranslation({ -200, 100,0 });
		obj = mTestGraphics->CreateParticleObject(particleMaterial, particleInfo, transform);
		mParticleObjects.push_back(obj);
	}
}

void Process::particleUpdate()
{
	using namespace fq::graphics;
	//	return;

	static float s_particlesPerSecond = 10.f;
	static bool s_bUsedX = true;
	s_particlesPerSecond += 0.001f;
	s_bUsedX ^= true;

	for (IParticleObject* obj : mParticleObjects)
	{
		ParticleInfo particleInfo = { };
		particleInfo.MainData.StartSpeedOption = ParticleInfo::EOption::RandomBetweenTwoConstant;
		particleInfo.MainData.StartSpeed.x = 1.f;
		particleInfo.MainData.StartSpeed.y = s_particlesPerSecond;

		particleInfo.MainData.StartSizeOption = ParticleInfo::EOption::RandomBetweenTwoConstant;
		particleInfo.MainData.StartSize.x = 1.f;
		particleInfo.MainData.StartSize.y = s_particlesPerSecond;

		particleInfo.EmissionData.ParticlesPerSecond = s_particlesPerSecond;
		//	particleInfo.VelocityOverLifetimeData.Velocity.y = s_particlesPerSecond;
		// particleInfo.VelocityOverLifetimeData.bIsUsed = true;

		particleInfo.ShapeData.ArcInDegree = s_particlesPerSecond * 10.f;
		particleInfo.ShapeData.Radius = 1.f + s_particlesPerSecond;

		if (GetAsyncKeyState('P') & 0x8000)
		{
		}
		else
		{
		}

		particleInfo.InstanceData.bIsRenderDebug = true;
		obj->SetFrameTime(mTimeManager.GetDeltaTime());
		particleInfo.InstanceData.bIsEmit = GetAsyncKeyState('O') & 0x8000;
		particleInfo.InstanceData.bIsReset = GetAsyncKeyState('I') & 0x8000;

		obj->SetInfo(particleInfo);
	}
}

void Process::animationUpdate()
{
	using namespace fq::graphics;

	static float s_animTime = 0.f;
	s_animTime += mTimeManager.GetDeltaTime();

	s_animTime = fmod(s_animTime, 5.f);

	for (ISkinnedMeshObject* iSkinnedMeshObject : mSkinnedMeshObjects)
	{
		const auto& nodeHierarchyInstance = iSkinnedMeshObject->GetNodeHierarchyInstance();
		const auto& nodeHierarchy = nodeHierarchyInstance->GetNodeHierarchy();
		const auto& registerAnimations = nodeHierarchy->GetRegisterAnimations();

		if (!registerAnimations.empty())
		{
			nodeHierarchyInstance->Update(s_animTime, *registerAnimations.begin());
		}
	}
}

void Process::materialUpdate()
{
	static float tempColor = 0.0f;

	tempColor += mTimeManager.GetDeltaTime();
	tempColor = fmod(tempColor, 1.f);

	for (auto matrialInterface : mTestGraphics->GetMaterials())
	{
		auto materialData = matrialInterface->GetInfo();

		if (GetAsyncKeyState('U') & 0x8000)
		{
			materialData.bUseBaseColor = false;
			materialData.bUseMetalness = false;
			materialData.bUseRoughness = false;
			materialData.BaseColor = { tempColor, tempColor, tempColor,tempColor };
			materialData.Metalness = tempColor;
			materialData.Roughness = tempColor;
			//materialData.BaseColorFileName = L"./resource/example/texture/boxBaseColor.jpg";
			//materialData.NormalFileName = L"./resource/example/texture/boxNormal.jpg";
		}
		else
		{
			materialData.bUseMetalness = true;
			materialData.bUseRoughness = true;
			materialData.bUseBaseColor = true;
		}

		matrialInterface->SetInfo(materialData);
	}

	for (auto meshInterface : mSkinnedMeshObjects)
	{
		for (auto matrialInterface : meshInterface->GetMaterials())
		{
			auto materialData = matrialInterface->GetInfo();

			if (GetAsyncKeyState('Y') & 0x8000)
			{
				materialData.bUseBaseColor = false;
				materialData.bUseMetalness = false;
				materialData.bUseRoughness = false;
				materialData.BaseColor = { tempColor, tempColor, tempColor,tempColor };
				materialData.Metalness = tempColor;
				materialData.Roughness = tempColor;
				materialData.BaseColorFileName = L"./resource/example/texture/boxBaseColor.jpg";
				materialData.NormalFileName = L"./resource/example/texture/boxNormal.jpg";
			}
			else
			{
				materialData.bUseMetalness = true;
				materialData.bUseRoughness = true;
				materialData.bUseBaseColor = true;
			}

			matrialInterface->SetInfo(materialData);
		}
	}
	for (auto meshInterface : mStaticMeshObjects)
	{
		for (auto matrialInterface : meshInterface->GetMaterials())
		{
			auto materialData = matrialInterface->GetInfo();

			if (GetAsyncKeyState('T') & 0x8000)
			{
				materialData.bUseBaseColor = false;
				materialData.bUseMetalness = false;
				materialData.bUseRoughness = false;
				materialData.BaseColor = { tempColor, tempColor, tempColor,tempColor };
				materialData.Metalness = tempColor;
				materialData.Roughness = tempColor;
				// materialData.BaseColorFileName = L"./resource/example/texture/boxBaseColor.jpg";
				// materialData.NormalFileName = L"./resource/example/texture/boxNormal.jpg";
			}
			else
			{
				materialData.bUseMetalness = true;
				materialData.bUseRoughness = true;
				materialData.bUseBaseColor = true;
			}

			matrialInterface->SetInfo(materialData);
		}
	}
}

void Process::socketUpdate()
{

}

void Process::decalInit()
{
	using namespace fq::graphics;
	using namespace DirectX::SimpleMath;

	DecalMaterialInfo decalMaterialInfo;
	decalMaterialInfo.BaseColorFileName = L"./resource/example/texture/boxBaseColor.jpg";
	decalMaterialInfo.NormalFileName = L"./resource/example/texture/boxNormal.jpg";
	std::shared_ptr<IDecalMaterial> decalMaterial = mTestGraphics->CreateDecalMaterial(decalMaterialInfo);

	{
		DecalInfo decalInfo;
		auto transform = Matrix::CreateScale(400) * Matrix::CreateRotationX(3.14 * 0.45f) * Matrix::CreateTranslation({ -400, 0, -400 });
		IDecalObject* decalObject = mTestGraphics->CreateDecalObject(decalMaterial, decalInfo, transform);
		mDecalObjects.push_back(decalObject);
	}
	{
		DecalInfo decalInfo;
		auto transform = Matrix::CreateScale(400) * Matrix::CreateRotationX(3.14 * 0.3) * Matrix::CreateTranslation({ -400, 0, 400 });
		IDecalObject* decalObject = mTestGraphics->CreateDecalObject(decalMaterial, decalInfo, transform);
		mDecalObjects.push_back(decalObject);
	}
	{
		DecalInfo decalInfo;
		auto transform = Matrix::CreateScale(400) * Matrix::CreateRotationX(3.14 * 0.30f) * Matrix::CreateTranslation({ 400, 0, 400 });
		IDecalObject* decalObject = mTestGraphics->CreateDecalObject(decalMaterial, decalInfo, transform);
		mDecalObjects.push_back(decalObject);
	}
}

void Process::decalUpdate()
{
	using namespace fq::graphics;
	using namespace DirectX::SimpleMath;

	static float s_rotate = 100;
	s_rotate += mTimeManager.GetDeltaTime();

	int i = -(int)mDecalObjects.size() + mDecalObjects.size() / 2;
	for (IDecalObject* decalObject : mDecalObjects)
	{
		DecalInfo decalInfo = decalObject->GetDecalInfo();
		auto transform = Matrix::CreateScale(200) * Matrix::CreateRotationX(s_rotate) * Matrix::CreateTranslation({ (float)i * 200, 0, 0 });

		decalInfo.Width = 200;
		decalInfo.Height = 200;
		decalInfo.Depth = 200;

		decalInfo.NormalThresholdInDegree = 90;
		decalObject->SetDecalInfo(decalInfo);
		decalObject->SetTransform(transform);

		++i;
	}
}

void Process::trailInit()
{
	using namespace fq::graphics;

	ParticleMaterialInfo particleMaterialInfo;
	particleMaterialInfo.BaseColorFileName = L"./resource/example/texture/trail01.png";
	particleMaterialInfo.bIsTwoSide = true;
	particleMaterialInfo.BaseColor = { 1, 1, 1, 1 };
	std::shared_ptr<IParticleMaterial> particleMaterial = mTestGraphics->CreateParticleMaterial(particleMaterialInfo);

	{
		TrailInfo trailInfo;
		auto* interfaceClass = mTestGraphics->CreateTrailObject(particleMaterial, trailInfo, DirectX::SimpleMath::Matrix::Identity);
		mTrailObjects.push_back(interfaceClass);
	}
}

void Process::trailUpdate()
{
	using namespace fq::graphics;

	static float s_prev_time = 0.f;
	static float s_time = 0.f;

	float deltaTime = mTimeManager.GetDeltaTime();
	deltaTime = fmod(deltaTime, 0.02);

	s_prev_time = s_time;
	s_time += deltaTime;

	for (auto* trailObject : mTrailObjects)
	{
		TrailInfo trailInfo = trailObject->GetTrailInfo();

		trailInfo.Time = 0.5f;
		trailInfo.MinVertexDistance = 25;
		trailInfo.AlignmentType = TrailInfo::EAlignment::TransformZ;
		trailInfo.TextureMode = TrailInfo::ETextureMode::Stretch;
		trailInfo.VertexDivisionCount = 5;
		trailInfo.WidthRatios.clear();
		trailInfo.WidthRatios.push_back({ 10000, 0.2 });
		trailInfo.WidthRatios.push_back({ 7000, 0.4 });
		trailInfo.WidthRatios.push_back({ 5000, 0.9 });

		trailInfo.ColorRatios.clear();
		trailInfo.ColorRatios.push_back({ 1, 0, 0, 0.2 });
		trailInfo.ColorRatios.push_back({ 0.5, 0, 0, 0.5 });
		trailInfo.ColorRatios.push_back({ 0.1, 0, 1, 0.9 });


		//if (s_time > 1.f)
		//{
		//	trailInfo.bIsEmit ^= true;
		//	s_time -= 1.f;
		//}

		trailObject->SetFrameTime(mTimeManager.GetDeltaTime());

		if (s_time < 1.f)
		{
			trailObject->SetTransform(DirectX::SimpleMath::Matrix::CreateTranslation(0, 0, 10));
		}
		else if (s_time < 2.f)
		{
			trailObject->SetTransform(DirectX::SimpleMath::Matrix::CreateTranslation(0, 400, 10));
		}
		else if (s_time < 3.f)
		{
			trailObject->SetTransform(DirectX::SimpleMath::Matrix::CreateTranslation(400, 400, 10));
		}
		else if (s_time < 4.f)
		{
			trailObject->SetTransform(DirectX::SimpleMath::Matrix::CreateTranslation(400, 0, 10));
		}
		else
		{
			trailObject->SetTransform(DirectX::SimpleMath::Matrix::CreateTranslation(200, -200, 10));
		}

		trailObject->SetTrailInfo(trailInfo);
	}

	if (s_time >= 5.f)
	{
		s_time = 0.f;
	}
}

void Process::createModel(std::string modelPath, std::filesystem::path textureBasePath, DirectX::SimpleMath::Matrix transform)
{
	using namespace fq::graphics;
	unsigned int key = std::hash<std::string>{}(modelPath);

	const fq::common::Model& modelData = mTestGraphics->CreateModelResource(key, modelPath, textureBasePath);

	auto boneHierarchy = mTestGraphics->GetNodeHierarchyByModelPathOrNull(key);

	for (auto animation : modelData.Animations)
	{
		auto animationInterface = mTestGraphics->GetAnimationByModelPathOrNull(key, animation.Name);
		boneHierarchy->RegisterAnimation(animationInterface);
	}

	auto boneHierarchyCache = boneHierarchy->CreateNodeHierarchyInstance();

	for (const auto& [node, mesh] : modelData.Meshes)
	{
		if (mesh.Vertices.empty())
		{
			continue;
		}

		std::vector<std::shared_ptr<IMaterial>> materialInterfaces;
		materialInterfaces.reserve(mesh.Subsets.size());
		MeshObjectInfo meshObjectInfo;


		for (const auto& subset : mesh.Subsets)
		{
			auto materialInterface = mTestGraphics->GetMaterialByModelPathOrNull(key, subset.MaterialName);
			materialInterfaces.push_back(materialInterface);
		}

		if (mesh.BoneVertices.empty())
		{
			auto meshInterface = mTestGraphics->GetStaticMeshByModelPathOrNull(key, mesh.Name);
			IStaticMeshObject* iStaticMeshObject = mTestGraphics->CreateStaticMeshObject(meshInterface, materialInterfaces, meshObjectInfo, node.ToParentMatrix * transform);
			mStaticMeshObjects.push_back(iStaticMeshObject);
		}
		else
		{
			auto meshInterface = mTestGraphics->GetSkinnedMeshByModelPathOrNull(key, mesh.Name);
			ISkinnedMeshObject* iSkinnedMeshObject = mTestGraphics->CreateSkinnedMeshObject(meshInterface, materialInterfaces, meshObjectInfo, transform);
			iSkinnedMeshObject->SetNodeHierarchyInstance(boneHierarchyCache);
			mSkinnedMeshObjects.push_back(iSkinnedMeshObject);
		}
	}
}

void Process::calculateFrameStats()
{
	float fps = (float)mTimeManager.GetFps();
	float mspf = 1000.0f / fps;

	std::wostringstream outs;
	outs.precision(6);
	outs << L"FPS: " << fps << L"    "
		<< L"Frame Time: " << mspf << L" (ms)";
	SetWindowText(mHwnd, outs.str().c_str());
}

void Process::convertFBXModelAll(std::filesystem::path readFolderPath, std::filesystem::path outFolderPath)
{
	if (!std::filesystem::exists(readFolderPath))
	{
		return;
	}

	for (const auto& iter : std::filesystem::directory_iterator{ readFolderPath })
	{
		if (iter.path().extension() == ".fbx")
		{
			auto outPath = iter.path();
			outPath.replace_extension(".model");
			outPath = outFolderPath / outPath.filename();

			auto convertedData = mTestGraphics->ConvertModel(iter.path().string());
			mTestGraphics->WriteModel(outPath.string(), convertedData);
		}
	}
}

void Process::convertFBXModelAll(std::filesystem::path folderPath)
{
	if (!std::filesystem::exists(folderPath))
	{
		return;
	}

	for (const auto& iter : std::filesystem::recursive_directory_iterator{ folderPath })
	{
		if (iter.path().extension() == ".fbx")
		{
			auto outPath = iter.path();
			outPath.replace_extension(".model");
			auto convertedData = mTestGraphics->ConvertModel(iter.path().string());
			mTestGraphics->WriteModel(outPath.string(), convertedData);
		}
	}
}

void Process::renderObjectInit()
{
	// convertFBXModelAll("./resource/example/fbx/", "./resource/example/model/");
	// convertFBXModelAll("C:/Git/FourQuest/code/FourQuest/FQGameEngineDemo/resource");

	const std::string geoModelPath = "./resource/example/model/geoBox.model";
	const std::string planeModelPath = "./resource/example/model/Plane.model";
	const std::string modelPath = "./resource/example/model/gun.model";
	const std::string animModelPath0 = "./resource/example/model/RangeMonster(Union_1).model";
	const std::string animModelPath1 = "./resource/example/model/temp123.model";
	const std::string textureBasePath = "./resource/example/texture";

	// createModel(geoModelPath, textureBasePath);
	// createModel(planeModelPath, textureBasePath, DirectX::SimpleMath::Matrix::CreateScale(1000));
	// createModel(modelPath, textureBasePath, DirectX::SimpleMath::Matrix::CreateTranslation(200, 0, 0));

	for (size_t i = 0; i < 1; ++i)
	{
		createModel(animModelPath0, textureBasePath, DirectX::SimpleMath::Matrix::CreateScale(100) * DirectX::SimpleMath::Matrix::CreateTranslation(i * 50, 0, 0));
	}
}

void Process::renderObjectUpdate()
{
	animationUpdate();
	materialUpdate();
	socketUpdate();
}

void Process::VFXInit()
{
	particleInit();
	decalInit();
	trailInit();
}

void Process::VFXUpdate()
{
	particleUpdate();
	decalUpdate();
	trailUpdate();
}
