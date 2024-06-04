#pragma once

#include <windows.h>

/// XInput
#include <Xinput.h>
#pragma comment(lib, "xinput.lib")

#include "InputEnum.h"

namespace fq::game_module
{
	using PadID = unsigned int; 

	class InputManager
	{
		struct KeyInfo
		{
			EKeyState state = EKeyState::None;
			bool prevPush = false;
		};

		struct PadStickInfo
		{
			float leftX = 0.f;
			float leftY = 0.f;
			float rightX = 0.f;
			float rightY = 0.f;
		};

		struct VibrationInfo
		{
			unsigned short leftIntensity = 0;
			unsigned short rightIntensity = 0;

			static constexpr float VibrationEnd = -1.f;

			float leftDuration = VibrationEnd;
			float rightDuration = VibrationEnd;
		};

	public:
		struct Vibration
		{
			EVibrationMode mode;
			unsigned int intensity = 3000;
			float second = 1.f;
		};

	public:
		InputManager();
		~InputManager();

		/// <summary>
		/// InputManager �ʱ�ȭ
		/// </summary>
		/// <param name="hwnd">������ �ڵ�</param>
		void Initialize(HWND hwnd);

		/// <summary>
		/// ���콺, Ű�Է�, �����е忡 ���� �Է�ó��
		/// </summary>
		void Update();

		/// <summary>
		/// ���� ������ ���콺 ��ġ ��ȯ
		/// </summary>
		/// <returns>���콺 ��ġ</returns>
		POINT GetMousePosition()const { return mCurrentMousePosition; }

		/// <summary>
		/// ���� ������ ���콺 ��ġ ��ȯ
		/// </summary>
		/// <returns>���� ������ ���콺 ��ġ</returns>
		POINT GetPrevMousePosition()const { return mPrevMousePosition; }

		/// <summary>
		/// �̹� ������ ���콺 ��ġ ��ȭ�� 
		/// </summary>
		/// <returns>���콺 ��ȭ��</returns>
		POINT GetDeltaMousePosition()const { return mDeltaMousePosition; }

		//////////////////////////////////////////////////////////////////////////
		//									Ű����								//
		//////////////////////////////////////////////////////////////////////////

		/// <summary>
		/// �̹��������� Key���¸� ��ȯ�մϴ�
		/// </summary>
		/// <param name="key">Key</param>
		/// <returns>�̹������� ����</returns>
		EKeyState GetKeyState(EKey key)const;

		/// <summary>
		/// �̹� ���������� Key�� KeyState�� ��ġ�ϴ��� Ȯ���մϴ�.
		/// </summary>
		/// <param name="key">Key</param>
		/// <param name="state">KetState</param>
		/// <returns>��ġ�ϸ� true, �׷��������� false</returns>
		bool IsKeyState(EKey key, EKeyState state) const;

		/// <summary>
		/// ���콺 �� ��ȭ���� ��ȯ�մϴ� 
		/// </summary>
		short GetDeltaMouseWheel() const { return mDeltaMouseWheel; }

		//////////////////////////////////////////////////////////////////////////
		//									�����е�								//
		//////////////////////////////////////////////////////////////////////////
		
		/// <summary>
		/// �е��� �Է»��¸� Ȯ���մϴ�
		/// </summary>
		EKeyState GetPadKeyState(PadID id, EPadKey key)const;
	
		/// <summary>
		/// �е��� �Է»��¸� Ȯ���մϴ�.
		/// </summary>
		bool IsPadKeyState(PadID id, EPadKey key, EKeyState keyState) const;
		
		/// <summary>
		/// ��ƽ�� �������¸� �����մϴ�.
		/// </summary>
		void SetVibration(PadID id, EVibrationMode mode, unsigned short intensity, float duration);

		/// <summary>
		///  -1.f ~ 1.f ������ ��ƽ�� ���������� ��ȯ�մϴ�.
		/// </summary>
		float GetStickInfomation(PadID id, EPadStick padStick) const;

	private:
		void updateMouse();
		void updateKeybord();
		void updateGamePad(float dt);
		float adjustStickBias(short value);

	public:
		static short DeltaMouseWheel;

	private:
		static const int MatchVK[static_cast<size_t>(EKey::Last)];
		static const int MatchPadKey[static_cast<size_t>(EPadKey::LeftTrigger)];

		HWND mHWND;

		// ���콺 
		POINT mCurrentMousePosition;
		POINT mPrevMousePosition;
		POINT mDeltaMousePosition;
		short mDeltaMouseWheel;

		// Ű���� 
		KeyInfo mKeyInfomations[static_cast<size_t>(EKey::Last)];

		// �����е�
		KeyInfo mPadKeyInfomations[XUSER_MAX_COUNT][static_cast<size_t>(EPadKey::Last)];
		PadStickInfo mPadStickInfomations[XUSER_MAX_COUNT];
		VibrationInfo mPadVibrationInfomations[XUSER_MAX_COUNT];


	};
}