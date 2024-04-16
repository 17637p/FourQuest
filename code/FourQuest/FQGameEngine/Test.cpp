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

		.data<&Test::fVal>("fVal"_hs)
		.prop(fq::reflect::prop::name, "fVal")

		.data<&Test::dVal>("dVal"_hs)
		.prop(fq::reflect::prop::name, "dVal")

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

		.base<fq::game_module::Component>();
}

fq::game_engine::Test::Test()
{
}

fq::game_module::Component* fq::game_engine::Test::Clone(fq::game_module::Component* clone) const
{
	Test* cloneTransform = static_cast<Test*>(clone);

	if (cloneTransform == nullptr) // 새로 생성해서 복사본을 준다
	{
		cloneTransform = new Test(*this);
	}
	else // clone에 데이터를 복사한다.
	{
		// 기본 대입 연산자 호출한다.
		*cloneTransform = *this;
	}
	return cloneTransform;
}

entt::meta_handle fq::game_engine::Test::GetHandle()
{
	return *this;
}

