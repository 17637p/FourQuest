#include "AlphaVertexAnimationDemo.h"

#include <IFQGraphics.h>
#include <fstream>

#include "InputManager.h"
#include "DemoUtils.h"
#include "../FQLoader/ModelLoader.h"
#include "../FQLoader/ModelConverter.h"

using namespace fq::utils;

AlphaVertexAnimationDemo::AlphaVertexAnimationDemo()
	:mWindowPosX(0),
	mWindowPosY(0),
	mScreenWidth(1920),
	mScreenHeight(1080),
	mResizing(false),
	mTestGraphics(nullptr)
{
	HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

	CreateHWND(L"FQ LightMap Demo", WS_OVERLAPPEDWINDOW, mWindowPosX, mWindowPosY, mScreenWidth, mScreenHeight);
	ShowWindow(mHwnd, SW_SHOWNORMAL);

	mEngineExporter = std::make_shared<fq::graphics::EngineExporter>();
}

AlphaVertexAnimationDemo::~AlphaVertexAnimationDemo()
{
	mEngineExporter->DeleteEngine(mTestGraphics);

	CoUninitialize();
}

bool AlphaVertexAnimationDemo::Init(HINSTANCE hInstance)
{
	/// 기본적인 초기화
	InputManager::GetInstance().Init(mHwnd);
	mTimeManager.Init();

	mTestGraphics = mEngineExporter->GetEngine();
	mTestGraphics->Initialize(mHwnd, mScreenWidth, mScreenHeight, fq::graphics::EPipelineType::Deferred);

	// 랜더링 오브젝트 생성
	mTestGraphics->WriteModel("./resource/Graphics/AlphaVertexAnimationDemo/LightMapTest.model", mTestGraphics->ConvertModel("./resource/Graphics/AlphaVertexAnimationDemo/LightMapTest.fbx"));
	createModel("./resource/Graphics/AlphaVertexAnimationDemo/LightMapTest.model", "./resource/Graphics/AlphaVertexAnimationDemo/", DirectX::SimpleMath::Matrix::Identity);
	mTestGraphics->SetLightMapTexture({ "./resource/Graphics/AlphaVertexAnimationDemo/Lightmap-0_comp_light.png" });
	mTestGraphics->SetLightMapDirectionTexture({ "./resource/Graphics/AlphaVertexAnimationDemo/Lightmap-0_comp_dir.png" });
	//mTestGraphics->SetSkyBox(L"./resource/Graphics/AlphaVertexAnimationDemo/123EnvHDR.dds", true);

	struct LightMapInfo
	{
		DirectX::SimpleMath::Vector4 OffsetScale;
		unsigned int Index;
	};

	std::map<std::string, LightMapInfo> lightMapUVMap;
	// load
	std::ifstream readData("./resource/Graphics/AlphaVertexAnimationDemo/LightmapUVs.json");
	nlohmann::ordered_json nodeUVsJson;

	if (readData.is_open())
	{
		readData >> nodeUVsJson;
		readData.close();
	}
	else
		assert(!"파일 열기 실패");

	for (const auto& nodeUVJson : nodeUVsJson["lightmapUVs"])
	{
		auto name = nodeUVJson["objectName"].get<std::string>();

		std::vector<DirectX::SimpleMath::Vector2> uvs;

		LightMapInfo lightmapInfo;

		lightmapInfo.OffsetScale.x = nodeUVJson["lightmapScaleOffset"][0].get<float>();
		lightmapInfo.OffsetScale.y = nodeUVJson["lightmapScaleOffset"][1].get<float>();
		lightmapInfo.OffsetScale.z = nodeUVJson["lightmapScaleOffset"][2].get<float>();
		lightmapInfo.OffsetScale.w = nodeUVJson["lightmapScaleOffset"][3].get<float>();

		lightmapInfo.Index = nodeUVJson["lightmapIndex"].get<unsigned int>();

		lightMapUVMap.insert({ name, std::move(lightmapInfo) });
	}

	for (fq::graphics::IStaticMeshObject* staticMeshObject : mStaticMeshObjects)
	{
		auto staticMeshObjectInf = staticMeshObject->GetMeshObjectInfo();
		staticMeshObjectInf.ObjectType = fq::graphics::MeshObjectInfo::EObjectType::Static;
		staticMeshObject->SetMeshObjectInfo(staticMeshObjectInf);

		for (auto material : staticMeshObject->GetMaterials())
		{
			auto materialInfo = material->GetInfo();
			materialInfo.Roughness = 0.f;
			materialInfo.RasterizeType = fq::graphics::ERasterizeMode::TwoSide;
			material->SetInfo(materialInfo);
		}

		auto meshData = staticMeshObject->GetStaticMesh()->GetMeshData();
		auto find = lightMapUVMap.find(meshData.NodeName);

		if (find != lightMapUVMap.end())
		{
			staticMeshObject->SetLightmapUVScaleOffset(find->second.OffsetScale);
			staticMeshObject->SetLightmapIndex(find->second.Index);
		}
	}

	/// camera 초기화
	AddDefaultCamera(mTestGraphics);

	// Camera Transform 설정
	mCameraTransform.worldPosition = { 0, 0, -100 };
	mCameraTransform.worldRotation = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(DirectX::XMConvertToRadians(0.0f), DirectX::XMConvertToRadians(0.0f), DirectX::XMConvertToRadians(0.0f));
	mCameraTransform.worldScale = { 1, 1, 1 };

	mCameraTransform.worldMatrix =
		DirectX::SimpleMath::Matrix::CreateScale(mCameraTransform.worldScale) *
		DirectX::SimpleMath::Matrix::CreateFromQuaternion(mCameraTransform.worldRotation) *
		DirectX::SimpleMath::Matrix::CreateTranslation(mCameraTransform.worldPosition);
	return true;
}

void AlphaVertexAnimationDemo::Loop()
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

void AlphaVertexAnimationDemo::Finalize()
{

}

LRESULT AlphaVertexAnimationDemo::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
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

void AlphaVertexAnimationDemo::Update()
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

void AlphaVertexAnimationDemo::Render()
{
	mTestGraphics->BeginRender();
	debugRender();
	mTestGraphics->Render();
	mTestGraphics->EndRender();
}

void AlphaVertexAnimationDemo::debugRender()
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

void AlphaVertexAnimationDemo::calculateFrameStats()
{
	float fps = (float)mTimeManager.GetFps();
	float mspf = 1000.0f / fps;

	std::wostringstream outs;
	outs.precision(6);
	outs << L"FPS: " << fps << L"    "
		<< L"Frame Time: " << mspf << L" (ms)";
	SetWindowText(mHwnd, outs.str().c_str());
}

void AlphaVertexAnimationDemo::createModel(std::string modelPath, std::filesystem::path textureBasePath, DirectX::SimpleMath::Matrix transform, bool bIsCreateHierarchy)
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

		auto tempMatrix = node.ToParentMatrix;

		DirectX::SimpleMath::Vector3 pos;
		DirectX::SimpleMath::Quaternion rotation;
		DirectX::SimpleMath::Vector3 scale;
		tempMatrix.Decompose(scale, rotation, pos);
		auto rotationAxis = rotation.ToEuler();

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
			IStaticMeshObject* iStaticMeshObject = mTestGraphics->CreateStaticMeshObject(meshInterface, materialInterfaces, meshObjectInfo, node.ToParentMatrix);

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
