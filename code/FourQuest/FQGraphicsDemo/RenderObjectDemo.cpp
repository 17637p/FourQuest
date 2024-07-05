#include "RenderObjectDemo.h"

#include <IFQGraphics.h>
#include <set>
#include <map>

#include "InputManager.h"
#include "DemoUtils.h"

using namespace fq::utils;

RenderObjectDemo::RenderObjectDemo()
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

RenderObjectDemo::~RenderObjectDemo()
{
	mEngineExporter->DeleteEngine(mTestGraphics);

	CoUninitialize();
}

bool RenderObjectDemo::Init(HINSTANCE hInstance)
{
	/// 기본적인 초기화
	InputManager::GetInstance().Init(mHwnd);
	mTimeManager.Init();

	mTestGraphics = mEngineExporter->GetEngine();
	mTestGraphics->Initialize(mHwnd, mScreenWidth, mScreenHeight, fq::graphics::EPipelineType::Forward);

	/// RenderObject 생성
	mTestGraphics->WriteModel("./resource/Graphics/RenderObjectDemo/gun.model", mTestGraphics->ConvertModel("./resource/Graphics/RenderObjectDemo/gun.fbx"));
	mTestGraphics->WriteModel("./resource/Graphics/RenderObjectDemo/SkinningTest.model", mTestGraphics->ConvertModel("./resource/Graphics/RenderObjectDemo/SkinningTest.fbx"));

	const std::string textureBasePath = "./resource/Graphics/RenderObjectDemo/";
	std::vector<std::string> modelPaths;
	modelPaths.reserve(32);
	std::vector<std::string> fbxPaths;
	modelPaths.reserve(32);

	auto addPath = [&modelPaths, &fbxPaths](std::string path)
		{
			modelPaths.push_back(path + ".model");
			fbxPaths.push_back(path + ".fbx");
		};

	addPath("./resource/Graphics/RenderObjectDemo/gun");
	addPath("./resource/Graphics/RenderObjectDemo/SkinningTest");
	addPath("./resource/Graphics/RenderObjectDemo/Meleemob_002");
	addPath("./resource/Graphics/RenderObjectDemo/player01");
	addPath("./resource/Graphics/RenderObjectDemo/playerani7");
	addPath("./resource/Graphics/RenderObjectDemo/RangeMonster(Union_100)");

	for (size_t i = 0; i < modelPaths.size(); ++i)
	{
		mTestGraphics->WriteModel(modelPaths[i], mTestGraphics->ConvertModel(fbxPaths[i]));
	}

	for (size_t i = 0; i < modelPaths.size(); ++i)
	{
		for (int j = -5; j < 5; ++j)
		{
			createModel(modelPaths[i], textureBasePath, DirectX::SimpleMath::Matrix::CreateScale(0.01f) * DirectX::SimpleMath::Matrix::CreateTranslation(j * 1, i * 2, 0));
		}
	}

	for (size_t i = 0; i < mSkinnedMeshObjects.size(); ++i)
	{
		SkinnedMeshRender renderer;

		renderer.SkinnedMeshObject = mSkinnedMeshObjects[i];
		renderer.NodeHierarchyInstance = mSkinnedMeshObjects[i]->GetNodeHierarchyInstance();

		const auto& nodeHierarchy = renderer.NodeHierarchyInstance->GetNodeHierarchy();
		const std::set<std::shared_ptr<fq::graphics::IAnimation>>& animations = nodeHierarchy->GetRegisterAnimations();

		for (auto animation : animations)
		{
			renderer.Animations.insert({ "Anim" + i, animation });
		}

		mSkinnedMeshRenderers.push_back(renderer);
	}

	/// camera 초기화
	AddDefaultCamera(mTestGraphics);

	// Camera Transform 설정
	mCameraTransform.worldPosition = { 0, 0, 0 };
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
	directionalLightInfo.intensity = 2;
	directionalLightInfo.direction = { 0, -0.7, -0.7 };
	directionalLightInfo.direction.Normalize();

	mTestGraphics->AddLight(1, directionalLightInfo);

	directionalLightInfo.type = fq::graphics::ELightType::Directional;
	directionalLightInfo.color = { 1, 1, 1, 1 };
	directionalLightInfo.intensity = 2;
	directionalLightInfo.direction = { 0, -0.7, 0.7 };
	directionalLightInfo.direction.Normalize();

	mTestGraphics->AddLight(2, directionalLightInfo);
	return true;
}

