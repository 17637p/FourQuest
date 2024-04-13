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
		
		Component* Clone(Component* clone /* = nullptr */)const override
		{
			Tmp* attackUI = static_cast<Tmp*>(clone);

			if (attackUI == nullptr) // 새로 생성해서 복사본을 준다
			{
				attackUI = new Tmp(*this);
			}
			else // clone에 데이터를 복사한다.
			{
				// 기본 대입 연산자 호출한다.
				*attackUI = *this;
			}

			return attackUI;
		}

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
		.base<Component>()
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

	GameObject object;

	GameObject obj;
	obj.GetComponent<Transform>()->SetParent(object.GetComponent<Transform>());
	obj.SetName("child");

	obj.GetComponent<Transform>()->SetPosition({ 10.f,20.f,100.f });

	object.AddComponent<fq::game_module::Tmp>();

	om.SavePrefab(&object, fq::path::GetResourcePath());

	auto d22 = om.LoadPrefab("C:\\Users\\PC\\Desktop\\Project\\code\\FourQuest\\FQGameEngineDemo\\resource\\GameObject.json");



	return 0;
}