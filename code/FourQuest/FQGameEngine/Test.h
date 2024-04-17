#pragma once

#include "../FQGameModule/GameModule.h"

namespace fq::game_engine
{
	class Test : public  fq::game_module::Component
	{
	public:
		Test();
		~Test(){}

		Component* Clone(fq::game_module::Component* clone /* = nullptr */)const override;

		entt::meta_handle GetHandle() override;

		int iVal = 10;
		float fVal = 20.f;
		double dVal = 30.5;
		unsigned int uiVal= 105;
		DirectX::SimpleMath::Vector2 v2Val = {1.f,2.f};
		DirectX::SimpleMath::Vector3 v3Val ={1.f,2.f,3.f};
		DirectX::SimpleMath::Vector4 v4Val = {1,2,3,4};
		DirectX::SimpleMath::Color cVal = {};
		std::string sVal = {};

		std::vector<int> veciVal;
		std::vector<DirectX::SimpleMath::Vector3> vecv3Val;
		FQ_REGISTRATION_FRIEND
	};
}