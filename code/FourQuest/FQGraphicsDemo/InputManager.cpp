#include "InputManager.h"

void InputManager::Init(HWND hwnd)
{
	mHwnd = hwnd;
	for (int i = 0; i < 255; i++)
	{
		mKey[i] = KeyState::None;
	}
}

void InputManager::Update()
{
	mIsKeyDown = false;
	mIsKey = false;
	mIsKeyUp = false;

	for (int i = 0; i < 255; i++)
	{
		/// 이번 프레임에 키 누름
		// 이전 프레임에 None or Up 이었는데 눌렸다. -> Down
		if ((mKey[i] == KeyState::None || mKey[i] == KeyState::Up)
			&& GetAsyncKeyState(i) & 0x8000)
		{
			mKey[i] = KeyState::Down;
			mIsKeyDown = true;
		}
		// 이전 프레임에 Down 이었는데 눌렸다. -> Press
		else if (mKey[i] == KeyState::Down && GetAsyncKeyState(i) & 0x8000)
		{
			mKey[i] = KeyState::Press;
			mIsKey = true;
		}

		/// 이번 프레임에 키 안 누름
		// 이전 프레임에 Down, Press 이었는데 안눌렀다. -> Up
		else if ((mKey[i] == KeyState::Down || mKey[i] == KeyState::Press)
			&& !(GetAsyncKeyState(i) & 0x8000))
		{
			mKey[i] = KeyState::Up;
			mIsKeyUp = true;
		}
		// 이전 프레임에 Up 이었는데 안눌렀다.
		else if (mKey[i] == KeyState::Up && !(GetAsyncKeyState(i) & 0x8000))
		{
			mKey[i] = KeyState::None;
		}
	}

	mPreMousePosition = mMousePosition;
	mMousePosition = GetMousePosition();
}

POINT InputManager::GetMousePosition()
{
	POINT mousePos;
	GetCursorPos(&mousePos);
	ScreenToClient(mHwnd, &mousePos);

	return mousePos;
}

POINT InputManager::GetDeltaPosition()
{
	return { mMousePosition.x - mPreMousePosition.x,  mMousePosition.y - mPreMousePosition.y };
}
