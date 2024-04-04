#include <iostream>

#include "../FQReflect/FQReflect.h"
#include "../FQGameModule/GameModule.h"

using namespace fq::game_module;

int main()
{
	fq::game_module::GameObject g;

	std::cout << g.HasComponent<Transform>();

	g.AddComponent<Transform>();

	std::cout << g.HasComponent<Transform>();

	return 0;
}