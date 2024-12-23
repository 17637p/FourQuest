#include "UIDemo.h"

#include <IFQGraphics.h>

#include "InputManager.h"
#include "DemoUtils.h"

using namespace fq::utils;

UIDemo::UIDemo()
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

UIDemo::~UIDemo()
{
	mTestGraphics->DeleteVideoObject(mVideoObject);

	mTestGraphics->Finalize();
	for (fq::graphics::IImageObject* iobj : mImageObjects)
	{
		mTestGraphics->DeleteImageObject(iobj);
	}

	mEngineExporter->DeleteEngine(mTestGraphics);

	CoUninitialize();
}

bool UIDemo::Init(HINSTANCE hInstance)
{
	/// 기본적인 초기화
	InputManager::GetInstance().Init(mHwnd);
	mTimeManager.Init();

	mTestGraphics = mEngineExporter->GetEngine();
	mTestGraphics->Initialize(mHwnd, mScreenWidth, mScreenHeight, fq::graphics::EPipelineType::Deferred);

	/// Terrain 생성
	const std::string planeModelPath = "./resource/Graphics/TerrainDemo/Plane.model";
	fq::common::Model modelData = mTestGraphics->ConvertModel("./resource/Graphics/TerrainDemo/Plane.fbx");
	mTestGraphics->WriteModel(planeModelPath, modelData);
	const std::string textureBasePath = "./resource/Graphics/TerrainDemo";
	mTestGraphics->CreateModelResource(std::hash<std::string>{}(planeModelPath), planeModelPath, textureBasePath);

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

	createImage();

	// Font 추가
	mTestGraphics->AddFont(L"resource/internal/font/DungGeunMo.ttf");

	///////////////////////////////////////////////////////////////
	// --------------------font Test-------------------------------
	///////////////////////////////////////////////////////////////

	fq::graphics::TextInfo textInfo{};
	textInfo.CenterX = 0;
	textInfo.CenterY = 0;
	textInfo.Width = 1920;
	textInfo.Height = 1080;
	textInfo.Text = reinterpret_cast<const char*>(u8"집가고싶당");
	textInfo.FontColor = { 0.1,0.8,0.4,1 };
	textInfo.FontSize = 32;
	textInfo.FontPath = reinterpret_cast <const char*>(u8"DungGeunMo");
	textInfo.Align = fq::graphics::ETextAlign::RightTop;
	textInfo.BoxAlign = fq::graphics::ETextBoxAlign::LeftTop;
	mTextObject1 = mTestGraphics->CreateText(textInfo);
	//mTestGraphics->DrawText(L"집가고싶당", drawRect, 32, L"DungGeunMo", { 0.1,0.8,0.4,1 });

	//textInfo.CenterX = 600;
	//textInfo.CenterY = 700;
	textInfo.FontSize = 50;
	textInfo.FontColor = { 0.8,0.8,0.4,1 };
	mTextObject2 = mTestGraphics->CreateText(textInfo);
	//mTestGraphics->DrawText(L"집가고싶당", drawRect, 50, L"Verdana", { 0.8,0.8,0.4,1 });

	// -----------------------------------------------------------

	return true;
}

void UIDemo::Loop()
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

void UIDemo::Finalize()
{

}

LRESULT UIDemo::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
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

		if (mTestGraphics != nullptr)
		{
			mTestGraphics->SetWindowSize(mScreenWidth, mScreenHeight);
		}
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

void UIDemo::Update()
{
	mTimeManager.Update();
	InputManager::GetInstance().Update();

	fq::graphics::VideoInfo videoInfo = mVideoObject->GetVideoInfo();
	videoInfo.PlayTime += mTimeManager.GetDeltaTime();
	mVideoObject->SetVideoInfo(videoInfo);

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

	if (InputManager::GetInstance().IsGetKeyDown('M'))
	{
		//mImageObjects[0]->SetRotation(mImageObjects[0]->GetRotation() + 10);
		mImageObjects[0]->SetScaleY(mImageObjects[0]->GetScaleY() + 0.5);
	}

	if (InputManager::GetInstance().IsGetKeyDown('V'))
	{
		mTestGraphics->DeleteText(mTextObject1);
		mTestGraphics->DeleteText(mTextObject2);
	}

	if (InputManager::GetInstance().IsGetKeyDown('J'))
	{
		fq::graphics::VideoInfo videoInfo = mVideoObject->GetVideoInfo();
		videoInfo.isReset = true;
		mVideoObject->SetVideoInfo(videoInfo);
	}
}

void UIDemo::Render()
{
	mTestGraphics->BeginRender();
	debugRender();
	mTestGraphics->Render();
	mTestGraphics->EndRender();
}

void UIDemo::debugRender()
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

void UIDemo::createImage()
{
	fq::graphics::UIInfo uiInfo;
	uiInfo.StartX = 300;
	uiInfo.StartY = 300;
	uiInfo.Width = 100;
	uiInfo.Height = 100;
	uiInfo.XRatio = 1;
	uiInfo.YRatio = 1;
	uiInfo.Alpha = 0.5;
	uiInfo.Layer = 1;
	uiInfo.ImagePath = "./resource/Graphics/UIDemo/soul_bar_gauge.png";
	//uiInfo.MaskPath = "./resource/Graphics/UIDemo/bitmapmask.png";
	uiInfo.ScaleX = 1;
	uiInfo.ScaleY = 1;
	uiInfo.fillDegree = 180;
	uiInfo.RotationAngle = 0;
	uiInfo.isCenter = true;

	fq::graphics::VideoInfo videoInfo;

	videoInfo.Width = 1080;
	videoInfo.Height = 800;
	videoInfo.isRender = true;
	videoInfo.VideoPath = "C:\\Users\\user\\Videos\\Captures\\GamePlay.mp4";
	videoInfo.StartX = 100;
	videoInfo.StartY = 100;

	mVideoObject = mTestGraphics->CreateVideoObject(videoInfo);

	//auto tempImageObject = mTestGraphics->CreateImageObject(uiInfo);
	//mImageObjects.push_back(tempImageObject);

	//uiInfo.StartX = 500;
	//uiInfo.StartY = 500;
	//uiInfo.Width = 100;
	//uiInfo.Height = 50;
	//uiInfo.XRatio = 1;
	//uiInfo.YRatio = 0.5;
	//uiInfo.Alpha = 1;
	//uiInfo.Layer = 0;
	//uiInfo.ImagePath = "./resource/Graphics/UIDemo/1_Base_color.png";
	//uiInfo.ScaleX = 1;
	//uiInfo.ScaleY = 1;
	//uiInfo.RotationAngle = 0;
	//uiInfo.fillDegree = 359;
	//
	//tempImageObject = mTestGraphics->CreateImageObject(uiInfo);
	//mImageObjects.push_back(tempImageObject);
}