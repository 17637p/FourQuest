#include "EffectDemo.h"

#include <IFQGraphics.h>

#include "InputManager.h"
#include "DemoUtils.h"
#include "../FQLoader/ModelLoader.h"

using namespace fq::utils;

EffectDemo::EffectDemo()
	:mWindowPosX(0),
	mWindowPosY(0),
	mScreenWidth(1920),
	mScreenHeight(1080),
	mResizing(false),
	mTestGraphics(nullptr)
{
	HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

	CreateHWND(L"FQ RenderObject Demo", WS_OVERLAPPEDWINDOW, mWindowPosX, mWindowPosY, mScreenWidth, mScreenHeight);
	ShowWindow(mHwnd, SW_SHOWNORMAL);

	mEngineExporter = std::make_shared<fq::graphics::EngineExporter>();
}

EffectDemo::~EffectDemo()
{
	mEngineExporter->DeleteEngine(mTestGraphics);

	CoUninitialize();
}

bool EffectDemo::Init(HINSTANCE hInstance)
{
	/// 기본적인 초기화
	InputManager::GetInstance().Init(mHwnd);
	mTimeManager.Init();

	mTestGraphics = mEngineExporter->GetEngine();
	mTestGraphics->Initialize(mHwnd, mScreenWidth, mScreenHeight, fq::graphics::EPipelineType::Deferred);

	mTestGraphics->WriteModel("./resource/Graphics/EffectDemo/untitled.model", mTestGraphics->ConvertModel("./resource/Graphics/EffectDemo/untitled.fbx"));
	mTestGraphics->WriteModel("./resource/Graphics/EffectDemo/Holland_Test.model", mTestGraphics->ConvertModel("./resource/Graphics/EffectDemo/Holland_Test.fbx"));
	mTestGraphics->WriteModel("./resource/Graphics/EffectDemo/Sprite.model", mTestGraphics->ConvertModel("./resource/Graphics/EffectDemo/Sprite.fbx"));
	mTestGraphics->WriteModel("./resource/Graphics/EffectDemo/aaaaaa.model", mTestGraphics->ConvertModel("./resource/Graphics/EffectDemo/aaaaaa.fbx"));

	createModel("./resource/Graphics/EffectDemo/untitled.model", "./resource/Graphics/EffectDemo/", DirectX::SimpleMath::Matrix::Identity, true);
	// auto animInterface = mTestGraphics->CreateAnimation("anim", animation);
	// mStaticMeshObjects.back()->GetNodeHierarchyInstance()->GetNodeHierarchy()->RegisterAnimation(animInterface);

	createModel("./resource/Graphics/EffectDemo/Plane.model", "./resource/Graphics/EffectDemo/", DirectX::SimpleMath::Matrix::CreateScale(10) * DirectX::SimpleMath::Matrix::CreateRotationX(-3.14 * 0.5f), false);

	auto materialInfo = mTestGraphics->GetMaterials()[0]->GetInfo();
	fq::loader::MaterialLoader::Write("./material.material", materialInfo);
	auto materialInfoRead = fq::loader::MaterialLoader::Read("./material.material");

	auto uvAnimation = fq::loader::UVAnimationLoader::Read("./resource/Graphics/EffectDemo/aaaaaUVAnim.txt");
	fq::loader::UVAnimationLoader::Write(uvAnimation, "./resource/Graphics/EffectDemo/aaaaaUVAnim.uvAnimation");
	auto animation = mTestGraphics->ReadAnimation("./resource/Graphics/EffectDemo/TestTransformAnim.txt");
	mTestGraphics->WriteAnimation("./resource/Graphics/EffectDemo/TestTransformAnim.animation", animation);
	auto modelData = mTestGraphics->GetModel(std::hash<std::string>{}("./resource/Graphics/EffectDemo/untitled.model"));
	mTestGraphics->WriteNodeHierarchy("./resource/Graphics/EffectDemo/TestTransformAnim.nodeHierarchy", modelData);
	auto nodeHierarchyData = mTestGraphics->ReadNodeHierarchy("./resource/Graphics/EffectDemo/TestTransformAnim.nodeHierarchy");
	mTestGraphics->CreateNodeHierarchy(nodeHierarchyData);

	auto uvAnimResource = mTestGraphics->CreateUVAnimation("uvAnim", uvAnimation);
	mUVAnimInstance = uvAnimResource->CreateUVAnimationInstance();
	mStaticMeshObjects.back()->SetUVAnimationInstance(mUVAnimInstance);

	for (auto& materialInterface : mStaticMeshObjects.back()->GetMaterials())
	{
		auto info = materialInterface->GetInfo();
		info.RasterizeType = fq::graphics::ERasterizeMode::TwoSide;
		materialInterface->SetInfo(info);
	}

	//mMeshEffectObjects.push_back(mTestGraphics->CreateMeshEffectObject("./resource/Graphics/EffectDemo/untitled.model", "./resource/Graphics/EffectDemo/TestUVAnim.txt", "./resource/Graphics/EffectDemo/TestTransformAnim.txt", "./resource/Graphics/EffectDemo/", DirectX::SimpleMath::Matrix::CreateScale(10)));
	//mMeshEffectObjects.push_back(mTestGraphics->CreateMeshEffectObject("./resource/Graphics/EffectDemo/Holland_Test.model", "./resource/Graphics/EffectDemo/uv_animation_data.txt", "./resource/Graphics/EffectDemo/transform_data.txt", "./resource/Graphics/EffectDemo/", DirectX::SimpleMath::Matrix::CreateScale(100)));
	// mMeshEffectObjects.push_back(mTestGraphics->CreateMeshEffectObject("./resource/Graphics/EffectDemo/Sprite.model", "./resource/Graphics/EffectDemo/uv_animation_data.txt", "./resource/Graphics/EffectDemo/transform_data.txt", "./resource/Graphics/EffectDemo/", DirectX::SimpleMath::Matrix::CreateScale(100)));

	materialInit();
	// particleInit();
	decalInit();
	// trailInit();

	/// camera 초기화
	AddDefaultCamera(mTestGraphics);

	// Camera Transform 설정
	mCameraTransform.worldPosition = { 0, 0, -50 };
	mCameraTransform.worldRotation = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(DirectX::XMConvertToRadians(0.0f), DirectX::XMConvertToRadians(0.0f), DirectX::XMConvertToRadians(0.0f));
	mCameraTransform.worldScale = { 1, 1, 1 };

	mCameraTransform.worldMatrix =
		DirectX::SimpleMath::Matrix::CreateScale(mCameraTransform.worldScale) *
		DirectX::SimpleMath::Matrix::CreateFromQuaternion(mCameraTransform.worldRotation) *
		DirectX::SimpleMath::Matrix::CreateTranslation(mCameraTransform.worldPosition);

	/// Light 초기화 
	fq::graphics::LightInfo directionalLightInfo;

	directionalLightInfo.type = fq::graphics::ELightType::Directional;
	directionalLightInfo.color = { 1, 1, 1, 1 };
	directionalLightInfo.intensity = 1;
	directionalLightInfo.direction = { 0, -0.7, -0.7 };
	directionalLightInfo.direction.Normalize();

	mTestGraphics->AddLight(1, directionalLightInfo);

	directionalLightInfo.type = fq::graphics::ELightType::Directional;
	directionalLightInfo.color = { 1, 1, 1, 1 };
	directionalLightInfo.intensity = 1;
	directionalLightInfo.direction = { 0, 0.7, -0.7 };
	directionalLightInfo.direction.Normalize();

	mTestGraphics->AddLight(2, directionalLightInfo);

	directionalLightInfo.type = fq::graphics::ELightType::Directional;
	directionalLightInfo.color = { 1, 1, 1, 1 };
	directionalLightInfo.intensity = 1;
	directionalLightInfo.direction = { 0, -0.7, 0.7 };
	directionalLightInfo.direction.Normalize();

	mTestGraphics->AddLight(3, directionalLightInfo);

	return true;
}

