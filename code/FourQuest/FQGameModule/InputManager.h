#pragma once

#include <windows.h>

#include "InputEnum.h"

namespace fq::game_module
{
	class InputManager
	{
		struct KeyInfo
		{
			KeyState state = KeyState::None;
			bool prevPush = false;
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

		// 키보드

		/// <summary>
		/// 이번프레임의 Key상태를 반환합니다
		/// </summary>
		/// <param name="key">Key</param>
		/// <returns>이번프레임 상태</returns>
		KeyState GetKeyState(Key key)const;

		/// <summary>
		/// 이번 프레임이의 Key가 KeyState와 일치하는지 확인합니다.
		/// </summary>
		/// <param name="key">Key</param>
		/// <param name="state">KetState</param>
		/// <returns>일치하면 true, 그렇지않으면 false</returns>
		bool IsKeyState(Key key, KeyState state) const;

		// 게임패드

	private:
		void updateMouse();
		void updateKeybord();
		void updateGamePad();

	private:
		static const int MatchVK[static_cast<size_t>(Key::Last)];
		
		HWND mHWND;

		// 마우스 
		POINT mCurrentMousePosition;
		POINT mPrevMousePosition;
		POINT mDeltaMousePosition;

		// 키보드 
		KeyInfo mKeyInfomations[static_cast<size_t>(Key::Last)];


		// 게임패드
	};
}