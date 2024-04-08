#pragma once

#include <windows.h>

namespace fq::game_module
{
	class InputManager
	{
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

		// 게임패드

	private:
		void updateMouse();

	private:
		HWND mHWND;

		// 마우스 
		POINT mCurrentMousePosition;
		POINT mPrevMousePosition;
		POINT mDeltaMousePosition;

		// 키보드 

		// 게임패드
	};
}