#include "Process.h"

//#include "GUIManager.h"
#include "InputManager.h"

//temp
#include "../FQGraphics/IFQGraphics.h"

#include "../FQLoader/ModelConverter.h"
#include "../FQLoader/ModelLoader.h"

#include <FQCommonGraphics.h>

Process::Process()
	:
	mWindowPosX(0),
	mWindowPosY(0),
	mScreenWidth(1920),
	mScreenHeight(1080),
	mResizing(false),
	mTestGraphics(nullptr)
{
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

	//mTestGraphics->DeleteLight(1);
	//mTestGraphics->DeleteLight(2);
	//mTestGraphics->DeleteLight(3);
	//mTestGraphics->DeleteLight(4);

	mEngineExporter->DeleteEngine(mTestGraphics);
}

bool Process::Init(HINSTANCE hInstance)
{
	InputManager::GetInstance().Init(mHwnd);
	//m_timer = std::make_unique<GameTimer>();
	mTimeManager.Init();

	mTestGraphics = mEngineExporter->GetEngine();

	mTestGraphics->Initialize(mHwnd, mScreenWidth, mScreenHeight, fq::graphics::EPipelineType::Forward);

	// 카메라 초기화
	fq::graphics::CameraInfo cameraInfo;

	cameraInfo.isPerspective = true;
	cameraInfo.filedOfView = 0.25f * 3.1415f;
	cameraInfo.nearPlain = 0.03f;
	cameraInfo.farPlain = 10000.0f;

	mTestGraphics->SetCamera(cameraInfo);
	//-------------------------------------
	const std::string modelPath = "./resource/example/model/gun.model";
	const std::string animModelPath0 = "./resource/example/model/SkinningTest.model";
	const std::string animModelPath1 = "./resource/example/model/kick.model";
	const std::string textureBasePath = "./resource/example/texture";

	mTestGraphics->CreateModel(modelPath, textureBasePath);

	std::vector<fq::graphics::AnimationInfo> animInfo;
	auto modelData = mTestGraphics->CreateModel(animModelPath0, textureBasePath);
	animInfo.push_back({ animModelPath0, modelData.Animations.front().Name, "Idle" });
	modelData = mTestGraphics->CreateModel(animModelPath1, textureBasePath);
	animInfo.push_back({ animModelPath1, modelData.Animations.front().Name, "Kick" });

	for (size_t i = 0; i < 10; ++i)
	{
		float randX = (float)(rand() % 500 - 250);
		float randY = (float)(rand() % 500 - 250);
		float randZ = (float)(rand() % 500 - 250);
		createModel(modelPath, DirectX::SimpleMath::Matrix::CreateTranslation({ randX, randY, randZ }));
		createModel(animModelPath0, animInfo, DirectX::SimpleMath::Matrix::CreateTranslation({ randX, randY, randZ }));
	}

	/// camera 초기화
	cameraTransform.worldPosition = { 0, 0, 0 };
	cameraTransform.worldRotation = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(DirectX::XMConvertToRadians(0.0f), DirectX::XMConvertToRadians(0.0f), DirectX::XMConvertToRadians(0.0f));
	cameraTransform.worldScale = { 1, 1, 1 };

	cameraTransform.worldMatrix =
		DirectX::SimpleMath::Matrix::CreateScale(cameraTransform.worldScale) *
		DirectX::SimpleMath::Matrix::CreateFromQuaternion(cameraTransform.worldRotation) *
		DirectX::SimpleMath::Matrix::CreateTranslation(cameraTransform.worldPosition);

	/// Light 초기화 
	fq::graphics::LightInfo directionalLightInfo;
	directionalLightInfo.type = fq::graphics::ELightType::Directional;
	directionalLightInfo.color = { 0,1,0, 1 };
	directionalLightInfo.intensity = 1;
	directionalLightInfo.direction = { 1,0,0 };

	//mTestGraphics->AddLight(1, directionalLightInfo);

	directionalLightInfo.type = fq::graphics::ELightType::Directional;
	directionalLightInfo.color = { 1,0,0, 1 };
	directionalLightInfo.intensity = 1;
	directionalLightInfo.direction = { -1,0,0 };

	//mTestGraphics->AddLight(2, directionalLightInfo);

	directionalLightInfo.type = fq::graphics::ELightType::Directional;
	directionalLightInfo.color = { 0,0,1, 1 };
	directionalLightInfo.intensity = 1;
	directionalLightInfo.direction = { 0,-1,0 };

	//mTestGraphics->AddLight(3, directionalLightInfo);

	directionalLightInfo.type = fq::graphics::ELightType::Point;
	directionalLightInfo.color = { 1,1,0, 1 };
	directionalLightInfo.intensity = 1;
	directionalLightInfo.range = 100;
	directionalLightInfo.position = { -10, 0 ,0 };

	mTestGraphics->AddLight(4, directionalLightInfo);

	fq::graphics::LightInfo pointLightInfo;
	pointLightInfo.type = fq::graphics::ELightType::Point;
	pointLightInfo.color = { 1, 0, 0, 1 };
	pointLightInfo.intensity = 1;
	pointLightInfo.range = 100;
	pointLightInfo.attenuation = { 10, 0, 0 };
	pointLightInfo.position = { 10.f, 0.f, 0.f };

	mTestGraphics->AddLight(5, pointLightInfo);

	return true;
}

