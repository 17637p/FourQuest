#include "ScreenManager.h"


#include "EventManager.h"
#include "Event.h"

fq::game_module::ScreenManager::ScreenManager()
	:mScreenHeight(0)
	, mScreenWidth(0)
	, mFixScreenWidth(0)
	, mFixScreenHeight(0)
	, mHwnd{}
	, mPrevRect{}
	, mbIsFullScreen(false)
{

}

fq::game_module::ScreenManager::~ScreenManager()
{

}

void fq::game_module::ScreenManager::Initialize(EventManager* eventMgr, UINT screenWidth, UINT screenHeight, HWND hwnd)
{
	mHwnd = hwnd;

	mSetScreenSizeHanlder = eventMgr->RegisterHandle<fq::event::SetScreenSize>(
		[this](fq::event::SetScreenSize event) {
			mScreenHeight = event.height;
			mScreenWidth = event.width;

			// 16:9 ���� �����ϱ� ���� �����ϸ� ���
			float renderAspect = 16.0f / 9.0f;
			float windowAspect = (float)mScreenWidth / mScreenHeight;

			if (windowAspect > renderAspect)
			{
				// â�� �� ���� ���
				mFixScreenWidth = mScreenHeight * renderAspect;
				mFixScreenHeight = mScreenHeight;
			}
			else
			{
				// â�� �� ���� ���
				mFixScreenWidth = mScreenWidth;
				mFixScreenHeight = mScreenWidth / renderAspect;
			}
		});

	mScreenHeight = screenHeight;
	mScreenWidth = screenWidth;

	// 16:9 ���� �����ϱ� ���� �����ϸ� ���
	float renderAspect = 16.0f / 9.0f;
	float windowAspect = (float)mScreenWidth / mScreenHeight;

	if (windowAspect > renderAspect)
	{
		// â�� �� ���� ���
		mFixScreenWidth = mScreenHeight * renderAspect;
		mFixScreenHeight = mScreenHeight;
	}
	else
	{
		// â�� �� ���� ���
		mFixScreenWidth = mScreenWidth;
		mFixScreenHeight = mScreenWidth / renderAspect;
	}

	GetWindowRect(mHwnd, &mPrevRect);
}

void fq::game_module::ScreenManager::SetScreenSize(UINT width, UINT height)
{
	mbIsFullScreen = false;

	// ����� �ػ� 
	int monitorWidth = GetSystemMetrics(SM_CXSCREEN);
	int monitorHeight = GetSystemMetrics(SM_CYSCREEN);

	RECT rect = {};
	rect.left = monitorWidth / 2 - width / 2;
	rect.right = monitorWidth / 2 + width / 2;
	rect.top = monitorHeight / 2 - height / 2;
	rect.bottom = monitorHeight / 2 + height / 2;

	::AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false);

	SetWindowLong(mHwnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);
	::SetWindowPos(mHwnd, NULL, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, SWP_SHOWWINDOW);
}

void fq::game_module::ScreenManager::SetFullScreen(bool isFull)
{
	if (mbIsFullScreen == isFull)
	{
		return;
	}

	if (isFull)
	{
		// ���� ȭ�� �ػ� ���� 
		GetWindowRect(mHwnd, &mPrevRect);

		// ����� �ػ� 
		int monitorWidth = GetSystemMetrics(SM_CXSCREEN);
		int monitorHeight = GetSystemMetrics(SM_CYSCREEN);

		// ������ ũ�� ����
		SetWindowLong(mHwnd, GWL_STYLE, WS_POPUP);
		SetWindowPos(mHwnd, HWND_TOP, 0, 0, monitorWidth, monitorHeight, SWP_SHOWWINDOW);
	}
	else
	{
		SetWindowLong(mHwnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);
		SetWindowPos(mHwnd, NULL, mPrevRect.left, mPrevRect.top, mPrevRect.right - mPrevRect.left, mPrevRect.bottom - mPrevRect.top, SWP_SHOWWINDOW);
	}

	mbIsFullScreen = isFull;
}
