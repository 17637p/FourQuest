#include "GameModulePCH.h"
#include "TimeManager.h"


fq::game_module::TimeManager::TimeManager()
	:mDeltaTime(0.)
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

	mDeltaTime = (mCurentCount.QuadPart - mPrevCount.QuadPart) /
		(double)mFrequency.QuadPart;

	// �ʹݿ� �ε�â���� dt�� Ŀ���°��� ����
	constexpr double minDeltaTime = 1. / 30.;
	mDeltaTime = std::min(mDeltaTime, minDeltaTime);

	mPrevCount = mCurentCount;

	++mUpdateCount;
	mElapsedTime += mDeltaTime;
	if (mElapsedTime >= 1.)
	{
		mElapsedTime -= 1.;
		mFPS = mUpdateCount;
		mUpdateCount = 0;
	}

	return static_cast<float>(mDeltaTime);
}
