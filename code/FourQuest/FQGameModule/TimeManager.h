#pragma once

namespace fq::game_module
{
	class TimeManager
	{
	public:
		TimeManager();
		~TimeManager();

		float Update();

	private:
		LARGE_INTEGER mCurentCount;
		LARGE_INTEGER mPrevCount;
		LARGE_INTEGER mFrequency;

		double mDeltaTime;
		double mElapsedTime;
		unsigned int mUpdateCount;
		unsigned int mFPS;
	};
}