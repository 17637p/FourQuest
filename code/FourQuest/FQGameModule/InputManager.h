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

		// Ű����

		// �����е�

	private:
		void updateMouse();

	private:
		HWND mHWND;

		// ���콺 
		POINT mCurrentMousePosition;
		POINT mPrevMousePosition;
		POINT mDeltaMousePosition;

		// Ű���� 

		// �����е�
	};
}