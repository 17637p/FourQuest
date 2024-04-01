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
		/// �̹� �����ӿ� Ű ����
		// ���� �����ӿ� None or Up �̾��µ� ���ȴ�. -> Down
		if ((mKey[i] == KeyState::None || mKey[i] == KeyState::Up)
			&& GetAsyncKeyState(i) & 0x8000)
		{
			mKey[i] = KeyState::Down;
			mIsKeyDown = true;
		}
		// ���� �����ӿ� Down �̾��µ� ���ȴ�. -> Press
		else if (mKey[i] == KeyState::Down && GetAsyncKeyState(i) & 0x8000)
		{
			mKey[i] = KeyState::Press;
			mIsKey = true;
		}

		/// �̹� �����ӿ� Ű �� ����
		// ���� �����ӿ� Down, Press �̾��µ� �ȴ�����. -> Up
		else if ((mKey[i] == KeyState::Down || mKey[i] == KeyState::Press)
			&& !(GetAsyncKeyState(i) & 0x8000))
		{
			mKey[i] = KeyState::Up;
			mIsKeyUp = true;
		}
		// ���� �����ӿ� Up �̾��µ� �ȴ�����.
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
