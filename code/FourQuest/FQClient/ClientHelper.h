#pragma once

#include <random>

namespace fq::client::helper
{
	/// <summary>
	/// ���� ������
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