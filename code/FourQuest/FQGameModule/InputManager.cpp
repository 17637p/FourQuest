#include "InputManager.h"

const int fq::game_module::InputManager::MatchVK[static_cast<size_t>(Key::Last)] =
{
		VK_LEFT, VK_RIGHT, VK_UP, VK_DOWN,
		'Q','W','E','R','T','Y','U','O','P',
		'A','S','D','F','G','H','J','K','L',
		'Z','X','C','V','B','N','M',

		VK_MENU, // ALT,
		VK_CONTROL, // CTRL,
		VK_LSHIFT, // LSHIFT,
		VK_SPACE, // SPACE,
		VK_RETURN, // ENTER,
		VK_ESCAPE, // ESC,
		VK_TAB, //TAB

		VK_LBUTTON, // LBTN
		VK_RBUTTON, // RBTN

		VK_F1, // F1
		VK_F2, // F2
		VK_F3, // F3
		VK_F4, // F4
		VK_F5, // F5
		VK_F6, // F6
		VK_F7, // F7
		VK_F8, // F8

	   VK_INSERT, //        Ins,
	   VK_DELETE,//Del,
	   VK_HOME,//Home,
	   VK_END,   //End,
	   VK_PRIOR,//PgUp,
	   VK_NEXT,//PgDn,
};


fq::game_module::InputManager::InputManager()
	:mHWND{}
	, mDeltaMousePosition{}
	, mCurrentMousePosition{}
	, mPrevMousePosition{}
{}

fq::game_module::InputManager::~InputManager()
{

}

void fq::game_module::InputManager::Update()
{
	updateMouse();
	updateKeybord();
	updateGamePad();
}

void fq::game_module::InputManager::Initialize(HWND hwnd)
{
	mHWND = hwnd;
}

void fq::game_module::InputManager::updateMouse()
{
	mPrevMousePosition = mCurrentMousePosition;

	GetCursorPos(&mCurrentMousePosition);
	ScreenToClient(mHWND, &mCurrentMousePosition);

	mDeltaMousePosition.x = mCurrentMousePosition.x - mPrevMousePosition.x;
	mDeltaMousePosition.y = mCurrentMousePosition.y - mPrevMousePosition.y;
}

KeyState fq::game_module::InputManager::GetKeyState(Key key) const
{
	return mKeyInfomations[static_cast<size_t>(key)].state;
}

bool fq::game_module::InputManager::IsKeyState(Key key, KeyState state) const
{
	if (state == GetKeyState(key))
	{
		return true;
	}

	return false;
}

void fq::game_module::InputManager::updateKeybord()
{
	HWND hwnd = ::GetFocus();

	if (hwnd != nullptr)
	{
		for (int i = 0; i < static_cast<int>(Key::Last); ++i)
		{
			if (GetAsyncKeyState(MatchVK[i]) & 0x8000)
			{
				if (mKeyInfomations[i].prevPush)
				{
					mKeyInfomations[i].state = KeyState::Hold;
				}
				else
				{
					mKeyInfomations[i].state = KeyState::Tap;
				}
				mKeyInfomations[i].prevPush = true;
			}
			else
			{
				if (mKeyInfomations[i].prevPush)
				{
					mKeyInfomations[i].state = KeyState::Away;
				}
				else
				{
					mKeyInfomations[i].state = KeyState::None;
				}
				mKeyInfomations[i].prevPush = false;
			}
		}
	}
	else
	{
		for (int i = 0; i < static_cast<int>(Key::Last); ++i)
		{
			mKeyInfomations[i].prevPush = false;
			if (KeyState::Tap == mKeyInfomations[i].state ||
				KeyState::Hold == mKeyInfomations[i].state)
			{
				mKeyInfomations[i].state = KeyState::Away;
			}
			else if (KeyState::Away == mKeyInfomations[i].state)
			{
				mKeyInfomations[i].state = KeyState::None;
			}
		}
	}

}

void fq::game_module::InputManager::updateGamePad()
{

}

