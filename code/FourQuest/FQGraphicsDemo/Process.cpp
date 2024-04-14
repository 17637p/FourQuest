#include "Process.h"

//#include "GUIManager.h"
#include "InputManager.h"

//temp
#include "../FQGraphics/Renderer.h"

Process::Process()
	:
	mWindowPosX(0), 
	mWindowPosY(0),
	mScreenWidth(1920), 
	mScreenHeight(1080),
	mResizing(false)
	//m_timer(nullptr),
	//m_pRenderer(nullptr)
{
	CreateHWND(L"FQGraphicsDemo", WS_OVERLAPPEDWINDOW, mWindowPosX, mWindowPosY, mScreenWidth, mScreenHeight);
	ShowWindow(mHwnd, SW_SHOWNORMAL);

	//m_pRenderer = EngineExporter::GetEngine();
	//_guiManager = new ImpGraphics::GUIManager;

}

Process::~Process()
{
	//delete _guiManager;
	//EngineExporter::DeleteEngine();
}

bool Process::Init(HINSTANCE hInstance)
{
	//m_pRenderer->Initialize((int)hInstance, (int)m_hWnd, m_screenWidth, m_screenHeight);

	//_guiManager->Init(reinterpret_cast<size_t>(m_hWnd), m_pRenderer->GetDevice(), m_pRenderer->GetDeviceContext());

	InputManager::GetInstance().Init(mHwnd);
	//m_timer = std::make_unique<GameTimer>();

	mTestGrphics = std::make_shared<fq_graphics::Renderer>();
	mTestGrphics->Initialize(mHwnd, mScreenWidth, mScreenHeight);

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

	InputManager::GetInstance().Update();
}

void Process::Render()
{
	mTestGrphics->BeginRender();
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
	mTestGrphics->EndRender();
}