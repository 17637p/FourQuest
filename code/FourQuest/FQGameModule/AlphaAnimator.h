#pragma once
#include "Component.h"
#include "../FQCommon/IFQRenderResource.h"

namespace fq::game_module
{
	/// <summary>
	/// 알파 애니메이션을 관리하는 컴포넌트
	/// 이펙트 처리 시 에디터에서 데이터 수정할 수 있게 하기 위해 만듦
	/// </summary>
	/// 
	enum class EAlphaAnimationMode
	{
		Increase,
		Decrease,
	};

	struct AlphaAnimatorInfo
	{
		float Alpha = 1.f;
		float TimePos = 0.f;

		bool bIsRecursive = true; // 반복 재생 여부
		bool bIsUpdate = true; // 델타타임 누적 여부

		float Duration = 1.f; // 총 재생시간
		float Speed = 1.f; // 델타타임에 비례해서 증가될 알파값
		float DelayTime = 0.f; 

		float InitAlpha = 1.f;
		float MaxAlpha = 1.f; 
		float MinAlpha = 0.f; 
		EAlphaAnimationMode AlphaAnimationMode = EAlphaAnimationMode::Decrease;
	};

	class AlphaAnimator : public Component
	{
		using ControllerPath = std::string;
		using ParameterID = unsigned int;

	public:
		AlphaAnimator() = default;
		~AlphaAnimator() = default;

		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

		void UpdateTimePos(float dt);

		void SetAlphaAnimatorInfo(const AlphaAnimatorInfo& alphaAnimatorInfo) { mInfo = alphaAnimatorInfo; }
		const AlphaAnimatorInfo& GetAlphaAnimatorInfo() const { return mInfo; }

	private:
		entt::meta_handle GetHandle() override { return *this; }

	private:
		AlphaAnimatorInfo mInfo;
	};
}