void EffectDemo::Loop()
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

void EffectDemo::Finalize()
{

}

LRESULT EffectDemo::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HDC			hdc;
	PAINTSTRUCT ps;

	//if (ImGui_ImplWin32_WndProcHandler(m_hWnd, uMsg, wParam, lParam))
	//	return true;

	switch (uMsg)
	{
	case WM_SIZE:
	{
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

void EffectDemo::Update()
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

	if (GetAsyncKeyState('L') & 0x8000)
	{
		mTestGraphics->SetIsRenderDebug(false);
	}
	else
	{
		mTestGraphics->SetIsRenderDebug(true);
	}

	using namespace fq::graphics;

	// particle
	for (IParticleObject* obj : mParticleObjects)
	{
		ParticleInfo& particleInfo = obj->GetInfo();
		particleInfo.InstanceData.bIsEmit = GetAsyncKeyState('P') & 0x8000;
		particleInfo.InstanceData.bIsReset = GetAsyncKeyState('R') & 0x8000;

		obj->SetFrameTime(mTimeManager.GetDeltaTime());
	}
		static float s_time = 0.f;
	s_time += mTimeManager.GetDeltaTime();

	std::vector<DirectX::SimpleMath::Matrix> transforms;
	transforms.push_back(DirectX::SimpleMath::Matrix::CreateTranslation(-100, -100, 0));
	transforms.push_back(DirectX::SimpleMath::Matrix::CreateTranslation(-100, 100, 0));
	transforms.push_back(DirectX::SimpleMath::Matrix::CreateTranslation(100, 100, 0));
	transforms.push_back(DirectX::SimpleMath::Matrix::CreateTranslation(100, -100, 0));

	transforms.push_back(DirectX::SimpleMath::Matrix::CreateTranslation(-200, -200, 0));
	transforms.push_back(DirectX::SimpleMath::Matrix::CreateTranslation(-200, 200, 0));
	transforms.push_back(DirectX::SimpleMath::Matrix::CreateTranslation(200, 200, 0));
	transforms.push_back(DirectX::SimpleMath::Matrix::CreateTranslation(200, -200, 0));

	transforms.push_back(DirectX::SimpleMath::Matrix::CreateTranslation(-300, -300, 0));
	transforms.push_back(DirectX::SimpleMath::Matrix::CreateTranslation(-300, 300, 0));
	transforms.push_back(DirectX::SimpleMath::Matrix::CreateTranslation(300, 300, 0));
	transforms.push_back(DirectX::SimpleMath::Matrix::CreateTranslation(300, -300, 0));

	transforms.push_back(DirectX::SimpleMath::Matrix::CreateTranslation(-400, -400, 0));
	transforms.push_back(DirectX::SimpleMath::Matrix::CreateTranslation(-400, 400, 0));
	transforms.push_back(DirectX::SimpleMath::Matrix::CreateTranslation(400, 400, 0));
	transforms.push_back(DirectX::SimpleMath::Matrix::CreateTranslation(400, -400, 0));

	static float trailTime = 0.f;
	float framePerSecond = 1.f / 60.f;
	bool bIsTrailUpdate = false;
	trailTime += mTimeManager.GetDeltaTime();

	if (trailTime > framePerSecond)
	{
		bIsTrailUpdate = true;
		trailTime -= framePerSecond;
	}

	for (size_t i = 0; i < mTrailObjects.size(); ++i)
	{
		ITrailObject* obj = mTrailObjects[i];

		TrailInfo& trailInfo = obj->GetTrailInfo();
		trailInfo.bIsEmit = GetAsyncKeyState('P') & 0x8000;
		trailInfo.bIsReset = GetAsyncKeyState('R') & 0x8000;
		obj->SetFrameTime(mTimeManager.GetDeltaTime());

		if (bIsTrailUpdate)
		{
			obj->SetTransform(DirectX::SimpleMath::Matrix::CreateTranslation(100 + i * 10, 0, 0) * DirectX::SimpleMath::Matrix::CreateRotationZ(s_time));
		}
	}

	// mStaticMeshObjects.back()->GetNodeHierarchyInstance()->Update(fmod(s_time * 0.5f, 2.f), mTestGraphics->GetAnimationOrNull("anim"));
	if (mUVAnimInstance != nullptr)
	{
		mUVAnimInstance->SetTimePos(fmod(s_time * 0.5f, 2.f));
	}

	// 카메라 조작
	float speed = mTimeManager.GetDeltaTime() * 100;
	if (InputManager::GetInstance().IsGetKey(VK_SHIFT))
	{
		speed *= 10;
	}

	if (InputManager::GetInstance().IsGetKey('W'))
	{
		Walk(mCameraTransform, speed);
	}
	if (InputManager::GetInstance().IsGetKey('S'))
	{
		Walk(mCameraTransform, -speed);
	}
	if (InputManager::GetInstance().IsGetKey('D'))
	{
		Strafe(mCameraTransform, speed);
	}
	if (InputManager::GetInstance().IsGetKey('A'))
	{
		Strafe(mCameraTransform, -speed);
	}
	if (InputManager::GetInstance().IsGetKey('E'))
	{
		WorldUpdown(mCameraTransform, speed);
	}
	if (InputManager::GetInstance().IsGetKey('Q'))
	{
		WorldUpdown(mCameraTransform, -speed);
	}

	if (InputManager::GetInstance().IsGetKey(VK_RBUTTON))
	{
		float dx = (0.25f * static_cast<float>(InputManager::GetInstance().GetDeltaPosition().x) * (3.141592f / 180.0f));
		float dy = (0.25f * static_cast<float>(InputManager::GetInstance().GetDeltaPosition().y) * (3.141592f / 180.0f));

		Pitch(mCameraTransform, dy);
		Yaw(mCameraTransform, dx);
	}

	mTestGraphics->UpdateCamera(mCameraTransform);
}

void EffectDemo::Render()
{
	mTestGraphics->BeginRender();
	debugRender();
	mTestGraphics->Render();
	mTestGraphics->EndRender();
}

void EffectDemo::debugRender()
{
	using namespace fq::graphics::debug;

	GridInfo gridInfo;
	gridInfo.Origin = { 0, 0,0 };
	gridInfo.XAxis = { 1, 0, 0 };
	gridInfo.YAxis = { 0, 0, 1 };
	gridInfo.XDivision = 10;
	gridInfo.YDivision = 10;
	gridInfo.GridSize = 1.f;
	gridInfo.Color = { 1, 1, 1, 1 };
	mTestGraphics->DrawGrid(gridInfo);
}

void EffectDemo::calculateFrameStats()
{
	float fps = (float)mTimeManager.GetFps();
	float mspf = 1000.0f / fps;

	std::wostringstream outs;
	outs.precision(6);
	outs << L"FPS: " << fps << L"    "
		<< L"Frame Time: " << mspf << L" (ms)";
	SetWindowText(mHwnd, outs.str().c_str());
}

void EffectDemo::materialInit()
{
	using namespace fq::graphics;
	using namespace DirectX::SimpleMath;

	{
		ParticleMaterialInfo particleMaterialInfo;
		particleMaterialInfo.bIsTwoSide = true;
		particleMaterialInfo.BaseColorFileName = L"./resource/Graphics/EffectDemo/Particle01.png";
		mTestGraphics->CreateParticleMaterial("Particle01", particleMaterialInfo);
	}
	{
		ParticleMaterialInfo particleMaterialInfo;
		particleMaterialInfo.bIsTwoSide = true;
		particleMaterialInfo.BaseColorFileName = L"./resource/Graphics/EffectDemo/Particle02.png";
		mTestGraphics->CreateParticleMaterial("Particle02", particleMaterialInfo);
	}
	{
		ParticleMaterialInfo particleMaterialInfo;
		particleMaterialInfo.bIsTwoSide = true;
		particleMaterialInfo.BaseColorFileName = L"./resource/Graphics/EffectDemo/Particle03.png";
		mTestGraphics->CreateParticleMaterial("Particle03", particleMaterialInfo);
	}
	{
		ParticleMaterialInfo particleMaterialInfo;
		particleMaterialInfo.bIsTwoSide = true;
		particleMaterialInfo.BaseColorFileName = L"./resource/Graphics/EffectDemo/Particle04.png";
		mTestGraphics->CreateParticleMaterial("Particle04", particleMaterialInfo);
	}
	{
		ParticleMaterialInfo particleMaterialInfo;
		particleMaterialInfo.bIsTwoSide = true;
		particleMaterialInfo.BaseColorFileName = L"./resource/Graphics/EffectDemo/trail01.png";
		mTestGraphics->CreateParticleMaterial("trail01", particleMaterialInfo);
	}
	{
		ParticleMaterialInfo particleMaterialInfo;
		particleMaterialInfo.bIsTwoSide = true;
		particleMaterialInfo.BaseColorFileName = L"./resource/Graphics/EffectDemo/trail02.jpg";
		mTestGraphics->CreateParticleMaterial("trail02", particleMaterialInfo);
	}
	{
		DecalMaterialInfo decalMaterialInfo;
		decalMaterialInfo.BaseColorFileName = L"./resource/Graphics/EffectDemo/boxBaseColor.jpg";
		decalMaterialInfo.NormalFileName = L"./resource/Graphics/EffectDemo/boxNormal.jpg";
		mTestGraphics->CreateDecalMaterial("Decal01", decalMaterialInfo);
	}
}

void EffectDemo::particleInit()
{
	using namespace fq::graphics;
	using namespace DirectX::SimpleMath;

	float x = -300;

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

		auto transform = Matrix::CreateScale(1, 2, 3) * Matrix::CreateFromYawPitchRoll(1.24, 2.46, 3.68) * DirectX::SimpleMath::Matrix::CreateTranslation({ x , 300,0 });
		obj = mTestGraphics->CreateParticleObject(mTestGraphics->GetParticleMaterialOrNull("Particle01"), particleInfo, transform);
		mParticleObjects.push_back(obj);

		particleInfo.EmissionData.ParticlesPerSecond = 200.f;
		transform = DirectX::SimpleMath::Matrix::CreateTranslation({ x , 100,0 });
		obj = mTestGraphics->CreateParticleObject(mTestGraphics->GetParticleMaterialOrNull("Particle02"), particleInfo, transform);
		mParticleObjects.push_back(obj);

		x += 100;
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

		auto transform = Matrix::CreateScale(1, 2, 3) * Matrix::CreateFromYawPitchRoll(1.24, 2.46, 3.68) * DirectX::SimpleMath::Matrix::CreateTranslation({ x, 300,0 });
		obj = mTestGraphics->CreateParticleObject(mTestGraphics->GetParticleMaterialOrNull("Particle03"), particleInfo, transform);
		mParticleObjects.push_back(obj);

		particleInfo.EmissionData.ParticlesPerSecond = 200.f;
		transform = DirectX::SimpleMath::Matrix::CreateTranslation({ x, 100,0 });
		obj = mTestGraphics->CreateParticleObject(mTestGraphics->GetParticleMaterialOrNull("Particle04"), particleInfo, transform);
		mParticleObjects.push_back(obj);

		x += 100;
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

		auto transform = Matrix::CreateScale(1, 2, 3) * Matrix::CreateFromYawPitchRoll(1.24, 2.46, 3.68) * DirectX::SimpleMath::Matrix::CreateTranslation({ x, 300,0 });
		obj = mTestGraphics->CreateParticleObject(mTestGraphics->GetParticleMaterialOrNull("Particle01"), particleInfo, transform);
		mParticleObjects.push_back(obj);

		particleInfo.EmissionData.ParticlesPerSecond = 200.f;
		transform = DirectX::SimpleMath::Matrix::CreateTranslation({ x, 100,0 });
		obj = mTestGraphics->CreateParticleObject(mTestGraphics->GetParticleMaterialOrNull("Particle02"), particleInfo, transform);
		mParticleObjects.push_back(obj);


		x += 100;

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

		particleInfo.ColorOverLifetimeData.bIsUsed = true;
		particleInfo.ColorOverLifetimeData.AlphaRatios.push_back({ 1.f, 0 });
		particleInfo.ColorOverLifetimeData.AlphaRatios.push_back({ 0.5f, 0.25f });
		particleInfo.ColorOverLifetimeData.AlphaRatios.push_back({ 0.0f, 0.75f });
		particleInfo.ColorOverLifetimeData.AlphaRatios.push_back({ 1, 1 });
		particleInfo.ColorOverLifetimeData.ColorRatios.push_back({ 1, 0, 0, 0 });
		particleInfo.ColorOverLifetimeData.ColorRatios.push_back({ 0, 1, 0, 0.5 });
		particleInfo.ColorOverLifetimeData.ColorRatios.push_back({ 0, 0, 1, 0.9 });
		particleInfo.ColorOverLifetimeData.ColorRatios.push_back({ 1, 1, 1, 1 });

		auto transform = Matrix::CreateScale(1, 2, 3) * Matrix::CreateFromYawPitchRoll(1.24, 2.46, 3.68) * DirectX::SimpleMath::Matrix::CreateTranslation({ x, 300,0 });
		obj = mTestGraphics->CreateParticleObject(mTestGraphics->GetParticleMaterialOrNull("Particle03"), particleInfo, transform);
		mParticleObjects.push_back(obj);

		particleInfo.EmissionData.ParticlesPerSecond = 200.f;
		transform = DirectX::SimpleMath::Matrix::CreateTranslation({ x, 100,0 });
		obj = mTestGraphics->CreateParticleObject(mTestGraphics->GetParticleMaterialOrNull("Particle04"), particleInfo, transform);
		mParticleObjects.push_back(obj);

		x += 100;

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

		particleInfo.VelocityOverLifetimeData.bIsUsed = true;
		particleInfo.VelocityOverLifetimeData.Velocity = { 0, 10, 0 };
		particleInfo.VelocityOverLifetimeData.Orbital = { 0, 1, 0 };
		particleInfo.VelocityOverLifetimeData.Offset = { 10, 0, 0 };

		particleInfo.ColorOverLifetimeData.bIsUsed = true;
		particleInfo.ColorOverLifetimeData.AlphaRatios.push_back({ 1.f, 0 });
		particleInfo.ColorOverLifetimeData.AlphaRatios.push_back({ 1.f, 1 });
		particleInfo.ColorOverLifetimeData.ColorRatios.push_back({ 1, 0.7, 0.6, 0 });
		particleInfo.ColorOverLifetimeData.ColorRatios.push_back({ 1, 0.2, 0.2, 0.05 });
		particleInfo.ColorOverLifetimeData.ColorRatios.push_back({ 1, 0.2, 0.2, 0.9f });
		particleInfo.ColorOverLifetimeData.ColorRatios.push_back({ 0.1, 0.1, 0.1, 1 });
		auto transform = Matrix::CreateScale(1, 2, 3) * Matrix::CreateFromYawPitchRoll(1.24, 2.46, 3.68) * DirectX::SimpleMath::Matrix::CreateTranslation({ x, 300,0 });
		obj = mTestGraphics->CreateParticleObject(mTestGraphics->GetParticleMaterialOrNull("Particle01"), particleInfo, transform);
		mParticleObjects.push_back(obj);

		particleInfo.EmissionData.ParticlesPerSecond = 200.f;
		transform = DirectX::SimpleMath::Matrix::CreateTranslation({ x, 100,0 });
		obj = mTestGraphics->CreateParticleObject(mTestGraphics->GetParticleMaterialOrNull("Particle03"), particleInfo, transform);
		mParticleObjects.push_back(obj);

		x += 100;

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
		auto transform = Matrix::CreateScale(1, 2, 3) * Matrix::CreateFromYawPitchRoll(1.24, 2.46, 3.68) * DirectX::SimpleMath::Matrix::CreateTranslation({ x, 300,0 });
		obj = mTestGraphics->CreateParticleObject(mTestGraphics->GetParticleMaterialOrNull("Particle02"), particleInfo, transform);
		mParticleObjects.push_back(obj);

		particleInfo.EmissionData.ParticlesPerSecond = 200.f;
		transform = DirectX::SimpleMath::Matrix::CreateTranslation({ x, 100,0 });
		obj = mTestGraphics->CreateParticleObject(mTestGraphics->GetParticleMaterialOrNull("Particle04"), particleInfo, transform);
		mParticleObjects.push_back(obj);

		x += 100;
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
		auto transform = Matrix::CreateScale(1, 2, 3) * Matrix::CreateFromYawPitchRoll(1.24, 2.46, 3.68) * DirectX::SimpleMath::Matrix::CreateTranslation({ x, 300,0 });
		obj = mTestGraphics->CreateParticleObject(mTestGraphics->GetParticleMaterialOrNull("Particle01"), particleInfo, transform);
		mParticleObjects.push_back(obj);

		particleInfo.EmissionData.ParticlesPerSecond = 200.f;
		transform = DirectX::SimpleMath::Matrix::CreateTranslation({ x, 100,0 });
		obj = mTestGraphics->CreateParticleObject(mTestGraphics->GetParticleMaterialOrNull("Particle02"), particleInfo, transform);
		mParticleObjects.push_back(obj);

		x += 100;

	}
}

