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
		/// ���纻�� �Ҵ��ؼ� ��ȯ�մϴ� 
		/// </summary>
		Component* Clone(Component* clone /* = nullptr */)const override;


	private:
		PrefabResource mFireObject;
	};

}