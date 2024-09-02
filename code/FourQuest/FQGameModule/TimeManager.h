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

		/// <summary>
		/// �ð� ��ȭ���� ��ȯ�մϴ�.
		/// </summary>
		float GetDeltaTime()const;

		/// <summary>
		/// ������ �ð������� ��ȯ�մϴ�
		/// </summary>
		/// <returns></returns>
		double GetTimeScale() const { return mTimeScale; }
		
		/// <summary>
		/// ������ �ð� ������ �����մϴ�
		/// </summary>
		/// <param name="val">�ð�����</param>
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