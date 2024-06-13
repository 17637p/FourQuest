#pragma once
#include "../FQGameModule/GameModule.h"
#include "../FQGameModule/GameModuleEnum.h"

namespace fq::client
{
	// ����Ϸ��� ���ݷ��� �����ϰ�
	// �ݶ��̴� ������ �����ϰ�
	// ���� �� ��ġ�� �����Ѵ��� 
	// Attack Object�� Tag�� �Է��ϸ� �˴ϴ�
	// Attacker �� Attack�� ��ȯ�ϴ� ������Ʈ�� �־��ּ���~
	class Attack : public fq::game_module::Component
	{
	public:
		Attack();
		~Attack();

		/// <summary>
		/// ���纻�� ��ȯ�մϴ� 
		/// </summary>
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

		void OnUpdate(float dt) override;

		float GetAttackPower() const { return mAttackPower; }
		void SetAttackPower(float val) { mAttackPower = val; }

		float GetAttackTime() const { return mAttackTime; }
		void SetAttackTime(float val) { mAttackTime = val; }

		fq::game_module::GameObject* GetAttacker() const { return mAttacker; }
		void SetAttacker(fq::game_module::GameObject* val) { mAttacker = val; }

	private:
		entt::meta_handle GetHandle() override { return *this; }

	private:
		// ���ݷ�
		float mAttackPower;
		float mAttackTime;
		float mElapsedTime;

		fq::game_module::GameObject* mAttacker;
	};
}

