#pragma once
/// 마우스 처리를 위해 HWND를 쓰려면 include 를 해야한다.
#include <windows.h>

#include "Singleton.h"

/// <summary>
/// 키보드 상태
/// </summary>
enum class KeyState
{
	None,	// 이전 프레임x, 현재 프레임 x
	Down,	// 이전 프레임x, 현재 프레임 o
	Press,	// 이전 프레임o, 현재 프레임 o
	Up		// 이전 프레임o, 현재 프레임 x
};

/// <summary>
/// 마우스, 키보드의 입력 처리를 담당한다.
/// 
/// 싱글톤으로 만든다. (싱글톤 템플릿을 상속받는다.)
/// 2023.07.24 김유진
/// </summary>
class InputManager : public Singleton<InputManager>
{
public:
	void Init(HWND hwnd);
	void Update();

	// 키보드, 마우스 둘다 처리가 가능하다. 가상 키에 이미 Mouse가 포함되어 있다.
	bool IsGetKeyDown(int key) const { return mKey[key] == KeyState::Down; }
	bool IsGetKey(int key) const { return mKey[key] == KeyState::Press; }
	bool IsGetKeyUp(int key) const { return mKey[key] == KeyState::Up; }

	bool IsAnyKeyDown() const { return mIsKeyDown; }
	bool IsAnyKey() const { return mIsKey; } 
	bool IsAnyKeyUp() const { return mIsKeyUp; }

	// 카메라도 일단 생각 안하고 만든다. 이후 카메라가 추가되면 변경 필요할수도 있음!
	// 일단 hwnd가 하나라고 가정하고 만든다. 이후 늘어난다면 변경해야 됨.
	POINT GetMousePosition();
	POINT GetDeltaPosition();

	void SetWheelPower(int power) { mWheelPower = power; }
	int GetWheelPower() const { return mWheelPower; }

private:
	// 한 개의 클래스라는 것을 보장하려면 생성자를 숨겨야 한다.
	InputManager() : mHwnd(0), mKey{}, mWheelPower(0), mIsKeyDown(false), mIsKey(0), mIsKeyUp(0), mPreMousePosition(), mMousePosition() {}
	~InputManager() {}

private:
	HWND mHwnd;
	int mWheelPower;

	bool mIsKeyDown;
	bool mIsKey;
	bool mIsKeyUp;

	POINT mPreMousePosition;
	POINT mMousePosition;

	KeyState mKey[0xFE /*VK_OEM_CLEAR*/ + 1];	// 255, https://learn.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes 모든 가상 키에 대응할 수 있게 크기를 정한다.

	friend class Singleton<InputManager>;
};