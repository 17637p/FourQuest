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
	// 이번 프레임 Count 측정
	LARGE_INTEGER currentCount;
	QueryPerformanceCounter(&currentCount);

	// 이번 프레임 빈도 측정
	QueryPerformanceFrequency(&mFrequency);

	mDeltaTime = (currentCount.QuadPart - mPreviousCount.QuadPart) / (float)mFrequency.QuadPart;
	mPreviousCount = currentCount;

	// 1초 동안 몇 번 인지 세기 위해 프레임 수를 1 올리고 시간 누적
	mAccumulationCount++;
	mAccumulationTime += mDeltaTime;

	// 1초가 되면 값 초기화
	if (mAccumulationTime >= 1.f)
	{
		mFps = (int)(mAccumulationCount / mAccumulationTime);

		mAccumulationTime = 0.f;
		mAccumulationCount = 0;
	}
}
