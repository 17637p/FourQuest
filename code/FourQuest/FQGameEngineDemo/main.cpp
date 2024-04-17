#include <iostream>

#include "../FQReflect/FQReflect.h"

enum class E
{
	A,
	B,
	C
};

struct sample
{
	E e; 
};

FQ_REGISTRATION
{
	using namespace entt::literals;

	entt::meta<E>()
	.data<E::A>("A"_hs)
	.data<E::B>("B"_hs)
	.data<E::C>("C"_hs);


	entt::meta<sample>()
		.data<&sample::e>("e"_hs);
}

int main()
{
	sample d;

	

	for (auto [id, data] : entt::resolve<sample>().data())
	{
		bool is = data.set(d,  data.get(d));
		std::cout << is;
	}


	return 0;
}