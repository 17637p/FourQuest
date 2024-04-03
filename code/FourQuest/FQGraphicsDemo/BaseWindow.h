#pragma once
#include <windows.h>

/// <summary>
/// WinAPI의 message를 좀더 깔끔하게 다루기 위해 노력한 흔적
/// 
/// msdn 설명을 바탕으로 재구성 해봤다.
/// https://learn.microsoft.com/ko-kr/windows/win32/learnwin32/managing-application-state-
/// 샘플코드
/// https://github.com/microsoft/Windows-classic-samples/tree/main/Samples/Win7Samples/begin/LearnWin32/BaseWindow
/// 
/// KGIT 5기 1반 김유진
/// </summary>
/// <typeparam name="DERIVED_TYPE">상속받는 클래스 명을 넣어주면 된다.</typeparam>
template <class DERIVED_TYPE>
class BaseWindow
{
public:
	BaseWindow(PCWSTR className = L"No Name") : mHwnd(NULL), mClassName(className) { }

	/// <summary>
	/// CRTP를 사용해서 windowProc을 클래스의 멤버 함수로 만들 수 있게 해주는 핵심
	/// </summary>
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		DERIVED_TYPE* pThis = NULL;

		if (uMsg == WM_NCCREATE)
		{
			// 만든 윈도우
			CREATESTRUCT* pCreate = (CREATESTRUCT*)lParam;

			// CreateWindowEx 함수를 호출할 때 넣어준 this 포인터(마지막 파라미터)를 가져옴 (사용자가 넣어준 값)
			pThis = (DERIVED_TYPE*)pCreate->lpCreateParams;

			// 창의 특성을 변경
			SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pThis);

			// 밑에서 Create할 때 m_hwnd 넣어준다는 것 pThis->m_hwnd 에 넣어주는 거랑 같은 거 아닐까?
			// 자식에도 m_hwnd있다면 당연히 아니다
			// 아니다...!...?
			pThis->mHwnd = hwnd;
		}
		else
		{
			pThis = (DERIVED_TYPE*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
		}

		// pThis가 널이 아니라면 == 창이 생성 되었다면
		if (pThis)
		{
			return pThis->HandleMessage(uMsg, wParam, lParam);
		}
		else
		{
			return DefWindowProc(hwnd, uMsg, wParam, lParam);
		}
	}

	/// <summary>
	/// 항상 보던 hWnd를 만드는 함수
	/// 
	/// hWnd 생성에 실패하면 false를 리턴한다.
	/// </summary>
	BOOL CreateHWND(
		PCWSTR lpWindowName,
		DWORD dwStyle,
		int x = CW_USEDEFAULT,
		int y = CW_USEDEFAULT,
		int nWidth = CW_USEDEFAULT,
		int nHeight = CW_USEDEFAULT,
		DWORD dwExStyle = 0,
		HWND hWndParent = 0,
		HMENU hMenu = 0
	)
	{
		WNDCLASS wc = { 0 };

		wc.lpfnWndProc = DERIVED_TYPE::WindowProc;
		wc.hInstance = GetModuleHandle(NULL);
		wc.lpszClassName = mClassName;

		RegisterClass(&wc);

		RECT rect;
		SetRect(&rect, 0, 0, nWidth, nHeight);
		AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

		mHwnd = CreateWindowEx(
			dwExStyle, mClassName, lpWindowName, dwStyle, x, y,
			rect.right - rect.left, rect.bottom - rect.top, hWndParent, hMenu, GetModuleHandle(NULL), this
		);

		return (mHwnd ? TRUE : FALSE);
	}

	HWND GetHWND() const { return mHwnd; }

protected:
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) abstract;

	HWND mHwnd;
	PCWSTR mClassName; // 이거 포인터면 해제해야 하나?

	//virtual PCWSTR  ClassName() const abstract; 함수로 만드는 이유를 모르겠다. 함수 한 번 더 호출할 거면 그냥 변수가 낫지 않나? 
};