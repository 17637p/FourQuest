#include "InputManager.h"

#include <cassert>

const int fq::game_module::InputManager::MatchVK[static_cast<size_t>(EKey::Last)] =
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

	'1',
	'2',
	'3',
	'4',
	'5',
	'6',
	'7',
	'8',
	'9',
	'0'
};

const int fq::game_module::InputManager::MatchPadKey[static_cast<size_t>(EPadKey::LeftTrigger)] =
{
	0x0001, //DpadUp,
	0x0002, //DpadDown,
	0x0004, //DpadLeft,
	0x0008, //DpadRight,
	0x1000, //A,
	0x2000, //B,
	0x4000, //X,
	0x8000, //Y,
	0x0010, //Start,
	0x0020, //Back,
	0x0040, //LeftThumb,
	0x0080, //RightThumb,
	0x0100, //LeftShoulder,
	0x0200, //RightShoulder,
};

short fq::game_module::InputManager::DeltaMouseWheel = 0.f;

fq::game_module::InputManager::InputManager()
	:mHWND{}
	, mDeltaMousePosition{}
	, mCurrentMousePosition{}
	, mPrevMousePosition{}
	, mPadStickInfomations{}
	, mPadVibrationInfomations{}
{}

fq::game_module::InputManager::~InputManager()
{

}

void fq::game_module::InputManager::Update()
{
	float dt = 0.f;

	updateMouse();
	updateKeybord();
	updateGamePad(dt);
	updateKeyGamePad();
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

	mDeltaMouseWheel = DeltaMouseWheel;
	DeltaMouseWheel = 0;
}

EKeyState fq::game_module::InputManager::GetKeyState(EKey key) const
{
	return mKeyInfomations[static_cast<size_t>(key)].state;
}

bool fq::game_module::InputManager::IsKeyState(EKey key, EKeyState state) const
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
		for (int i = 0; i < static_cast<int>(EKey::Last); ++i)
		{
			if (GetAsyncKeyState(MatchVK[i]) & 0x8000)
			{
				if (mKeyInfomations[i].prevPush)
				{
					mKeyInfomations[i].state = EKeyState::Hold;
				}
				else
				{
					mKeyInfomations[i].state = EKeyState::Tap;
				}
				mKeyInfomations[i].prevPush = true;
			}
			else
			{
				if (mKeyInfomations[i].prevPush)
				{
					mKeyInfomations[i].state = EKeyState::Away;
				}
				else
				{
					mKeyInfomations[i].state = EKeyState::None;
				}
				mKeyInfomations[i].prevPush = false;
			}
		}
	}
	else
	{
		for (int i = 0; i < static_cast<int>(EKey::Last); ++i)
		{
			mKeyInfomations[i].prevPush = false;
			if (EKeyState::Tap == mKeyInfomations[i].state ||
				EKeyState::Hold == mKeyInfomations[i].state)
			{
				mKeyInfomations[i].state = EKeyState::Away;
			}
			else if (EKeyState::Away == mKeyInfomations[i].state)
			{
				mKeyInfomations[i].state = EKeyState::None;
			}
		}
	}
}

float fq::game_module::InputManager::adjustStickBias(short value)
{
	constexpr short minStickBias = 10000;
	constexpr short maxStickBias = 30000;
	constexpr float maxValue = maxStickBias - minStickBias;
	constexpr float inversMaxValue = 1.f / maxValue;

	if (-minStickBias <= value && value <= minStickBias)
	{
		return 0.f;
	}
	else if (maxStickBias <= value)
	{
		return 1.f;
	}
	else if (value <= -maxStickBias)
	{
		return -1.f;
	}

	float adjustValue = 0.f;

	if (value <= 0)
	{
		adjustValue = (static_cast<float>(value) + minStickBias) * inversMaxValue;
	}
	else
	{
		adjustValue = (static_cast<float>(value) - minStickBias) * inversMaxValue;
	}

	return adjustValue;
}

