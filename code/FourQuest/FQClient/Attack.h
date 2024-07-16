#pragma once
#include "../FQGameModule/GameModule.h"
#include "../FQGameModule/GameModuleEnum.h"

namespace fq::client
{
	// 사용하려면 공격력을 설정하고
	// 콜라이더 범위를 설정하고
	// 만들 때 위치도 설정한다음 
	// Attack Object에 Tag를 입력하면 됩니다
	// Attacker 도 Attack을 소환하는 오브젝트를 넣어주세요~
	class Attack : public fq::game_module::Component
	{

	public:
		Attack();
		~Attack();

		/// <summary>
		/// 복사본을 반환합니다 
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
		// 공경 방향이 없는 경우
		inline static DirectX::SimpleMath::Vector3 NoDirection = DirectX::SimpleMath::Vector3::Zero;

	private:
		// 공격력
		float mAttackPower;
		float mAttackTime;
		float mElapsedTime;

		// 공격횟수가 무한한지 
		bool mbIsInfinite;
		unsigned int mRemainingAttackCount;

		DirectX::SimpleMath::Vector3 mAttackDirection;

		fq::game_module::GameObject* mAttacker;

		friend void RegisterMetaData();
	};
}

