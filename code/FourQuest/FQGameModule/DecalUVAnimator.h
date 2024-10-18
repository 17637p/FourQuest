#pragma once
#include "Component.h"
#include "EAnimationMode.h"
#include "../FQCommon/IFQRenderResource.h"

namespace fq::game_module
{
	/// <summary>
	/// UV 애니메이션을 관리하는 컴포넌트
	/// </summary>
	/// 
	struct DecalGeoKeyframe
	{
		float TimePos;
		DirectX::SimpleMath::Vector3 Translation = { 0.f, 0.f, 0.f };
		DirectX::SimpleMath::Vector3 Scale = { 1.f, 1.f, 1.f };
		float Rotation = 0.f;
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

		const DecalGeoKeyframe& GetDecalKeyframe() { updateKeyframe();  return mDecalKeyframe; }
		const DecalUVKeyframe& GetDecalUVKeyframe() { updateUVKeyframe(); return mDecalUVKeyframe; }

	private:
		entt::meta_handle GetHandle() override { return *this; }

		void updateKeyframe();
		void updateUVKeyframe();

	private:
		bool mbIsLooping;
		bool mbIsUpdate; // 델타타임 누적 여부
		float mPlaySpeed; // 재생속도
		float mDuration;
		float mAccumulationTime;

		std::vector<DecalGeoKeyframe> mDecalKeyframes;
		std::vector<DecalUVKeyframe> mDecalUVKeyframes;

		DecalGeoKeyframe mDecalKeyframe;
		DecalUVKeyframe mDecalUVKeyframe;

		friend void RegisterMetaData();
	};
}

