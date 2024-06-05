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
		/// InputManager 초기화
		/// </summary>
		/// <param name="hwnd">윈도우 핸들</param>
		void Initialize(HWND hwnd);

		/// <summary>
		/// 마우스, 키입력, 게임패드에 대한 입력처리
		/// </summary>
		void Update();

		/// <summary>
		/// 현재 프레임 마우스 위치 반환
		/// </summary>
		/// <returns>마우스 위치</returns>
		POINT GetMousePosition()const { return mCurrentMousePosition; }

		/// <summary>
		/// 이전 프레임 마우스 위치 반환
		/// </summary>
		/// <returns>이전 프레임 마우스 위치</returns>
		POINT GetPrevMousePosition()const { return mPrevMousePosition; }

		/// <summary>
		/// 이번 프레임 마우스 위치 변화량 
		/// </summary>
		/// <returns>마우스 변화량</returns>
		POINT GetDeltaMousePosition()const { return mDeltaMousePosition; }

		//////////////////////////////////////////////////////////////////////////
		//									키보드								//
		//////////////////////////////////////////////////////////////////////////

		/// <summary>
		/// 이번프레임의 Key상태를 반환합니다
		/// </summary>
		/// <param name="key">Key</param>
		/// <returns>이번프레임 상태</returns>
		EKeyState GetKeyState(EKey key)const;

		/// <summary>
		/// 이번 프레임이의 Key가 KeyState와 일치하는지 확인합니다.
		/// </summary>
		/// <param name="key">Key</param>
		/// <param name="state">KetState</param>
		/// <returns>일치하면 true, 그렇지않으면 false</returns>
		bool IsKeyState(EKey key, EKeyState state) const;

		/// <summary>
		/// 마우스 휠 변화량을 반환합니다 
		/// </summary>
		short GetDeltaMouseWheel() const { return mDeltaMouseWheel; }

		//////////////////////////////////////////////////////////////////////////
		//									게임패드								//
		//////////////////////////////////////////////////////////////////////////
		
		/// <summary>
		/// 패드의 입력상태를 확인합니다
		/// </summary>
		EKeyState GetPadKeyState(PadID id, EPadKey key)const;
	
		/// <summary>
		/// 패드의 입력상태를 확인합니다.
		/// </summary>
		bool IsPadKeyState(PadID id, EPadKey key, EKeyState keyState) const;
		
		/// <summary>
		/// 스틱의 진동상태를 설정합니다.
		/// </summary>
		void SetVibration(PadID id, EVibrationMode mode, unsigned short intensity, float duration);

		/// <summary>
		///  -1.f ~ 1.f 사이의 스틱의 방향정보를 반환합니다.
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

		// 마우스 
		POINT mCurrentMousePosition;
		POINT mPrevMousePosition;
		POINT mDeltaMousePosition;
		short mDeltaMouseWheel;

		// 키보드 
		KeyInfo mKeyInfomations[static_cast<size_t>(EKey::Last)];

		// 게임패드
		KeyInfo mPadKeyInfomations[XUSER_MAX_COUNT][static_cast<size_t>(EPadKey::Last)];
		PadStickInfo mPadStickInfomations[XUSER_MAX_COUNT];
		VibrationInfo mPadVibrationInfomations[XUSER_MAX_COUNT];


	};
}