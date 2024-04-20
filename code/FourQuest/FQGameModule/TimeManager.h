#pragma once

#include <windows.h>

namespace fq::game_module
{
	/// <summary>
	/// �ð� ó���� �ϴ� Ŭ���� 
	/// </summary>
	class TimeManager
	{
	public:
		TimeManager();
		~TimeManager();

		/// <summary>
		/// �̹� �������� �ð��� ����մϴ� 
		/// </summary>
		/// <returns>Delta Time</returns>
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