void fq::game_module::InputManager::updateGamePad(float dt)
{
	DWORD dwResult = ERROR_SUCCESS;
	HWND hwnd = GetFocus();

	if (hwnd != nullptr)
	{
		for (DWORD i = 0; i < XUSER_MAX_COUNT; i++)
		{
			// 컨트롤러 입력 상태 초기화
			XINPUT_STATE state;
			ZeroMemory(&state, sizeof(XINPUT_STATE));

			// 컨트롤러 입력 상태 받아오기
			dwResult = XInputGetState(i, &state);

			if (dwResult == ERROR_SUCCESS)
			{
				for (size_t j = 0; j < static_cast<size_t>(EPadKey::Last); ++j)
				{
					bool isPressed = false;
					if (j <= static_cast<size_t>(EPadKey::RightShoulder))
					{
						isPressed = state.Gamepad.wButtons & MatchPadKey[j];
					}
					else if (j == static_cast<size_t>(EPadKey::LeftTrigger))
					{
						isPressed = state.Gamepad.bLeftTrigger;
					}
					else if (j == static_cast<size_t>(EPadKey::RightTrigger))
					{
						isPressed = state.Gamepad.bRightTrigger;
					}

					auto& padKeyInfo = mPadKeyInfomations[i][j];

					if (isPressed && !padKeyInfo.prevPush)
						padKeyInfo.state = EKeyState::Tap;
					else if (isPressed && padKeyInfo.prevPush)
						padKeyInfo.state = EKeyState::Hold;
					else if (isPressed && padKeyInfo.prevPush)
						padKeyInfo.state = EKeyState::Away;
					else
						padKeyInfo.state = EKeyState::None;

					padKeyInfo.prevPush = isPressed;
				}
			}

			// Stick 관련 정보 저장
			mPadStickInfomations[i].leftX = adjustStickBias(state.Gamepad.sThumbLX);
			mPadStickInfomations[i].leftY = adjustStickBias(state.Gamepad.sThumbLY);
			mPadStickInfomations[i].rightX = adjustStickBias(state.Gamepad.sThumbRX);
			mPadStickInfomations[i].rightY = adjustStickBias(state.Gamepad.sThumbRY);

			// 진동 관련 처리 
			auto& vibrationInfo = mPadVibrationInfomations[i];

			if (vibrationInfo.leftDuration != VibrationInfo::VibrationEnd)
			{
				vibrationInfo.leftDuration -= dt;

				// 왼쪽 진동 종료
				if (vibrationInfo.leftDuration <= 0.f)
				{
					vibrationInfo.leftDuration = VibrationInfo::VibrationEnd;

					XINPUT_VIBRATION vibration;
					ZeroMemory(&vibration, sizeof(XINPUT_VIBRATION));
					vibration.wLeftMotorSpeed = 0; // 왼쪽 진동 모터의 세기

					if (vibrationInfo.rightDuration == VibrationInfo::VibrationEnd)
					{
						vibration.wRightMotorSpeed = 0; // 오른쪽 진동 모터의 세기
					}
					else
					{
						vibration.wRightMotorSpeed = mPadVibrationInfomations[i].rightIntensity;
					}

					XInputSetState(i, &vibration);
				}
			}

			if (vibrationInfo.rightDuration != VibrationInfo::VibrationEnd)
			{
				vibrationInfo.rightDuration -= dt;

				// 오른쪽 진동 종료
				if (vibrationInfo.rightDuration <= 0.f)
				{
					vibrationInfo.rightDuration = VibrationInfo::VibrationEnd;

					XINPUT_VIBRATION vibration;
					ZeroMemory(&vibration, sizeof(XINPUT_VIBRATION));
					vibration.wRightMotorSpeed = 0; // 오른쪽 진동 모터의 세기

					if (vibrationInfo.leftDuration == VibrationInfo::VibrationEnd)
					{
						vibration.wLeftMotorSpeed = 0; // 왼쪽 진동 모터의 세기
					}
					else
					{
						vibration.wLeftMotorSpeed = mPadVibrationInfomations[i].leftIntensity;
					}

					XInputSetState(i, &vibration);
				}
			}
		}
	}
	else
	{
		for (size_t j = 0; j < static_cast<size_t>(EPadKey::Last); ++j)
		{
			for (int i = 0; i < 4; ++i)
			{
				if (mPadKeyInfomations[i][j].prevPush)
				{
					mPadKeyInfomations[i][j].state = EKeyState::Away;
				}
				else
				{
					mPadKeyInfomations[i][j].state = EKeyState::None;
				}
			}
		}

		for (int i = 0; i < 4; ++i)
		{
			mPadStickInfomations[i].leftX = 0.f;
			mPadStickInfomations[i].leftY = 0.f;
			mPadStickInfomations[i].rightX = 0.f;
			mPadStickInfomations[i].rightY = 0.f;
		}
	}
}

EKeyState fq::game_module::InputManager::GetPadKeyState(PadID id, EPadKey key) const
{
	assert(id <= 3);
	return mPadKeyInfomations[id][static_cast<size_t>(key)].state;
}

bool fq::game_module::InputManager::IsPadKeyState(PadID id, EPadKey key, EKeyState keyState) const
{
	return keyState == GetPadKeyState(id, key);
}

