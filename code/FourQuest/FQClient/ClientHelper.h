#pragma once

#include <random>

namespace fq::client::helper
{
	/// <summary>
	/// 난수 생성기
	/// </summary>
	class RandomGenerator
	{
	public:
		static RandomGenerator& GetInstance() 
		{
			static RandomGenerator instance;
			return instance;
		}

		float GetRandomNumber(float min, float max);
		int GetRandomNumber(int min, int max);
	
	private:
		RandomGenerator();
		~RandomGenerator();

	private:
		std::mt19937 mDevice;

	};

}