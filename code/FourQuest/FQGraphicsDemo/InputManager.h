#pragma once
/// ���콺 ó���� ���� HWND�� ������ include �� �ؾ��Ѵ�.
#include <windows.h>

#include "Singleton.h"

/// <summary>
/// Ű���� ����
/// </summary>
enum class KeyState
{
	None,	// ���� ������x, ���� ������ x
	Down,	// ���� ������x, ���� ������ o
	Press,	// ���� ������o, ���� ������ o
	Up		// ���� ������o, ���� ������ x
};

/// <summary>
/// ���콺, Ű������ �Է� ó���� ����Ѵ�.
/// 
/// �̱������� �����. (�̱��� ���ø��� ��ӹ޴´�.)
/// 2023.07.24 ������
/// </summary>
class InputManager : public Singleton<InputManager>
{
public:
	void Init(HWND hwnd);
	void Update();

	// Ű����, ���콺 �Ѵ� ó���� �����ϴ�. ���� Ű�� �̹� Mouse�� ���ԵǾ� �ִ�.
	bool IsGetKeyDown(int key) const { return mKey[key] == KeyState::Down; }
	bool IsGetKey(int key) const { return mKey[key] == KeyState::Press; }
	bool IsGetKeyUp(int key) const { return mKey[key] == KeyState::Up; }

	bool IsAnyKeyDown() const { return mIsKeyDown; }
	bool IsAnyKey() const { return mIsKey; } 
	bool IsAnyKeyUp() const { return mIsKeyUp; }

	// ī�޶� �ϴ� ���� ���ϰ� �����. ���� ī�޶� �߰��Ǹ� ���� �ʿ��Ҽ��� ����!
	// �ϴ� hwnd�� �ϳ���� �����ϰ� �����. ���� �þ�ٸ� �����ؾ� ��.
	POINT GetMousePosition();
	POINT GetDeltaPosition();

	void SetWheelPower(int power) { mWheelPower = power; }
	int GetWheelPower() const { return mWheelPower; }

private:
	// �� ���� Ŭ������� ���� �����Ϸ��� �����ڸ� ���ܾ� �Ѵ�.
	InputManager() : mHwnd(0), mKey{}, mWheelPower(0), mIsKeyDown(false), mIsKey(0), mIsKeyUp(0), mPreMousePosition(), mMousePosition() {}
	~InputManager() {}

private:
	HWND mHwnd;
	int mWheelPower;

	bool mIsKeyDown;
	bool mIsKey;
	bool mIsKeyUp;

	POINT mPreMousePosition;
	POINT mMousePosition;

	KeyState mKey[0xFE /*VK_OEM_CLEAR*/ + 1];	// 255, https://learn.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes ��� ���� Ű�� ������ �� �ְ� ũ�⸦ ���Ѵ�.

	friend class Singleton<InputManager>;
};