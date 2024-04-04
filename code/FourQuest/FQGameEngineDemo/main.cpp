#include <iostream>

#include "../FQReflect/FQReflect.h"
#include "../FQGameModule/GameModule.h"

using namespace fq::game_module;

int main()
{

	fq::game_module::Scene s;

	Scene sc;

	sc.ViewObjects([](GameObject& object)
		{
			std::cout << "hi\n";
		});

	sc.GetComponentView<Transform>();

	sc.ViewComponents<Transform>([](GameObject& object, Transform& transform)
		{
		});


	return 0;
}