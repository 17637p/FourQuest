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

	
		float GetAttackPower() const { return mAttackPower; }
		void SetAttackPower(float val) { mAttackPower = val; }

		fq::game_module::GameObject* GetAttacker() const { return mAttacker; }
		void SetAttacker(fq::game_module::GameObject* val) { mAttacker = val; }

		DirectX::SimpleMath::Vector3 GetAttackDirection() const { return mAttackDirection; }
		void SetAttackDirection(DirectX::SimpleMath::Vector3 val) { mAttackDirection = val; }

	private:
		entt::meta_handle GetHandle() override { return *this; }
		void OnUpdate(float dt) override;
		void OnTriggerEnter(const game_module::Collision& collision)override;
	public:
		// ���� ������ ���� ���
		inline static DirectX::SimpleMath::Vector3 NoDirection = DirectX::SimpleMath::Vector3::Zero;

	private:
		// ���ݷ�
		float mAttackPower;
		float mAttackTime;
		float mElapsedTime;

		// ����Ƚ���� �������� 
		bool mbIsInfinite;
		unsigned int mRemainingAttackCount;

		DirectX::SimpleMath::Vector3 mAttackDirection;

		fq::game_module::GameObject* mAttacker;

		friend void RegisterMetaData();
	};
}

