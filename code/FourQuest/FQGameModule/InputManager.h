#pragma once

#include <windows.h>

#include "InputEnum.h"

namespace fq::game_module
{
	class InputManager
	{
		struct KeyInfo
		{
			EKeyState state = EKeyState::None;
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

		/// <summary>
		/// ���콺 �� ��ȭ���� ��ȯ�մϴ� 
		/// </summary>
		short GetDeltaMouseWheel() const { return mDeltaMouseWheel; }

		// Ű����
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


		// �����е�

	private:
		void updateMouse();
		void updateKeybord();
		void updateGamePad();

	public:
		static short DeltaMouseWheel;
	
	private:
		static const int MatchVK[static_cast<size_t>(EKey::Last)];
		
		HWND mHWND;

		// ���콺 
		POINT mCurrentMousePosition;
		POINT mPrevMousePosition;
		POINT mDeltaMousePosition;
		short mDeltaMouseWheel;

		// Ű���� 
		KeyInfo mKeyInfomations[static_cast<size_t>(EKey::Last)];

		// �����е�
	};
}