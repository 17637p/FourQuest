#include <iostream>

#include "../FQGameModule/GameModule.h"

namespace fq::game_module
{
	class Tmp : public Component
	{
	public:
		Tmp() {}
		Tmp(int number, float hp)
			:num(number), hp(hp)
		{}

		int num = 50;
		float hp = 100.f;
		DirectX::SimpleMath::Vector3 pos = { 100, 200, 300 };
	};
}

using namespace fq::game_module;

FQ_REGISTRATION
{
	entt::meta<Tmp>()
		.type(entt::hashed_string("Tmp"))
		.data<&Tmp::num>(entt::hashed_string("num"))
		.prop(fq::reflect::tag::name, "num")
		.data<&Tmp::hp>(entt::hashed_string("hp"))
		.prop(fq::reflect::tag::name, "hp")
		.data<&Tmp::pos>(entt::hashed_string("pos"))
		.prop(fq::reflect::tag::name, "pos");

}

int main()
{
	ObjectManager om;
	SimpleMathRelfection d;

	GameObject object;

	object.AddComponent<fq::game_module::Tmp>();

	om.SavePrefab(&object, L"..\\resource");


	return 0;
}