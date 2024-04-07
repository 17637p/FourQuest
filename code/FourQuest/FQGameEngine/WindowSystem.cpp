#include "FQGameEnginePCH.h"
#include "WindowSystem.h"
#include "IEngine.h"

fq::game_engine::WindowSystem::WindowSystem()
	:mHWND{}
	,mHInstance{}
{}

void fq::game_engine::WindowSystem::Initialize()
{
	mHInstance = GetModuleHandle(NULL);

	WNDCLASS wndClass{};

	wndClass.style = CS_HREDRAW | CS_VREDRAW;

	wndClass.lpfnWndProc = WndProc::ToolWndProc;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hInstance = mHInstance;
	wndClass.hIcon = LoadIcon(mHInstance, IDI_APPLICATION);
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
		, NULL, NULL, mHInstance, NULL);

	::AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false);

	ShowWindow(mHWND, SW_SHOWNORMAL);
	UpdateWindow(mHWND);
}

fq::game_engine::WindowSystem::~WindowSystem()
{

}

void fq::game_engine::WindowSystem::Finalize()
{
	::DestroyWindow(mHWND);
	::UnregisterClassW(L"4Quset", mHInstance);
}

