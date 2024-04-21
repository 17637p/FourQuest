#include "Test.h"

FQ_REGISTRATION
{
	using namespace fq::game_engine;
	using namespace entt::literals;

	entt::meta<fq::game_engine::Test>()
		.type("Test"_hs)
		.prop(fq::reflect::prop::name, "Test")
		
		.data<&Test::iVal>("iVal"_hs)
		.prop(fq::reflect::prop::name, "iVal")
		.prop( fq::reflect::prop::comment , "single랑" )

		.data<&Test::fVal>("fVal"_hs)
		.prop(fq::reflect::prop::name, "fVal")
		.prop(fq::reflect::prop::comment, u8"single랑")

		.data<&Test::dVal>("dVal"_hs)
		.prop(fq::reflect::prop::name, "dVal")
		.prop( fq::reflect::prop::comment , "single" )

		.data<&Test::uiVal>("uiVal"_hs)
		.prop(fq::reflect::prop::name, "uiVal")

		.data<&Test::v2Val>("v2Val"_hs)
		.prop(fq::reflect::prop::name, "v2Val")

		.data<&Test::v3Val>("v3Val"_hs)
		.prop(fq::reflect::prop::name, "v3Val")

		.data<&Test::v4Val>("v4Val"_hs)
		.prop(fq::reflect::prop::name, "v4Val")

		.data<&Test::cVal>("cVal"_hs)
		.prop(fq::reflect::prop::name, "cVal")

		.data<&Test::sVal>("sVal"_hs)
		.prop(fq::reflect::prop::name, "sVal")

		.data<&Test::veciVal>("veciVal"_hs)
		.prop(fq::reflect::prop::name, "veciVal")

		.data<&Test::vecv3Val>("vecv3Val"_hs)
		.prop(fq::reflect::prop::name, "vecv3Val")

		.base<fq::game_module::Component>();

}

fq::game_engine::Test::Test()
{
}

fq::game_module::Component* fq::game_engine::Test::Clone(fq::game_module::Component* clone) const
{
	Test* cloneTransform = static_cast<Test*>(clone);

	if (cloneTransform == nullptr) 
	{
		cloneTransform = new Test(*this);
	}
	else // clone에 데이터를 복사한다.
	{
		// 복사 대입 연산자 호출한다.
		*cloneTransform = *this;
	}
	return cloneTransform;
}

entt::meta_handle fq::game_engine::Test::GetHandle()
{
	return *this;
}

void fq::game_engine::Test::aa()
{
	auto scene =  GetScene();

}

void fq::game_engine::Test::OnUpdate(float dt)
{
	auto transform =  GetComponent<fq::game_module::Transform>();
	
	transform->AddPosition({ 1.f,0.f,1.f });
}

