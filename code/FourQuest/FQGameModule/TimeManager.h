#pragma once

#include <windows.h>

namespace fq::game_module
{
	/// <summary>
	/// 시간 처리를 하는 클래스 
	/// </summary>
	class TimeManager
	{
	public:
		TimeManager();
		~TimeManager();

		/// <summary>
		/// 이번 프레임의 시간을 계산합니다 
		/// </summary>
		/// <returns>Delta Time</returns>
		float Update();

		/// <summary>
		/// FPS를 반환합니다
		/// </summary>
		/// <returns>FPS</returns>
		unsigned int GetFPS()const { return mFPS; }

		/// <summary>
		/// 시간 변화량을 반환합니다.
		/// </summary>
		float GetDeltaTime()const;

		/// <summary>
		/// 게임의 시간비율을 반환합니다
		/// </summary>
		/// <returns></returns>
		double GetTimeScale() const { return mTimeScale; }
		
		/// <summary>
		/// 게임의 시간 비율을 설정합니다
		/// </summary>
		/// <param name="val">시간비율</param>
		void SetTimeScale(double val) { mTimeScale = val; }

	private:
		LARGE_INTEGER mCurentCount;
		LARGE_INTEGER mPrevCount;
		LARGE_INTEGER mFrequency;

		double mTimeScale;
		double mDeltaTime;
		double mElapsedTime;
		unsigned int mUpdateCount;
		unsigned int mFPS;
	};
}