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

	mEngineExporter->DeleteEngine(mTestGraphics);
}

bool Process::Init(HINSTANCE hInstance)
{
	InputManager::GetInstance().Init(mHwnd);
	//m_timer = std::make_unique<GameTimer>();

	mTestGraphics = mEngineExporter->GetEngine();

	mTestGraphics->Initialize(mHwnd, mScreenWidth, mScreenHeight);

	// 카메라 초기화
	fq::graphics::CameraInfo cameraInfo;

	cameraInfo.isPerspective = true;
	cameraInfo.filedOfView = 0.25f * 3.1415f;
	cameraInfo.nearPlain = 0.03f;
	cameraInfo.farPlain = 1000.0f;

	mTestGraphics->SetCamera(cameraInfo);
	//-------------------------------------
	const std::string modelPath = "./resource/example/model/box.model";

	mTestGraphics->ConvertModel("./resource/example/fbx/box.fbx", modelPath);
	auto model = mTestGraphics->CreateModel(modelPath, "./resource/example/texture");
	auto model1 = mTestGraphics->GetModel(modelPath);
	for (auto mesh : model.Meshes)
	{
		if (mesh.second.Vertices.empty())
		{
			continue;
		}

		fq::graphics::MeshObjectInfo meshInfo;
		meshInfo.ModelPath = modelPath;
		meshInfo.MeshName = mesh.second.Name;
		meshInfo.Transform = mesh.first.ToParentMatrix;

		for (auto subset : mesh.second.Subsets)
		{
			meshInfo.MaterialNames.push_back(subset.MaterialName);
		}

		if (mesh.second.BoneVertices.empty())
		{
			mStaticMeshObjects.push_back(mTestGraphics->CreateStaticMeshObject(meshInfo));
			mStaticMeshObjects.back()->UpdateTransform(mStaticMeshObjects.back()->GetTransform() * DirectX::SimpleMath::Matrix::CreateRotationY(3.14 * 1.5f));
		}
		else
		{
			mTestGraphics->CreateSkinnedMeshObject(meshInfo);
		}
	}
	mTestGraphics->DeleteModel("./resource/example/gun/gun");
	// auto model2 = mTestGraphics->GetModel("../Temp/gun/gun"); // assert (false)
	mTestGraphics->GetSRV();

	/// camera 초기화
	cameraTransform.worldPosition = { 0, 0, 0 };
	cameraTransform.worldRotation = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(DirectX::XMConvertToRadians(0.0f), DirectX::XMConvertToRadians(0.0f), DirectX::XMConvertToRadians(0.0f));
	cameraTransform.worldScale = { 1, 1, 1 };

	cameraTransform.worldMatrix =
		DirectX::SimpleMath::Matrix::CreateScale(cameraTransform.worldScale) *
		DirectX::SimpleMath::Matrix::CreateFromQuaternion(cameraTransform.worldRotation) *
		DirectX::SimpleMath::Matrix::CreateTranslation(cameraTransform.worldPosition);

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
	const float speed = 0.01f;
	if (InputManager::GetInstance().IsGetKey('W'))
	{
		walk(speed);
	}
	if (InputManager::GetInstance().IsGetKey('S'))
	{
		walk(-speed);
	}
	if (InputManager::GetInstance().IsGetKey('A'))
	{
		strafe(speed);
	}
	if (InputManager::GetInstance().IsGetKey('D'))
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
	mTestGraphics->UpdateCamera(cameraTransform);

	InputManager::GetInstance().Update();
}

void Process::Render()
{
	mTestGraphics->BeginRender();

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
		obj->UpdateTransform(obj->GetTransform() * DirectX::SimpleMath::Matrix::CreateRotationY(0.0001f));
	}

	mTestGraphics->EndRender();
}

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
