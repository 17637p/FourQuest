#include "WindowSystem.h"
#include "IEngine.h"

UINT fq::game_engine::WindowSystem::Height;
UINT fq::game_engine::WindowSystem::Width;

fq::game_engine::WindowSystem::WindowSystem()
	:mHWND{}
	,mHInstance{}
	,mScreenWidth{}
	,mScreenHeight{}
	,mScreenLeft{}
	,mScreenTop{}
{}

void fq::game_engine::WindowSystem::Initialize()
{
	mHInstance = ::GetModuleHandle(NULL);

	WNDCLASS wndClass{};

	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = WndProc::ToolWndProc;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hInstance = mHInstance;
	wndClass.hIcon = ::LoadIcon(mHInstance, IDI_APPLICATION);
	wndClass.hCursor = ::LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = static_cast<HBRUSH>(::GetStockObject(WHITE_BRUSH));
	wndClass.lpszMenuName = NULL;
	wndClass.lpszClassName = L"4Quset";

	RegisterClass(&wndClass);

	RECT rect = { 0,0, 1920, 1080 };

	mHWND = CreateWindow(L"4Quset"
		, L"4Quest"
		, WS_OVERLAPPEDWINDOW
		, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top
		, NULL, NULL, mHInstance, NULL);

	::AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false);
	
	mScreenWidth = rect.right - rect.left;
	mScreenHeight = rect.bottom - rect.top;
	mScreenLeft = rect.left;
	mScreenTop = rect.top;

	::ShowWindow(mHWND, SW_SHOWNORMAL);
	::UpdateWindow(mHWND);
}

fq::game_engine::WindowSystem::~WindowSystem()
{

}

void fq::game_engine::WindowSystem::Finalize()
{
	::DestroyWindow(mHWND);
	::UnregisterClassW(L"4Quset", mHInstance);
}

void fq::game_engine::WindowSystem::OnResize()
{
	mScreenWidth= WindowSystem::Width;
	mScreenHeight = WindowSystem::Height;

	// Event »£√‚

}

bool fq::game_engine::WindowSystem::IsResizedWindow()
{
	if (mScreenHeight == WindowSystem::Height
		&& mScreenWidth == WindowSystem::Width)
	{
		return false;
	}

	return true;
}

