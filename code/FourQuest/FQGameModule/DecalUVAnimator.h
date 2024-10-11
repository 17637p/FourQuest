#pragma once
#include "Component.h"
#include "EAnimationMode.h"
#include "../FQCommon/IFQRenderResource.h"

namespace fq::game_module
{
	class Decal;
	/// <summary>
	/// UV 애니메이션을 관리하는 컴포넌트
	/// </summary>
	/// 

	struct DecalKeyframe
	{
		float TimePos;
		DirectX::SimpleMath::Vector3 Translation = { 0.f, 0.f, 0.f };
		DirectX::SimpleMath::Vector3 Rotation{ 0.f, 0.f, 0.f };
		DirectX::SimpleMath::Vector3 Scale = { 1.f, 1.f, 1.f };
		DirectX::SimpleMath::Quaternion RotationQuaternion;
	};

	struct DecalUVKeyframe
	{
		float TimePos;
		DirectX::SimpleMath::Vector2 Translation = { 0.f, 0.f };
		DirectX::SimpleMath::Vector2 Scale = { 1.f, 1.f };
		float Rotation;
	};

	class DecalUVAnimator : public Component
	{
		using ControllerPath = std::string;
		using ParameterID = unsigned int;

	public:
		DecalUVAnimator();
		~DecalUVAnimator() = default;

		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

		void OnStart() override;
		void OnUpdate(float dt) override;

		const DirectX::SimpleMath::Matrix& GetAdditiveTransform() { return mAdditiveTransform; }

	private:
		entt::meta_handle GetHandle() override { return *this; }

	private:
		Decal* mDecalOrNull;
		bool mbIsLooping;
		bool mbIsUpdate; // 델타타임 누적 여부
		float mPlaySpeed; // 재생속도
		float mDuration;
		float mAccumulationTime;

		std::vector<DecalKeyframe> mKeyframes;
		std::vector<DecalUVKeyframe> mUVKeyframes;

		DirectX::SimpleMath::Matrix mAdditiveTransform;

		friend void RegisterMetaData();
	};
}