void EffectDemo::decalInit()
{
	using namespace fq::graphics;
	using namespace DirectX::SimpleMath;
	
	createModel("./resource/Graphics/EffectDemo/Plane.model", "./resource/Graphics/EffectDemo/", DirectX::SimpleMath::Matrix::CreateScale(100) * DirectX::SimpleMath::Matrix::CreateRotationX(3.14f * 0.5));

	DecalMaterialInfo decalMaterialInfo;
	decalMaterialInfo.BaseColorFileName = L"./resource/example/texture/boxBaseColor.jpg";
	decalMaterialInfo.NormalFileName = L"./resource/example/texture/boxNormal.jpg";
	decalMaterialInfo.bIsUsedEmissive = false;
	std::shared_ptr<IDecalMaterial> decalMaterial = mTestGraphics->GetDecalMaterialOrNull("Decal01");

	for (size_t i = 0; i < 1; ++i)
	{
		for (size_t j = 0; j < 1; ++j)
		{
			DecalInfo decalInfo;
			decalInfo.Width = 100;
			decalInfo.Height = 100;
			decalInfo.Depth = 100;
			decalInfo.bIsRenderDebug = true;
			auto transform = DirectX::SimpleMath::Matrix::Identity;// Matrix::CreateRotationX(3.14 * 0.45f)* Matrix::CreateTranslation({ i * 100.f - 500.f, 0, j * 100.f - 500.f });
			IDecalObject* decalObject = mTestGraphics->CreateDecalObject(decalMaterial, decalInfo, transform);
			mDecalObjects.push_back(decalObject);
		}
	}

	const std::string fbxPath = "./resource/Graphics/EffectDemo/untitled.fbx";
	const std::string modelPath = "./resource/Graphics/EffectDemo/untitled.model";
	mTestGraphics->WriteModel(modelPath, mTestGraphics->ConvertModel(fbxPath));
	createModel(modelPath, "", DirectX::SimpleMath::Matrix::CreateScale(1));
}