void fq::game_module::InputManager::SetVibration(PadID id, EVibrationMode mode, unsigned short intensity, float duration)
{
	assert(id <= 3);

	if (mode == EVibrationMode::Left)
	{
		mPadVibrationInfomations[id].leftIntensity = intensity;
		mPadVibrationInfomations[id].leftDuration = duration;
	}
	else if (mode == EVibrationMode::Right)
	{
		mPadVibrationInfomations[id].rightIntensity = intensity;
		mPadVibrationInfomations[id].rightDuration = duration;
	}
	else if (mode == EVibrationMode::Both)
	{
		mPadVibrationInfomations[id].leftIntensity = intensity;
		mPadVibrationInfomations[id].leftDuration = duration;

		mPadVibrationInfomations[id].rightIntensity = intensity;
		mPadVibrationInfomations[id].rightDuration = duration;
	}

	// Pad 진동 관련
	XINPUT_VIBRATION vibration;
	ZeroMemory(&vibration, sizeof(XINPUT_VIBRATION));
	vibration.wLeftMotorSpeed = mPadVibrationInfomations[id].leftIntensity; // 왼쪽 진동 모터의 세기0
	vibration.wRightMotorSpeed = mPadVibrationInfomations[id].rightIntensity; // 오른쪽 진동 모터의 세기
	XInputSetState(id, &vibration);
}

float fq::game_module::InputManager::GetStickInfomation(PadID id, EPadStick padStick) const
{
	assert(id <= 3);

	float value = 0.f;
	switch (padStick)
	{
		case EPadStick::leftX:
		{
			value = mPadStickInfomations[id].leftX;
		}
		break;
		case EPadStick::leftY:
		{
			value = mPadStickInfomations[id].leftY;
		}
		break;
		case EPadStick::rightX:
		{
			value = mPadStickInfomations[id].rightX;
		}
		break;
		case EPadStick::rightY:
		{
			value = mPadStickInfomations[id].rightY;
		}
		break;
		default:
			break;
	}

	return value;
}

void fq::game_module::InputManager::updateKeyGamePad()
{
	// 게임패드를 키보로 입력가능하게 설정

	// Player 0 
	if (!IsKeyState(EKey::W, EKeyState::None))
	{
		mPadStickInfomations[0].leftY = 1.f;
	}
	if (!IsKeyState(EKey::S, EKeyState::None))
	{
		mPadStickInfomations[0].leftY = -1.f;
	}
	if (!IsKeyState(EKey::A, EKeyState::None))
	{
		mPadStickInfomations[0].leftX = -1.f;
	}
	if (!IsKeyState(EKey::D, EKeyState::None))
	{
		mPadStickInfomations[0].leftX = 1.f;
	}

	// X 
	if (!IsKeyState(EKey::R, EKeyState::None))
	{
		mPadKeyInfomations[0][static_cast<size_t>(EPadKey::X)].state = GetKeyState(EKey::R);
	}
	// Y
	if (!IsKeyState(EKey::T, EKeyState::None))
	{
		mPadKeyInfomations[0][static_cast<size_t>(EPadKey::Y)].state = GetKeyState(EKey::T);
	}
	// A
	if (!IsKeyState(EKey::Y, EKeyState::None))
	{
		mPadKeyInfomations[0][static_cast<size_t>(EPadKey::A)].state = GetKeyState(EKey::Y);
	}

	// Player 1
	if (!IsKeyState(EKey::Up, EKeyState::None))
	{
		mPadStickInfomations[1].leftY = 1.f;
	}
	if (!IsKeyState(EKey::Down, EKeyState::None))
	{
		mPadStickInfomations[1].leftY = -1.f;
	}
	if (!IsKeyState(EKey::Left, EKeyState::None))
	{
		mPadStickInfomations[1].leftX = -1.f;
	}
	if (!IsKeyState(EKey::Right, EKeyState::None))
	{
		mPadStickInfomations[1].leftX = 1.f;
	}

	// X 
	if (!IsKeyState(EKey::R, EKeyState::None))
	{
		mPadKeyInfomations[1][static_cast<size_t>(EPadKey::X)].state = GetKeyState(EKey::B);
	}
	// Y
	if (!IsKeyState(EKey::T, EKeyState::None))
	{
		mPadKeyInfomations[1][static_cast<size_t>(EPadKey::Y)].state = GetKeyState(EKey::N);
	}
	// A
	if (!IsKeyState(EKey::Y, EKeyState::None))
	{
		mPadKeyInfomations[1][static_cast<size_t>(EPadKey::A)].state = GetKeyState(EKey::M);
	}

}
