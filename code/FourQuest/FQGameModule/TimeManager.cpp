
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
{
	QueryPerformanceCounter(&mCurentCount);
	QueryPerformanceCounter(&mPrevCount);
	QueryPerformanceFrequency(&mFrequency);
}

fq::game_module::TimeManager::~TimeManager()
{}

float fq::game_module::TimeManager::Update()
{
	mPrevCount = mCurentCount;

	QueryPerformanceCounter(&mCurentCount);

	mDeltaTime = (mCurentCount.QuadPart - mPrevCount.QuadPart) /
		(double)mFrequency.QuadPart;

	++mUpdateCount;
	mElapsedTime += mDeltaTime;
	if (mElapsedTime >= 1.)
	{
		mElapsedTime -= 1.;
		mFPS = mUpdateCount;
		mUpdateCount = 0;
	}

	// 1�����ӿ� 1.f / 30.f �ʸ� �Ѿ� ���ٸ� �ִ� 1.f / 30.f�� ����
	if (mDeltaTime >= 1.f / 30.f)
		mDeltaTime = 1.f / 30.f;

	return static_cast<float>(mDeltaTime * mTimeScale);
}

float fq::game_module::TimeManager::GetDeltaTime() const
{
	return mDeltaTime;
}
