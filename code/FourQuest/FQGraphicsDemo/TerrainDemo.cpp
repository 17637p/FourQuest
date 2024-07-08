#include "TerrainDemo.h"

#include <IFQGraphics.h>

#include "InputManager.h"
#include "DemoUtils.h"
#include <IFQRenderResource.h>

using namespace fq::utils;

TerrainDemo::TerrainDemo()
	:mWindowPosX(0),
	mWindowPosY(0),
	mScreenWidth(1920),
	mScreenHeight(1080),
	mResizing(false),
	mTestGraphics(nullptr)
{
	HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

	CreateHWND(L"FQ Terrain Demo", WS_OVERLAPPEDWINDOW, mWindowPosX, mWindowPosY, mScreenWidth, mScreenHeight);
	ShowWindow(mHwnd, SW_SHOWNORMAL);

	mEngineExporter = std::make_shared<fq::graphics::EngineExporter>();
}

TerrainDemo::~TerrainDemo()
{
	mEngineExporter->DeleteEngine(mTestGraphics);

	CoUninitialize();
}

bool TerrainDemo::Init(HINSTANCE hInstance)
{
	/// 기본적인 초기화
	InputManager::GetInstance().Init(mHwnd);
	mTimeManager.Init();

	mTestGraphics = mEngineExporter->GetEngine();
	mTestGraphics->Initialize(mHwnd, mScreenWidth, mScreenHeight, fq::graphics::EPipelineType::Forward);

	/// Terrain 생성
	const std::string planeModelPath = "./resource/Graphics/TerrainDemo/Plane.model";
	mTestGraphics->ConvertModel("./resource/Graphics/TerrainDemo/Plane.fbx", planeModelPath);
	const std::string textureBasePath = "./resource/Graphics/TerrainDemo";
	mTestGraphics->CreateModel(planeModelPath, textureBasePath);

	createTerrain(planeModelPath, DirectX::SimpleMath::Matrix::CreateTranslation({ 0, 0, 0 }));

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
	directionalLightInfo.direction = { 0.5, -0.7, -0.3 };
	directionalLightInfo.direction.Normalize();

	mTestGraphics->AddLight(1, directionalLightInfo);

	directionalLightInfo.direction = { -0.3, 0.2, 0.7 };
	directionalLightInfo.direction.Normalize();

	mTestGraphics->AddLight(2, directionalLightInfo);

	mTestGraphics->AddCubeProbe({ 0, 6, 0 });

	//Light Probe Test
	const std::string cubeYModel = "./resource/Graphics/TerrainDemo/testCubeY.model";
	const std::string cubeYModel1 = "./resource/Graphics/TerrainDemo/testCubeY1.model";
	const std::string cubeYModel2 = "./resource/Graphics/TerrainDemo/testCubeY2.model";
	const std::string cubeYModel3 = "./resource/Graphics/TerrainDemo/testCubeY3.model";

	const std::string sphereModel = "./resource/Graphics/TerrainDemo/testSphere.model";
	mTestGraphics->ConvertModel("./resource/Graphics/TerrainDemo/testCubeY.fbx", cubeYModel);
	mTestGraphics->ConvertModel("./resource/Graphics/TerrainDemo/testCubeY1.fbx", cubeYModel1);
	mTestGraphics->ConvertModel("./resource/Graphics/TerrainDemo/testCubeY2.fbx", cubeYModel2);
	mTestGraphics->ConvertModel("./resource/Graphics/TerrainDemo/testCubeY3.fbx", cubeYModel3);

	mTestGraphics->ConvertModel("./resource/Graphics/TerrainDemo/testSphere.fbx", sphereModel);

	const std::string textureBase3Path = "./resource/example/texture";
	mTestGraphics->CreateModel(cubeYModel, textureBase3Path);
	mTestGraphics->CreateModel(cubeYModel1, textureBase3Path);
	mTestGraphics->CreateModel(cubeYModel2, textureBase3Path);
	mTestGraphics->CreateModel(cubeYModel3, textureBase3Path);
	mTestGraphics->CreateModel(sphereModel, textureBase3Path);

	createModel(cubeYModel, DirectX::SimpleMath::Matrix::CreateScale({ 0.01f, 0.01f, 0.01f }) * DirectX::SimpleMath::Matrix::CreateTranslation({ 2.5, 0, 0 }), false);
	createModel(cubeYModel1, DirectX::SimpleMath::Matrix::CreateScale({ 0.01f, 0.01f, 0.01f }) * DirectX::SimpleMath::Matrix::CreateTranslation({ -2.5, 0, 0 }), false);
	createModel(cubeYModel2, DirectX::SimpleMath::Matrix::CreateScale({ 0.01f, 0.01f, 0.01f }) * DirectX::SimpleMath::Matrix::CreateTranslation({ 0, 0, 2.5 }), false);
	createModel(cubeYModel3, DirectX::SimpleMath::Matrix::CreateScale({ 0.01f, 0.01f, 0.01f }) * DirectX::SimpleMath::Matrix::CreateTranslation({ 0, 0, -2.5 }), false);

	//createModel(sphereModel, DirectX::SimpleMath::Matrix::CreateScale({ 0.002f, 0.002f, 0.002f }) * DirectX::SimpleMath::Matrix::CreateTranslation({ -0.7, 1.2, -0.7 }), true);
	//createModel(sphereModel, DirectX::SimpleMath::Matrix::CreateScale({ 0.002f, 0.002f, 0.002f }) * DirectX::SimpleMath::Matrix::CreateTranslation({ 0.7, 1.4, 0.7 }), true);
	//createModel(sphereModel, DirectX::SimpleMath::Matrix::CreateScale({ 0.002f, 0.002f, 0.002f }) * DirectX::SimpleMath::Matrix::CreateTranslation({ 0.7, 1.7, -0.7 }), true);
	//createModel(sphereModel, DirectX::SimpleMath::Matrix::CreateScale({ 0.002f, 0.002f, 0.002f }) * DirectX::SimpleMath::Matrix::CreateTranslation({ -0.7, 1.9, 0.7 }), true);

	createModel(sphereModel, DirectX::SimpleMath::Matrix::CreateScale({ 0.002f, 0.002f, 0.002f }) * DirectX::SimpleMath::Matrix::CreateTranslation({ -0.7, 1.2, 0 }), true);
	createModel(sphereModel, DirectX::SimpleMath::Matrix::CreateScale({ 0.002f, 0.002f, 0.002f }) * DirectX::SimpleMath::Matrix::CreateTranslation({ 0.7, 1.4, 0 }), true);
	createModel(sphereModel, DirectX::SimpleMath::Matrix::CreateScale({ 0.002f, 0.002f, 0.002f }) * DirectX::SimpleMath::Matrix::CreateTranslation({ 0, 1.7, -0.7 }), true);
	createModel(sphereModel, DirectX::SimpleMath::Matrix::CreateScale({ 0.002f, 0.002f, 0.002f }) * DirectX::SimpleMath::Matrix::CreateTranslation({ 0, 1.9, 0.7 }), true);

	lightProbePositions.push_back({ 1, 1, 0 });
	lightProbePositions.push_back({ -1, 1, 0 });
	lightProbePositions.push_back({ 0, 1, 1 });
	lightProbePositions.push_back({ 0, 1, -1 });
	lightProbePositions.push_back({ 1, 2, 0 });
	lightProbePositions.push_back({ -1,2, 0 });
	lightProbePositions.push_back({ 0, 2, 1 });
	lightProbePositions.push_back({ 0, 2, - 1 });

	lightProbePositions.push_back({ 1, 1, 1 });
	lightProbePositions.push_back({ -1, 1, 1 });
	lightProbePositions.push_back({ -1, 1, -1 });
	lightProbePositions.push_back({ 1, 1, -1 });
	lightProbePositions.push_back({ 1, 2, 1 });
	lightProbePositions.push_back({ -1,2, 1 });
	lightProbePositions.push_back({ -1, 2, -1 });
	lightProbePositions.push_back({ 1, 2, -1 });

	//lightProbePositions.push_back({ 5, -1, 0 });
	//lightProbePositions.push_back({ -5, -1, 0 });
	//lightProbePositions.push_back({ 0, -1, 5 });
	//lightProbePositions.push_back({ 0, -1, -5 });
	//lightProbePositions.push_back({ 5, 10, 0 });
	//lightProbePositions.push_back({ -5,10, 0 });
	//lightProbePositions.push_back({ 0, 10, 5 });
	//lightProbePositions.push_back({ 0, 10, -5 });

	for (int i = 0; i < lightProbePositions.size(); i++)
	{
		mTestGraphics->AddLightProbe(lightProbePositions[i]);
	}

	return true;
}

