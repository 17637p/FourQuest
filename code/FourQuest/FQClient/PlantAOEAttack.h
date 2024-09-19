#pragma once

#include "../FQGameModule/Component.h"
#include "../FQGameModule/PrefabResource.h"

#include "AttackDefine.h"

namespace fq::client
{
	class PlantAOEAttack : public fq::game_module::Component
	{
	public:
		PlantAOEAttack();
		~PlantAOEAttack();

		/// <summary>
		/// 목표 지점의 거리 값에 따라서 도착하는 시간을 계산해주는 함수 및 데칼 표시
		/// </summary>
		void DetermineArrivalTime();

		DirectX::SimpleMath::Vector3 GetStartPosition() const { return mStartPosition; }
		void SetStartPosition(const DirectX::SimpleMath::Vector3& startPosition) { mStartPosition = startPosition; }
		DirectX::SimpleMath::Vector3 GetTargetPosition() const { return mTargetPosition; }
		void SetTargetPosition(const DirectX::SimpleMath::Vector3& targetVector) { mTargetPosition = targetVector; }
		float GetMaxRange() const { return mMaxRange; }
		void SetmaxRange(float maxRange) { mMaxRange = maxRange; }
		float GetMinArrivalTime() const { return mMinArrivalTime; }
		void SetMinArrivalTime(float minArrivalTime) { mMinArrivalTime = minArrivalTime; }
		float GetMaxArrivalTime() const { return mMaxArrivalTime; }
		void SetMaxArrivalTime(float maxArrivalTime) { mMaxArrivalTime = maxArrivalTime; }

	private:
		/// <summary>
		/// 포물선 공격의 위치 업데이트 및 도착시 폭발
		/// </summary>
		void OnUpdate(float dt) override;

		/// <summary>
		/// 플레이어와 부딪히면 독 장판 컴포넌트 생성 및 삭제
		/// </summary>
		virtual void OnTriggerEnter(const fq::game_module::Collision& collision) override;
		virtual void OnTriggerExit(const fq::game_module::Collision& collision) override;

		/// <summary>
		/// 초기 Y축 속도를 계산해주는 함수
		/// </summary>
		void determineYVelocity();

		entt::meta_handle GetHandle() override { return *this; }
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;


	private:
		game_module::PrefabResource mAOEAttackPoolEffectPrefeb;
		game_module::PrefabResource mAOEAttackSplashEffectPrefeb;

		DirectX::SimpleMath::Vector3 mStartPosition;
		DirectX::SimpleMath::Vector3 mTargetPosition;
		float mYPosition;
		float mMaxRange;
		float mMinArrivalTime;
		float mMaxArrivalTime;
		float mDestroyTime;
		float mPoisonDamage;
		float mPoisonTurm;

		float mArrivalTime;
		float mInitialYVelocity;
		float mYVelocity;

		float mDurationTime;
		float mDestroyDurationTime;

		bool mbIsCreateEffect;

		friend void RegisterMetaData();
	};

}