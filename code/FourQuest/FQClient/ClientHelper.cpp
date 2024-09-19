#include "ClientHelper.h"

float fq::client::helper::RandomGenerator::GetRandomNumber(float min, float max)
{
	std::uniform_real_distribution<float> dist(min, max);
	return dist(mDevice);
}

int fq::client::helper::RandomGenerator::GetRandomNumber(int min, int max)
{
	std::uniform_int_distribution<int> dist(min, max);
	return dist(mDevice);
}

fq::client::helper::RandomGenerator::RandomGenerator()
{
	std::random_device rd;
	mDevice = std::mt19937(rd());
}

fq::client::helper::RandomGenerator::~RandomGenerator()
{

}
