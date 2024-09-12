#pragma once

#include "../FQGameModule/Component.h"
#include "AttackDefine.h"

namespace fq::client
{
	class ArcAttack : public fq::game_module::Component
	{
	public:
		ArcAttack();
		~ArcAttack();

		/// <summary>
		/// ��ǥ ������ �Ÿ� ���� ���� �����ϴ� �ð��� ������ִ� �Լ� �� ��Į ǥ��
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
		/// ������ ������ ��ġ ������Ʈ �� ������ ����
		/// </summary>
		void OnUpdate(float dt) override;

		/// <summary>
		/// �ʱ� Y�� �ӵ��� ������ִ� �Լ�
		/// </summary>
		void determineYVelocity();

		entt::meta_handle GetHandle() override { return *this; }
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;


	private:
		game_module::PrefabResource mAOEAttackPrefeb;

		DirectX::SimpleMath::Vector3 mStartPosition;
		DirectX::SimpleMath::Vector3 mTargetPosition;
		float mMaxRange;
		float mMinArrivalTime;
		float mMaxArrivalTime;

		float mArrivalTime;
		float mInitialYVelocity;
		float mYVelocity;

		float mDurationTime;
	};

}