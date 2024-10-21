#pragma once
#include "Component.h"
#include "../FQCommon/IFQRenderResource.h"

namespace fq::game_module
{
	struct BlendUVKeyframe
	{
		float TimePos;
		DirectX::SimpleMath::Vector2 Translation = { 0.f, 0.f };
		DirectX::SimpleMath::Vector2 Scale = { 1.f, 1.f };
	};

	class BlendUVAnimator : public Component
	{
		using ControllerPath = std::string;
		using ParameterID = unsigned int;

	public:
		BlendUVAnimator();
		~BlendUVAnimator() = default;

		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

		void OnStart() override;
		void OnUpdate(float dt) override;

		const BlendUVKeyframe& GetUVKeyframe() { updateKeyframe(); return mKeyframe; }

	private:
		entt::meta_handle GetHandle() override { return *this; }

		void updateKeyframe();

	private:
		bool mbIsLooping;
		bool mbIsUpdate; // 델타타임 누적 여부
		float mPlaySpeed; // 재생속도
		float mDuration;
		float mAccumulationTime;

		std::vector<BlendUVKeyframe> mKeyframes;
		BlendUVKeyframe mKeyframe;

		friend void RegisterMetaData();
	};
}