void Process::Loop()
{
	MSG msg;

	while (true)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT) break;

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
	//_guiManager->Finalize();
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
	calculateFrameStats();

	// ESC 버튼 누르면 프로그램 종료
	if (GetAsyncKeyState(VK_ESCAPE))
	{
		PostQuitMessage(0);
	}

	if (GetAsyncKeyState(VK_F1) & 0x8000)
	{
		mTestGraphics->SetWindowSize(mScreenWidth, mScreenHeight);
	}

	// 카메라 조작
	const float speed = mTimeManager.GetDeltaTime() * 1000.f;
	if (InputManager::GetInstance().IsGetKey('W'))
	{
		walk(speed);
	}
	if (InputManager::GetInstance().IsGetKey('S'))
	{
		walk(-speed);
	}
	if (InputManager::GetInstance().IsGetKey('D'))
	{
		strafe(speed);
	}
	if (InputManager::GetInstance().IsGetKey('A'))
	{
		strafe(-speed);
	}
	if (InputManager::GetInstance().IsGetKey('E'))
	{
		worldUpdown(speed);
	}
	if (InputManager::GetInstance().IsGetKey('Q'))
	{
		worldUpdown(-speed);
	}
	if (InputManager::GetInstance().IsGetKey(VK_RBUTTON))
	{
		float dx = (0.25f * static_cast<float>(InputManager::GetInstance().GetDeltaPosition().x) * (3.141592f / 180.0f));
		float dy = (0.25f * static_cast<float>(InputManager::GetInstance().GetDeltaPosition().y) * (3.141592f / 180.0f));

		pitch(dy);
		yaw(dx);
	}
	mTestGraphics->UpdateCamera(cameraTransform);

	InputManager::GetInstance().Update();
}

