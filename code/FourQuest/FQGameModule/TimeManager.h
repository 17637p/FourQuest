#pragma once

#include <windows.h>

namespace fq::game_module
{
	class TimeManager
	{
	public:
		TimeManager();
		~TimeManager();

		float Update();


		/// <summary>
		/// FPS�� ��ȯ�մϴ�
		/// </summary>
		/// <returns>FPS</returns>
		unsigned int GetFPS()const { return mFPS; }

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