#include "RenderObjectDemo.h"

#include <IFQGraphics.h>
#include <set>
#include <map>
#include "../FQLoader/ModelLoader.h"

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
	mTestGraphics->Initialize(mHwnd, mScreenWidth, mScreenHeight, fq::graphics::EPipelineType::Deferred);

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

	//addPath("./resource/Graphics/RenderObjectDemo/gun");
	//addPath("./resource/Graphics/RenderObjectDemo/SkinningTest");
	//addPath("./resource/Graphics/RenderObjectDemo/Meleemob_002");
	//addPath("./resource/Graphics/RenderObjectDemo/player01");
	addPath("./resource/Graphics/RenderObjectDemo/Player(1)");
	//addPath("./resource/Graphics/RenderObjectDemo/RangeMonster(Union_100)");
	//addPath("./resource/Graphics/RenderObjectDemo/RangeMonster(Union_100)");

	for (size_t i = 0; i < modelPaths.size(); ++i)
	{
		mTestGraphics->WriteModel(modelPaths[i], mTestGraphics->ConvertModel(fbxPaths[i]));
	}

	for (size_t i = 0; i < modelPaths.size(); ++i)
	{
		for (int j = 0; j < 10; ++j)
		{
			createModel(modelPaths[i], textureBasePath, DirectX::SimpleMath::Matrix::CreateScale(1) * DirectX::SimpleMath::Matrix::CreateTranslation(j * 10, i * 10, 0));
		}
	}

	// 스킨 매쉬 랜더러
	for (size_t i = 0; i < mSkinnedMeshObjects.size(); ++i)
	{
		SkinnedMeshRender renderer;

		renderer.SkinnedMeshObject = mSkinnedMeshObjects[i];
		renderer.NodeHierarchyInstance = mSkinnedMeshObjects[i]->GetNodeHierarchyInstance();

		const auto& nodeHierarchy = renderer.NodeHierarchyInstance->GetNodeHierarchy();
		const std::set<std::shared_ptr<fq::graphics::IAnimation>>& animations = nodeHierarchy->GetRegisterAnimations();

		size_t j = 0;
		for (auto animation : animations)
		{
			{
				renderer.Animations.insert({ "Anim" + std::to_string(j++), animation });
			}
		}

		mSkinnedMeshRenderers.push_back(renderer);
	}

	//auto animation = fq::loader::AnimationLoader::Read("./resource/Graphics/RenderObjectDemo/TransformAnim.txt");
	//auto animationInterface = mTestGraphics->CreateAnimation(animation);

	// 스태틱 매쉬 렌더러
	for (size_t i = 0; i < mStaticMeshObjects.size(); ++i)
	{
		StaticMeshRenderer renderer;

		renderer.StaticMeshObject = mStaticMeshObjects[i];
		renderer.NodeHierarchyInstance = mStaticMeshObjects[i]->GetNodeHierarchyInstance();

		if (renderer.NodeHierarchyInstance != nullptr)
		{
			const auto nodeHierarchy = renderer.NodeHierarchyInstance->GetNodeHierarchy();
			const std::set<std::shared_ptr<fq::graphics::IAnimation>>& animations = nodeHierarchy->GetRegisterAnimations();

			auto nodeName = mStaticMeshObjects[i]->GetStaticMesh()->GetMeshData().NodeName;
			mStaticMeshObjects[i]->SetReferenceBoneIndex(nodeHierarchy->GetBoneIndex(nodeName));

			//renderer.Animations.insert({ "Anim" + i, animationInterface });

			mStaticMeshRenderers.push_back(renderer);
		}
	}

	/// camera 초기화
	AddDefaultCamera(mTestGraphics);

	// Camera Transform 설정
	mCameraTransform.worldPosition = { 0, 0, -5 };
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
	for (auto* object : mSkinnedMeshObjects)
	{
		mTestGraphics->DeleteSkinnedMeshObject(object);
	}
	for (auto* object : mStaticMeshObjects)
	{
		mTestGraphics->DeleteStaticMeshObject(object);
	}
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
	static float s_change_time = 0.f;
	static int index = 0;
	static int nextIndex = 0;

	s_animTime += mTimeManager.GetDeltaTime();

	// 상체 애니메이션 증가
	if (GetAsyncKeyState('I') & 0x8000 && s_change_time > 1.f)
	{
		++index;
		s_change_time = 0.f;
	}

	// 하체 애니메이션 인덱스 증가
	if (GetAsyncKeyState('P') & 0x8000 && s_change_time > 1.f)
	{
		++nextIndex;
		s_change_time = 0.f;
	}
	else
	{
		s_change_time += mTimeManager.GetDeltaTime();
	}

	for (SkinnedMeshRender& skinnedMeshRenderer : mSkinnedMeshRenderers)
	{
		if (!skinnedMeshRenderer.Animations.empty())
		{
			auto anim0 = std::next(skinnedMeshRenderer.Animations.begin(), index)->second;
			auto anim1 = std::next(skinnedMeshRenderer.Animations.begin(), nextIndex)->second;

			unsigned int spineIndex = skinnedMeshRenderer.NodeHierarchyInstance->GetNodeHierarchy()->GetSpineIndex();
			unsigned int upperEndIndex = skinnedMeshRenderer.NodeHierarchyInstance->GetNodeHierarchy()->GetUpperBodyEndIndex();
			unsigned int lowerStartIndex = skinnedMeshRenderer.NodeHierarchyInstance->GetNodeHierarchy()->GetLowerBodyStartIndex();
			unsigned int endIndex = skinnedMeshRenderer.NodeHierarchyInstance->GetNodeHierarchy()->GetEndIndex();

			// 애니메이션 행렬을 로컬 애니메이션을 초기상태로 만들어둡니다.
			skinnedMeshRenderer.NodeHierarchyInstance->SetBindPoseLocalTransform();
			// 스파인부터 상체의 끝까지 로컬 애니메이션 행렬을 해당 애니메이션으로 갱신합니다.
			// 루트에 이동이 들어간 애니메이션의 경우 0번부터 애니메이션 갱신을 수행해야 함
			skinnedMeshRenderer.NodeHierarchyInstance->UpdateLocalTransformRange(fmod(s_animTime, anim0->GetAnimationClip().Duration), anim0, spineIndex, upperEndIndex);
			// 하체의 시작부터 끝까지 로컬 애니메이션 행렬을 해당 애니메이션으로 갱신합니다.
			skinnedMeshRenderer.NodeHierarchyInstance->UpdateLocalTransformRange(fmod(s_animTime, anim1->GetAnimationClip().Duration), anim1, lowerStartIndex, endIndex);
			// 갱신 해둔 로컬 애니메이션 행렬로 애니메이션에 사용할 toRoot 트랜스폼을 갱신합니다.
			skinnedMeshRenderer.NodeHierarchyInstance->UpdateByLocalTransform();
		}
	}

	for (StaticMeshRenderer& staticMeshRenderer : mStaticMeshRenderers)
	{
		if (!staticMeshRenderer.Animations.empty())
		{
			auto anim = staticMeshRenderer.Animations.begin()->second;
			staticMeshRenderer.NodeHierarchyInstance->Update(fmod(s_animTime, anim->GetAnimationClip().Duration), anim);
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

void RenderObjectDemo::createModel(std::string modelPath, std::filesystem::path textureBasePath, DirectX::SimpleMath::Matrix transform, bool bIsCreateHierarchy)
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
			iStaticMeshObject->SetNodeHierarchyInstance(boneHierarchyCache);
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