void Process::Render()
{
	mTestGraphics->BeginRender();
	debugRender();
	mTestGraphics->Render();
	/// 그리기를 준비한다.
	//m_pRenderer->BeginRender();
	//
	///// 엔진만의 그리기를 한다.
	//m_pRenderer->Render(IImpGraphicsEngine::RendererType::Forward);
	//
	//if (_has2ndCamera)
	//{
	//	m_pRenderer->UpdateRight(m_timer->DeltaTime());
	//
	//	m_pRenderer->BeginRenderRight();
	//	m_pRenderer->RenderRight(IImpGraphicsEngine::RendererType::Forward);
	//}
	//
	//m_pRenderer->RenderUI();
	//_guiManager->Render();
	///// 그리기를 끝낸다.
	//m_pRenderer->EndRender();

	for (auto& obj : mStaticMeshObjects)
	{
		obj->UpdateTransform(obj->GetTransform() * DirectX::SimpleMath::Matrix::CreateRotationY(mTimeManager.GetDeltaTime()));
	}

	static float s_time = 0.f;
	s_time += mTimeManager.GetDeltaTime();
	s_time = fmod(s_time, 3.f);

	for (auto& obj : mSkinnedMeshObjects)
	{
		if (GetAsyncKeyState('1') & 0x8000)
		{
			obj->SetAnimationKey("Kick");
		}
		else if (GetAsyncKeyState('2') & 0x8000)
		{
			obj->SetAnimationKey("Idle");
		}

		obj->UpdateAnimationTime(s_time);
		auto data1 = obj->GetAnimationKeys();
	}

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

	frustumInfo.Frustum.Origin = { 0, 0, 200 };
	frustumInfo.Frustum.Near = 0.1f;
	frustumInfo.Frustum.Far = 500.f;
	frustumInfo.Frustum.LeftSlope = -0.5f;
	frustumInfo.Frustum.RightSlope = 0.5f;
	frustumInfo.Frustum.TopSlope = 0.5f;
	frustumInfo.Frustum.BottomSlope = -0.5f;
	frustumInfo.Color = { 0, 0, 1, 1 };
	mTestGraphics->DrawFrustum(frustumInfo);

	GridInfo gridInfo;
	gridInfo.Origin = { 0,0,0 };
	gridInfo.XAxis = { 1, 0, 0 };
	gridInfo.YAxis = { 0, 0, 1 };
	gridInfo.XDivision = 20;
	gridInfo.YDivision = 20;
	gridInfo.GridSize = 1000.f;
	gridInfo.Color = { 0, 1, 1, 1 };
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

/*=============================================================================
		camera
=============================================================================*/
#pragma region camera
void Process::strafe(float distance)
{
	//mPosition = XMFLOAT3(mRight.x * d + mPosition.x, mRight.y * d + mPosition.y, mRight.z * d + mPosition.z);
	DirectX::SimpleMath::Matrix tempMatrix;
	tempMatrix = DirectX::SimpleMath::Matrix::CreateFromQuaternion(cameraTransform.worldRotation);

	cameraTransform.worldPosition = DirectX::SimpleMath::Vector3(
		tempMatrix._11 * distance + cameraTransform.worldPosition.x,
		tempMatrix._12 * distance + cameraTransform.worldPosition.y,
		tempMatrix._13 * distance + cameraTransform.worldPosition.z);

	cameraTransform.worldMatrix =
		DirectX::SimpleMath::Matrix::CreateScale(cameraTransform.worldScale) *
		DirectX::SimpleMath::Matrix::CreateFromQuaternion(cameraTransform.worldRotation) *
		DirectX::SimpleMath::Matrix::CreateTranslation(cameraTransform.worldPosition);
}

void Process::walk(float distance)
{
	DirectX::SimpleMath::Matrix tempMatrix;
	tempMatrix = DirectX::SimpleMath::Matrix::CreateFromQuaternion(cameraTransform.worldRotation);

	cameraTransform.worldPosition = DirectX::SimpleMath::Vector3(
		tempMatrix._31 * distance + cameraTransform.worldPosition.x,
		tempMatrix._32 * distance + cameraTransform.worldPosition.y,
		tempMatrix._33 * distance + cameraTransform.worldPosition.z);

	cameraTransform.worldMatrix =
		DirectX::SimpleMath::Matrix::CreateScale(cameraTransform.worldScale) *
		DirectX::SimpleMath::Matrix::CreateFromQuaternion(cameraTransform.worldRotation) *
		DirectX::SimpleMath::Matrix::CreateTranslation(cameraTransform.worldPosition);
}

void Process::worldUpdown(float distance)
{
	DirectX::SimpleMath::Matrix tempMatrix;
	tempMatrix = DirectX::SimpleMath::Matrix::CreateFromQuaternion(cameraTransform.worldRotation);

	cameraTransform.worldPosition = DirectX::SimpleMath::Vector3(
		tempMatrix._21 * distance + cameraTransform.worldPosition.x,
		tempMatrix._22 * distance + cameraTransform.worldPosition.y,
		tempMatrix._23 * distance + cameraTransform.worldPosition.z);

	cameraTransform.worldMatrix =
		DirectX::SimpleMath::Matrix::CreateScale(cameraTransform.worldScale) *
		DirectX::SimpleMath::Matrix::CreateFromQuaternion(cameraTransform.worldRotation) *
		DirectX::SimpleMath::Matrix::CreateTranslation(cameraTransform.worldPosition);
}

void Process::yaw(float angle)
{
	DirectX::SimpleMath::Vector3 up{ 0, 1, 0 };
	//up.Normalize();
	up = up * angle;
	DirectX::SimpleMath::Quaternion quaternion = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(up.y, up.x, up.z);

	cameraTransform.worldRotation = quaternion * cameraTransform.worldRotation;

	cameraTransform.worldMatrix =
		DirectX::SimpleMath::Matrix::CreateScale(cameraTransform.worldScale) *
		DirectX::SimpleMath::Matrix::CreateFromQuaternion(cameraTransform.worldRotation) *
		DirectX::SimpleMath::Matrix::CreateTranslation(cameraTransform.worldPosition);
}

void Process::pitch(float angle)
{
	DirectX::SimpleMath::Vector3 right{ 1, 0, 0 };
	right = right * angle;
	DirectX::SimpleMath::Quaternion quaternion = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(right.y, right.x, right.z);

	cameraTransform.worldRotation = quaternion * cameraTransform.worldRotation;

	cameraTransform.worldMatrix =
		DirectX::SimpleMath::Matrix::CreateScale(cameraTransform.worldScale) *
		DirectX::SimpleMath::Matrix::CreateFromQuaternion(cameraTransform.worldRotation) *
		DirectX::SimpleMath::Matrix::CreateTranslation(cameraTransform.worldPosition);
}
#pragma endregion camera

void Process::createModel(std::string modelPath, DirectX::SimpleMath::Matrix transform)
{
	createModel(modelPath, {}, transform);
}

void Process::createModel(std::string modelPath, std::vector<fq::graphics::AnimationInfo> animInfos, DirectX::SimpleMath::Matrix transform)
{
	const fq::common::Model& modelData = mTestGraphics->GetModel(modelPath);

	for (auto mesh : modelData.Meshes)
	{
		if (mesh.second.Vertices.empty())
		{
			continue;
		}

		fq::graphics::MeshObjectInfo meshInfo;
		meshInfo.ModelPath = modelPath;
		meshInfo.MeshName = mesh.second.Name;
		meshInfo.Transform = mesh.first.ToParentMatrix * transform;

		for (auto subset : mesh.second.Subsets)
		{
			meshInfo.MaterialNames.push_back(subset.MaterialName);
		}

		if (mesh.second.BoneVertices.empty())
		{
			fq::graphics::IStaticMeshObject* iStaticMeshObject = mTestGraphics->CreateStaticMeshObject(meshInfo);
			mStaticMeshObjects.push_back(iStaticMeshObject);
		}
		else
		{
			fq::graphics::ISkinnedMeshObject* iSkinnedMeshObject = mTestGraphics->CreateSkinnedMeshObject(meshInfo);

			for (const auto& animInfo : animInfos)
			{
				mTestGraphics->AddAnimation(iSkinnedMeshObject, animInfo);
			}
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
