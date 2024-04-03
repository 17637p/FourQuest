#pragma once
#include <windows.h>

// #include <chrono> �ð� ���� ���̺귯�� �ð� ���� Ȯ���غ���

/// <summary>
/// DeltaTime�� �����ϱ� ���� Ŭ����
/// 
/// 2023.07.21 ������
/// </summary>
class TimeManager
{
public:
	static float m_timeScale;

	TimeManager();
	~TimeManager();

	void	Init();
	void	Update();

	int		GetFps()		const { return (int)(mFps * m_timeScale); }
	float	GetDeltaTime()	const { return mDeltaTime * m_timeScale; }

private:
	float	mDeltaTime;			// 1 ������ �� �ɸ��� �ð� 
	int		mFps;					// �ʴ� ������ ��

	LARGE_INTEGER mPreviousCount;	// ���� frequency ���� �� �� �����ߴ��� �����ϱ� ���� ����
	LARGE_INTEGER mFrequency;		// 1�ʿ� �� �� ���� �ߴ��� �����ϱ� ���� ����

	int		mAccumulationCount;	// 1�� ���� �� �������� ����Ǿ����� ���� ���� ����
	float	mAccumulationTime;		// 1�ʸ� ���� ���� ����
};