void EffectDemo::trailInit()
{
	using namespace fq::graphics;

	std::vector<std::shared_ptr<IParticleMaterial>> particleMaterials = mTestGraphics->GetParticleMaterials();

	{
		TrailInfo trailInfo;
		auto* interfaceClass = mTestGraphics->CreateTrailObject(particleMaterials[0], trailInfo, DirectX::SimpleMath::Matrix::Identity);
		mTrailObjects.push_back(interfaceClass);
	}

	{
		TrailInfo trailInfo;
		trailInfo.TextureMode = TrailInfo::ETextureMode::Tile;
		auto* interfaceClass = mTestGraphics->CreateTrailObject(particleMaterials[1], trailInfo, DirectX::SimpleMath::Matrix::Identity);
		mTrailObjects.push_back(interfaceClass);
	}

	{
		TrailInfo trailInfo;
		trailInfo.TextureMode = TrailInfo::ETextureMode::DistributePerSegment;
		auto* interfaceClass = mTestGraphics->CreateTrailObject(particleMaterials[2], trailInfo, DirectX::SimpleMath::Matrix::Identity);
		mTrailObjects.push_back(interfaceClass);
	}

	{
		TrailInfo trailInfo;
		trailInfo.TextureMode = TrailInfo::ETextureMode::RepeatPerSegment;
		auto* interfaceClass = mTestGraphics->CreateTrailObject(particleMaterials[3], trailInfo, DirectX::SimpleMath::Matrix::Identity);
		mTrailObjects.push_back(interfaceClass);
	}

	{
		TrailInfo trailInfo;
		trailInfo.Time = 1.f;
		trailInfo.WidthRatios.push_back({ 100, 1.f });
		trailInfo.ColorRatios.push_back({ 1, 0, 0, 1.f });
		trailInfo.AlphaRatios.push_back({ 0.5, 1.f });
		trailInfo.VertexDivisionCount = 5;
		auto* interfaceClass = mTestGraphics->CreateTrailObject(particleMaterials[4], trailInfo, DirectX::SimpleMath::Matrix::Identity);
		mTrailObjects.push_back(interfaceClass);
	}

	{
		TrailInfo trailInfo;
		auto* interfaceClass = mTestGraphics->CreateTrailObject(particleMaterials[5], trailInfo, DirectX::SimpleMath::Matrix::Identity);
		mTrailObjects.push_back(interfaceClass);
	}
}

void EffectDemo::createModel(std::string modelPath, std::filesystem::path textureBasePath, DirectX::SimpleMath::Matrix transform, bool bIsCreateHierarchy)
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
			IStaticMeshObject* iStaticMeshObject = mTestGraphics->CreateStaticMeshObject(meshInterface, materialInterfaces, meshObjectInfo, transform);

			if (bIsCreateHierarchy)
			{
				unsigned int index = boneHierarchy->GetBoneIndex(node.Name);
				iStaticMeshObject->SetNodeHierarchyInstance(boneHierarchyCache);
				iStaticMeshObject->SetReferenceBoneIndex(index);
			}

			mStaticMeshObjects.push_back(iStaticMeshObject);
		}
	}
}
