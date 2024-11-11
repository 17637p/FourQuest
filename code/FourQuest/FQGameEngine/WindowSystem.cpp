#include "WindowSystem.h"
#include "IEngine.h"
#include "WndProc.h"

#include "../FQGameEngineDemo/resource.h"

// Window 이벤트용도의 static 변수
UINT fq::game_engine::WindowSystem::Height;
UINT fq::game_engine::WindowSystem::Width;
std::vector<std::wstring> fq::game_engine::WindowSystem::DropFiles{};

fq::game_engine::WindowSystem::WindowSystem()
	:mHWND{}
	, mHInstance{}
	, mScreenLeft{}
	, mScreenTop{}
	, mMonitorHeight{}
	, mMonitorWidth{}
{}

void fq::game_engine::WindowSystem::Initialize(UINT screenWidth, UINT screenHeight)
{
	mHInstance = ::GetModuleHandle(NULL);
	//mHInstance = GetModuleHandle(TEXT("FQGameEngine.dll"));

	WNDCLASS wndClass{};
	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = WndProc::GameWndProc;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hInstance = mHInstance;
	wndClass.hIcon = ::LoadIcon(mHInstance, MAKEINTRESOURCE(IDI_ICON1));
	wndClass.hCursor = ::LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = static_cast<HBRUSH>(::GetStockObject(WHITE_BRUSH));
	wndClass.lpszMenuName = NULL;
	wndClass.lpszClassName = L"4Quset";

	RegisterClass(&wndClass);

	// 모니터 해상도 
	mMonitorWidth = GetSystemMetrics(SM_CXSCREEN);
	mMonitorHeight = GetSystemMetrics(SM_CYSCREEN);

	RECT rect = {};
	rect.left = mMonitorWidth / 2 - screenWidth / 2;
	rect.right = mMonitorWidth / 2 + screenWidth / 2;
	rect.top = mMonitorHeight / 2 - screenHeight / 2;
	rect.bottom = mMonitorHeight / 2 + screenHeight / 2;

	::AdjustWindowRect(&rect, WS_OVERLAPPED |
		WS_CAPTION |
		WS_SYSMENU |
		WS_MINIMIZEBOX |
		WS_MAXIMIZEBOX, false);

	mHWND = CreateWindow(L"4Quset"
		, L"4Quest"
		, WS_OVERLAPPED |
		WS_CAPTION |
		WS_SYSMENU |
		WS_MINIMIZEBOX |
		WS_MAXIMIZEBOX
		, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top
		, NULL, NULL, mHInstance, NULL);

	// 화면 해상도
	mScreenWidth = screenWidth;
	mScreenHeight = screenHeight;
	mScreenLeft = rect.left;//rect.left;
	mScreenTop = rect.top;// rect.top;

	::ShowWindow(mHWND, SW_SHOWNORMAL);
	::UpdateWindow(mHWND);
}


void fq::game_engine::WindowSystem::InitializeEditorType(UINT screenWidth, UINT screenHeight)
{
	mHInstance = ::GetModuleHandle(NULL);
	//mHInstance = GetModuleHandle(TEXT("FQGameEngine.dll"));

	WNDCLASS wndClass{};

	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = WndProc::ToolWndProc;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hInstance = mHInstance;
	wndClass.hIcon = ::LoadIcon(mHInstance, MAKEINTRESOURCE(IDI_ICON1));
	wndClass.hCursor = ::LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = static_cast<HBRUSH>(::GetStockObject(WHITE_BRUSH));
	wndClass.lpszMenuName = NULL;
	wndClass.lpszClassName = L"4Quset";

	RegisterClass(&wndClass);

	// 모니터 해상도 
	mMonitorWidth = GetSystemMetrics(SM_CXSCREEN);
	mMonitorHeight = GetSystemMetrics(SM_CYSCREEN);

	RECT rect = {};
	rect.left = mMonitorWidth / 2 - screenWidth / 2;
	rect.right = mMonitorWidth / 2 + screenWidth / 2;
	rect.top = mMonitorHeight / 2 - screenHeight / 2;
	rect.bottom = mMonitorHeight / 2 + screenHeight / 2;

	::AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false);

	mHWND = CreateWindow(L"4Quset"
		, L"4Quest"
		, WS_OVERLAPPEDWINDOW
		, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top
		, NULL, NULL, mHInstance, NULL);

	// DragDrop 이벤트를 활성화
	DragAcceptFiles(mHWND, true);

	// 화면 해상도
	mScreenWidth = screenWidth;
	mScreenHeight = screenHeight;
	mScreenLeft = rect.left;//rect.left;
	mScreenTop = rect.top;// rect.top;

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
	mScreenWidth = WindowSystem::Width;
	mScreenHeight = WindowSystem::Height;
}

bool fq::game_engine::WindowSystem::IsResizedWindow()const
{
	if (mScreenHeight == WindowSystem::Height
		&& mScreenWidth == WindowSystem::Width)
	{
		return false;
	}

	return true;
}


