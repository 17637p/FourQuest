#include "TimeManager.h"

float TimeManager::m_timeScale = 1.f;

TimeManager::TimeManager()
	:mDeltaTime(0), mFps(0),
	mAccumulationCount(0), mAccumulationTime(0),
	mPreviousCount({}), mFrequency({})
{

}

TimeManager::~TimeManager()
{

}

void TimeManager::Init()
{
	QueryPerformanceCounter(&mPreviousCount);
}

void TimeManager::Update()
{
	// �̹� ������ Count ����
	LARGE_INTEGER currentCount;
	QueryPerformanceCounter(&currentCount);

	// �̹� ������ �� ����
	QueryPerformanceFrequency(&mFrequency);

	mDeltaTime = (currentCount.QuadPart - mPreviousCount.QuadPart) / (float)mFrequency.QuadPart;
	mPreviousCount = currentCount;

	// 1�� ���� �� �� ���� ���� ���� ������ ���� 1 �ø��� �ð� ����
	mAccumulationCount++;
	mAccumulationTime += mDeltaTime;

	// 1�ʰ� �Ǹ� �� �ʱ�ȭ
	if (mAccumulationTime >= 1.f)
	{
		mFps = (int)(mAccumulationCount / mAccumulationTime);

		mAccumulationTime = 0.f;
		mAccumulationCount = 0;
	}
}
