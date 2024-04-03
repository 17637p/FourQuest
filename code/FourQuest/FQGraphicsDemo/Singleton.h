#pragma once
template <typename className>
class Singleton
{
public:
	static className& GetInstance();
};

template <typename className>
className& Singleton<className>::GetInstance()
{
	static className instance;
	return instance;
}