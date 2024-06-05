#pragma once
#include "../FQGameModule/GameModule.h"
#include "../FQGameModule/GameModuleEnum.h"

namespace fq::client
{
	// ����Ϸ��� ���ݷ��� �����ϰ�
	// �ݶ��̴� ������ �����ϰ�
	// ���� �� ��ġ�� �����Ѵ��� 
	// Attack Object�� Tag�� �Է��ϸ� �˴ϴ�
	class Attack : public fq::game_module::Component
	{
	public:
		Attack();
		~Attack();

		float GetAttackPower() const { return mAttackPower; }
		void SetAttackPower(float val) { mAttackPower = val; }

		/// <summary>
		/// ���纻�� ��ȯ�մϴ� 
		/// </summary>
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

	private:
		entt::meta_handle GetHandle() override { return *this; }

	private:
		// ���ݷ�
		float mAttackPower;
	};
}

