#pragma once
#include "Component.h"
#include "../FQCommon/IFQRenderResource.h"

namespace fq::game_module
{
	/// <summary>
	/// ���� �ִϸ��̼��� �����ϴ� ������Ʈ
	/// ����Ʈ ó�� �� �����Ϳ��� ������ ������ �� �ְ� �ϱ� ���� ����
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

		bool bIsRecursive = true; // �ݺ� ��� ����
		bool bIsUpdate = true; // ��ŸŸ�� ���� ����

		float Duration = 1.f; // �� ����ð�
		float Speed = 1.f; // ��ŸŸ�ӿ� ����ؼ� ������ ���İ�
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

