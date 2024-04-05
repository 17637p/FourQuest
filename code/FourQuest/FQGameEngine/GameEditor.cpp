#include "FQGameEnginePCH.h"
#include "GameEditor.h"

fq::game_engine::GameEditor::GameEditor()
	:mHWND()
{

}

fq::game_engine::GameEditor::~GameEditor()
{

}

void fq::game_engine::GameEditor::Initialize(HINSTANCE hInstance)
{
	// window √ ±‚»≠
	initializeWindow(hInstance);
}

void fq::game_engine::GameEditor::Process()
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

void fq::game_engine::GameEditor::Finalize()
{

}

void fq::game_engine::GameEditor::initializeWindow(HINSTANCE hInstance)
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

	RECT rect = { 0,0, 1080, 560};

	mHWND = CreateWindow(L"4Quset"
		, L"4Quest"
		, WS_OVERLAPPEDWINDOW
		, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top
		, NULL, NULL, hInstance, NULL);

	::AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false);

	ShowWindow(mHWND, SW_SHOWNORMAL);
	UpdateWindow(mHWND);
}
