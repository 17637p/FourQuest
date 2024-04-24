#include "Process.h"

//#include "GUIManager.h"
#include "InputManager.h"

//temp
#include "../FQGraphics/IFQGraphics.h"

#include "../FQLoader/ModelConverter.h"
#include "../FQLoader/ModelLoader.h"

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
	mEngineExporter->DeleteEngine(mTestGraphics);
}

bool Process::Init(HINSTANCE hInstance)
{
	InputManager::GetInstance().Init(mHwnd);
	//m_timer = std::make_unique<GameTimer>();

	mTestGraphics = mEngineExporter->GetEngine();
	mTestGraphics->Initialize(mHwnd, mScreenWidth, mScreenHeight);

	const std::string modelBasePath = "./resource/example/model/";
	const std::string FBXBasePath = "./resource/example/fbx/";
	const std::string model1 = "SkinningTest";
	const std::string model2 = "kick";

	mTestGraphics->ConvertModel(FBXBasePath + model1 + ".fbx", modelBasePath + model1 + ".model");
	mTestGraphics->ConvertModel(FBXBasePath + model2 + ".fbx", modelBasePath + model2 + ".model");

	auto modelData1 = mTestGraphics->CreateModel(modelBasePath + model1 + ".model", "./resource/example/texture");
	auto modelData2 = mTestGraphics->CreateModel(modelBasePath + model2 + ".model", "./resource/example/texture");

	for (auto mesh : modelData1.Meshes)
	{
		if (mesh.second.Vertices.empty())
		{
			continue;
		}

		fq::graphics::MeshObjectInfo meshInfo;
		meshInfo.ModelPath = modelBasePath + model1 + ".model";
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
			auto* iSkinned = mTestGraphics->CreateSkinnedMeshObject(meshInfo);
			mTestGraphics->AddAnimation(iSkinned, { modelBasePath + model1 + ".model",  modelData1.Animations.front().Name, "Idle" });
			mTestGraphics->AddAnimation(iSkinned, { modelBasePath + model2 + ".model",  modelData2.Animations.front().Name, "kick" });
			iSkinned->SetAnimationKey("Idle");
			mSkinnedMeshObjects.push_back(iSkinned);
		}
	}

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
	// ESC 버튼 누르면 프로그램 종료
	if (GetAsyncKeyState(VK_ESCAPE))
	{
		PostQuitMessage(0);
	}

	if (GetAsyncKeyState(VK_F1) & 0x8000)
	{
		mTestGraphics->SetWindowSize(mScreenWidth, mScreenHeight);
	}

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

	static float s_time = 0.f;
	s_time += 0.001f;
	s_time = fmod(s_time, 3.f);

	for (auto& obj : mSkinnedMeshObjects)
	{
		if (GetAsyncKeyState('1') & 0x8000)
		{
			obj->SetAnimationKey("kick");
		}
		else
		{
			obj->SetAnimationKey("Idle");
		}

		obj->UpdateAnimationTime(s_time);
		auto data1 = obj->GetAnimationKeys();
	}

	mTestGraphics->EndRender();
}
