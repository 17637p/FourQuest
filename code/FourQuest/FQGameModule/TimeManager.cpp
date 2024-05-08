
#include "TimeManager.h"


fq::game_module::TimeManager::TimeManager()
	:mDeltaTime(0.)
	,mTimeScale(1.)
	,mFrequency()
	,mPrevCount()
	,mCurentCount()
	,mElapsedTime(0.f)
	,mFPS(0)
	,mUpdateCount(0)
{}

fq::game_module::TimeManager::~TimeManager()
{}

float fq::game_module::TimeManager::Update()
{
	QueryPerformanceCounter(&mCurentCount);
	QueryPerformanceFrequency(&mFrequency);

	mDeltaTime = (mCurentCount.QuadPart - mPrevCount.QuadPart) /
		(double)mFrequency.QuadPart;

	// 초반에 로딩창에서 dt가 커지는것을 방지
	constexpr double minDeltaTime = 1. / 30.;
	mDeltaTime = min(mDeltaTime, minDeltaTime);

	mPrevCount = mCurentCount;

	++mUpdateCount;
	mElapsedTime += mDeltaTime;
	if (mElapsedTime >= 1.)
	{
		mElapsedTime -= 1.;
		mFPS = mUpdateCount;
		mUpdateCount = 0;
	}

	return static_cast<float>(mDeltaTime * mTimeScale);
}
