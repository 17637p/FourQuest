#include "TerrainDemo.h"

#include <IFQGraphics.h>

#include "InputManager.h"
#include "DemoUtils.h"

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
	directionalLightInfo.direction = { 0, -0.7, -0.7 };
	directionalLightInfo.direction.Normalize();

	mTestGraphics->AddLight(1, directionalLightInfo);
	mTestGraphics->AddCubeProbe({ 0, 6, 0 });

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

	if (InputManager::GetInstance().IsGetKey('U'))
	{
		auto test = mTerrainMeshObjects[0]->GetHeightData();
		int a = 3;
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
