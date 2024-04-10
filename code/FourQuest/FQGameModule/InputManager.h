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

		/// <summary>
		/// �̹��������� Key���¸� ��ȯ�մϴ�
		/// </summary>
		/// <param name="key">Key</param>
		/// <returns>�̹������� ����</returns>
		KeyState GetKeyState(Key key)const;

		/// <summary>
		/// �̹� ���������� Key�� KeyState�� ��ġ�ϴ��� Ȯ���մϴ�.
		/// </summary>
		/// <param name="key">Key</param>
		/// <param name="state">KetState</param>
		/// <returns>��ġ�ϸ� true, �׷��������� false</returns>
		bool IsKeyState(Key key, KeyState state) const;

		// �����е�

	private:
		void updateMouse();
		void updateKeybord();
		void updateGamePad();

	private:
		static const int MatchVK[static_cast<size_t>(Key::Last)];
		
		HWND mHWND;

		// ���콺 
		POINT mCurrentMousePosition;
		POINT mPrevMousePosition;
		POINT mDeltaMousePosition;

		// Ű���� 
		KeyInfo mKeyInfomations[static_cast<size_t>(Key::Last)];


		// �����е�
	};
}