void RenderObjectDemo::Loop()
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

void RenderObjectDemo::Finalize()
{

}

LRESULT RenderObjectDemo::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
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

void RenderObjectDemo::Update()
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

	using namespace fq::graphics;

	// animation

	static float s_animTime = 0.f;
	s_animTime += mTimeManager.GetDeltaTime();
	for (SkinnedMeshRender& skinnedMeshRenderer : mSkinnedMeshRenderers)
	{
		if (!skinnedMeshRenderer.Animations.empty())
		{
			auto anim = skinnedMeshRenderer.Animations.begin()->second;
			skinnedMeshRenderer.NodeHierarchyInstance->Update(fmod(s_animTime, anim->GetAnimationClip().Duration), anim);
		}
	}

	// 카메라 조작
	float speed = mTimeManager.GetDeltaTime();
	if (InputManager::GetInstance().IsGetKey(VK_SHIFT))
	{
		speed = mTimeManager.GetDeltaTime() * 10;
	}
	else
	{
		speed = mTimeManager.GetDeltaTime();
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

void RenderObjectDemo::Render()
{
	mTestGraphics->BeginRender();
	debugRender();
	mTestGraphics->Render();
	mTestGraphics->EndRender();
}

void RenderObjectDemo::debugRender()
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

void RenderObjectDemo::createModel(std::string modelPath, std::filesystem::path textureBasePath, DirectX::SimpleMath::Matrix transform)
{
	using namespace fq::graphics;

	const fq::common::Model& modelData = mTestGraphics->CreateModelResource(modelPath, textureBasePath);

	auto boneHierarchy = mTestGraphics->GetNodeHierarchyByModelPathOrNull(modelPath);

	for (auto animation : modelData.Animations)
	{
		auto animationInterface = mTestGraphics->GetAnimationByModelPathOrNull(modelPath, animation.Name);
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
			auto materialInterface = mTestGraphics->GetMaterialByModelPathOrNull(modelPath, subset.MaterialName);
			materialInterfaces.push_back(materialInterface);
		}

		if (mesh.BoneVertices.empty())
		{
			auto meshInterface = mTestGraphics->GetStaticMeshByModelPathOrNull(modelPath, mesh.Name);
			IStaticMeshObject* iStaticMeshObject = mTestGraphics->CreateStaticMeshObject(meshInterface, materialInterfaces, meshObjectInfo, node.ToParentMatrix * transform);
			mStaticMeshObjects.push_back(iStaticMeshObject);
		}
		else
		{
			auto meshInterface = mTestGraphics->GetSkinnedMeshByModelPathOrNull(modelPath, mesh.Name);
			ISkinnedMeshObject* iSkinnedMeshObject = mTestGraphics->CreateSkinnedMeshObject(meshInterface, materialInterfaces, meshObjectInfo, transform);
			iSkinnedMeshObject->SetNodeHierarchyInstance(boneHierarchyCache);
			mSkinnedMeshObjects.push_back(iSkinnedMeshObject);
		}
	}
}


void RenderObjectDemo::calculateFrameStats()
{
	float fps = (float)mTimeManager.GetFps();
	float mspf = 1000.0f / fps;

	std::wostringstream outs;
	outs.precision(6);
	outs << L"FPS: " << fps << L"    "
		<< L"Frame Time: " << mspf << L" (ms)";
	SetWindowText(mHwnd, outs.str().c_str());
}