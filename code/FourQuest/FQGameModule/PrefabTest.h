#pragma once
#include "Component.h"
#include "PrefabResource.h"

namespace fq::game_module
{
	class PrefabTest :public Component
	{
	public:
		PrefabTest();
		~PrefabTest();

		/// <summary>
		/// 복사본을 할당해서 반환합니다 
		/// </summary>
		Component* Clone(Component* clone /* = nullptr */)const override;


	private:
		PrefabResource mFireObject;
	};

}