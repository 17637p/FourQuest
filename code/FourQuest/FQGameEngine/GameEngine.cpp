#include "FQGameEnginePCH.h"
#include "GameEngine.h"

fq::game_engine::GameEngine::GameEngine()
	:mHWND()
{

}

fq::game_engine::GameEngine::~GameEngine()
{

}

void fq::game_engine::GameEngine::Initialize(HINSTANCE hInstance)
{

}

void fq::game_engine::GameEngine::Process()
{
	MSG msg;

	bool bIsDone = false;
	while (!bIsDone)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				bIsDone = true;
				break;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{

		}
	}
}

void fq::game_engine::GameEngine::Finalize()
{
}

void fq::game_engine::GameEngine::initializeWindow(HINSTANCE hInstance)
{
	WNDCLASS wndClass{};

	wndClass.style = CS_HREDRAW | CS_VREDRAW;

	wndClass.lpfnWndProc = WndProc::ToolWndProc;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hInstance = hInstance;
	wndClass.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = static_cast<HBRUSH>(GetStockObject(WHITE_BRUSH));
	wndClass.lpszMenuName = NULL;
	wndClass.lpszClassName = L"4Quset";

	RegisterClass(&wndClass);

	RECT rect = { 0,0, 1080, 560 };

	mHWND = CreateWindow(L"4Quset"
		, L"4Quest"
		, WS_OVERLAPPEDWINDOW
		, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top
		, NULL, NULL, hInstance, NULL);

	::AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false);

	ShowWindow(mHWND, SW_SHOWNORMAL);
	UpdateWindow(mHWND);
}
