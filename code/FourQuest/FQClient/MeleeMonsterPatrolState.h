#pragma once

#include "../FQGameModule/IStateBehaviour.h"


namespace fq::client
{
	/// <summary>
	/// �������� ���� ����
	/// </summary>
	class MeleeMonsterPatrolState :public fq::game_module::IStateBehaviour
	{
	public:
		MeleeMonsterPatrolState();
		~MeleeMonsterPatrolState();

	private:
		std::shared_ptr<IStateBehaviour> Clone() override;
		entt::meta_handle GetHandle() override { return *this; }

	};


}