void TerrainDemo::Loop()
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

void TerrainDemo::Finalize()
{

}

LRESULT TerrainDemo::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
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

void TerrainDemo::Update()
{
	mTimeManager.Update();
	InputManager::GetInstance().Update();

	if (GetAsyncKeyState(VK_F1) & 0x8000)
	{
		mTestGraphics->SetWindowSize(mScreenWidth, mScreenHeight);
	}
	// ESC 버튼 누르면 프로그램 종료
	if (GetAsyncKeyState(VK_ESCAPE))
	{
		PostQuitMessage(0);
	}

	//picking 테스트
	if (InputManager::GetInstance().IsGetKey('Y'))
	{
		POINT mousePosition = InputManager::GetInstance().GetMousePosition();
		if (mousePosition.x < mScreenWidth && mousePosition.y < mScreenHeight && mousePosition.x > 0 && mousePosition.y > 0)
		{
			auto temp = mTestGraphics->GetPickingObject(mousePosition.x, mousePosition.y);
			if (temp != nullptr)
			{
				int t = 3;
			}
			int a = 3;
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

	if (InputManager::GetInstance().IsGetKeyDown('L'))
	{
		mTestGraphics->SaveCubeProbeTexture(1024, 1024);
	}
	if (InputManager::GetInstance().IsGetKeyDown('K'))
	{
		mTestGraphics->BakeLightProbe();
	}
	if (InputManager::GetInstance().IsGetKeyDown('O'))
	{
		mTestGraphics->DeleteLightProbe(3);
	}
	if (InputManager::GetInstance().IsGetKeyDown('N'))
	{
		mTestGraphics->SaveLightProbes("lightProbe");
	}
	if (InputManager::GetInstance().IsGetKeyDown('M'))
	{
		mTestGraphics->LoadLightProbes("lightProbe");
	}

	mTestGraphics->UpdateCamera(mCameraTransform);
}

void TerrainDemo::Render()
{
	mTestGraphics->BeginRender();
	debugRender();
	mTestGraphics->Render();
	mTestGraphics->EndRender();
}

void TerrainDemo::debugRender()
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

	SphereInfo sphereInfo;
	sphereInfo.Sphere.Center = { 0, 0, 0 };
	sphereInfo.Sphere.Radius = 0.1f;
	sphereInfo.Color = { 1,1,1,1 };

	for (int i = 0; i < lightProbePositions.size(); i++)
	{
		sphereInfo.Sphere.Center = lightProbePositions[i];
		mTestGraphics->DrawSphere(sphereInfo);
	}

}

void TerrainDemo::createTerrain(std::string modelPath, DirectX::SimpleMath::Matrix transform /*= DirectX::SimpleMath::Matrix::Identity*/)
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

		fq::graphics::ITerrainMeshObject* iTerrainMeshObject = mTestGraphics->CreateTerrainMeshObject(meshInfo);
		mTerrainMeshObjects.push_back(iTerrainMeshObject);

		//fq::graphics::TerrainMaterialInfo terrainMaterial;
		terrainMaterial.Layers.clear();

		fq::graphics::TerrainLayer layer1;
		fq::graphics::TerrainLayer layer2;
		fq::graphics::TerrainLayer layer3;

		layer1.BaseColor = "./resource/Graphics/TerrainDemo/t2.jpg";
		layer2.BaseColor = "./resource/Graphics/TerrainDemo/t1.jpg";
		layer3.BaseColor = "./resource/Graphics/TerrainDemo/t3.jpg";

		layer1.NormalMap = "./resource/Graphics/TerrainDemo/boxNormal.jpg";
		layer2.NormalMap = "./resource/Graphics/TerrainDemo/cerberus_N.png";
		layer3.NormalMap = "./resource/Graphics/TerrainDemo/character_normal.png";

		layer1.TileOffsetX = 0.5;
		layer1.TileOffsetY = 0.5;
		layer2.TileOffsetX = 0;
		layer2.TileOffsetY = 0;
		layer3.TileOffsetX = 0;
		layer3.TileOffsetY = 0;

		layer1.TileSizeX = 20;
		layer2.TileSizeX = 20;
		layer3.TileSizeX = 20;
		layer1.TileSizeY = 20;
		layer2.TileSizeY = 20;
		layer3.TileSizeY = 20;

		layer1.Metalic = 0;
		layer2.Metalic = 0;
		layer3.Metalic = 0;

		layer1.Roughness = 0;
		layer2.Roughness = 0;
		layer3.Roughness = 0;

		terrainMaterial.AlPhaFileName = "./resource/Graphics/TerrainDemo/TestAlpha4.png";

		// Height 설정
		terrainMaterial.HeightFileName = "./resource/Graphics/TerrainDemo/terrain.raw";
		terrainMaterial.HeightScale = 1000;
		terrainMaterial.TerrainWidth = 513;
		terrainMaterial.TerrainHeight = 513;

		terrainMaterial.TextureWidth = 513;
		terrainMaterial.TextureHeight = 513;

		terrainMaterial.Layers.push_back(layer1);
		terrainMaterial.Layers.push_back(layer2);
		terrainMaterial.Layers.push_back(layer3);

		mTestGraphics->SetTerrainMeshObject(iTerrainMeshObject, terrainMaterial);
	}
}

void TerrainDemo::createModel(std::string modelPath, DirectX::SimpleMath::Matrix transform, bool isUseLightProbe)
{
	const fq::common::Model& modelData = mTestGraphics->GetModel(modelPath);

	static int index = 0;

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

			std::vector<std::shared_ptr<fq::graphics::IMaterial>> mat = iStaticMeshObject->GetMaterialInterfaces();
			for (int i = 0; i < mat.size(); i++)
			{
				fq::common::Material matData = mat[i]->GetMaterialData();
				if (index == 0)
				{
					matData.BaseColorFileName = L"Red.png";
				}
				else if (index == 1)
				{
					matData.BaseColorFileName = L"Green.png";
				}
				else if (index == 2)
				{
					matData.BaseColorFileName = L"Blue.png";
				}
				else if (index == 3)
				{
					matData.BaseColorFileName = L"Black.png";
				}
				else
				{

				}

				mat[i]->SetMaterialData(matData);
			}

			mStaticMeshObjects.push_back(iStaticMeshObject);
			iStaticMeshObject->SetUseLightProbe(isUseLightProbe);
		}
	}

	index++;
}
