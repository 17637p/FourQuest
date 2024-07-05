#pragma once

#include "../FQGameModule/IStateBehaviour.h"


namespace fq::client
{
	/// <summary>
	/// �������Ͱ� �ƹ��͵� ���� �ʴ� ���� 
	/// </summary>
	class MeleeMonsterIdleState :public fq::game_module::IStateBehaviour
	{
	public:
		MeleeMonsterIdleState();
		~MeleeMonsterIdleState();

	private:
		std::shared_ptr<IStateBehaviour> Clone() override;
		entt::meta_handle GetHandle() override { return *this; }

	};


}