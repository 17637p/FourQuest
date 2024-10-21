#pragma once
#include "Component.h"
#include "EAnimationMode.h"
#include "../FQCommon/IFQRenderResource.h"

namespace fq::game_module
{
	/// <summary>
	/// 알파 애니메이션을 관리하는 컴포넌트
	/// 이펙트 처리 시 에디터에서 데이터 수정할 수 있게 하기 위해 만듦
	/// </summary>
	/// 
	struct AlphaAnimatorInfo
	{
		bool bIsUsed = true;

		float Alpha = 1.f;
		float TimePos = 0.f;

		bool bIsLooping = true; // 반복 재생 여부
		bool bIsUpdate = true; // 델타타임 누적 여부

		float Duration = 1.f; // 총 재생시간
		float Speed = 1.f; // 델타타임에 비례해서 증가될 알파값
		float DelayTime = 0.f;

		float InitAlpha = 1.f;
		float MaxAlpha = 1.f;
		float MinAlpha = 0.f;
		EAnimationMode  AnimationMode = EAnimationMode::Decrease;
	};

	struct DissolveAnimatorInfo
	{
		bool bIsUsed = true;

		float DissolveCutoff = 0.f;
		float TimePos = 0.f;

		bool bIsLooping = true; // 반복 재생 여부
		bool bIsUpdate = true; // 델타타임 누적 여부

		float Duration = 1.f; // 총 재생시간
		float Speed = 1.f; // 델타타임에 비례해서 증가될 알파값
		float DelayTime = 0.f;

		float InitDissolveCutoff = 0.f;
		float MaxDissolveCutoff = 1.1f;
		float MinDissolveCutoff = 0.f;
		EAnimationMode AnimationMode = EAnimationMode::Increase;
	};

	// 자식 계층 이어서 처리
	class MaterialAnimator : public Component
	{
		using ControllerPath = std::string;
		using ParameterID = unsigned int;

	public:
		MaterialAnimator() = default;
		~MaterialAnimator() = default;

		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

		void OnUpdate(float dt);

		void SetAlphaAnimatorInfo(const AlphaAnimatorInfo& alphaAnimatorInfo) { mAlphaAnimatorInfo = alphaAnimatorInfo; }
		const AlphaAnimatorInfo& GetAlphaAnimatorInfo() const { return mAlphaAnimatorInfo; }

		void SetDissolveAnimatorInfo(const DissolveAnimatorInfo& dissolveAnimatorInfo) { mDissovleAnimatorInfo = dissolveAnimatorInfo; }
		const DissolveAnimatorInfo& GetDissolveAnimatorInfo() const { return mDissovleAnimatorInfo; }

	private:
		entt::meta_handle GetHandle() override { return *this; }

		void updateAlpha(float dt);
		void updateDissolve(float dt);

	private:
		AlphaAnimatorInfo mAlphaAnimatorInfo;
		DissolveAnimatorInfo mDissovleAnimatorInfo;
	};
}

