#pragma once
#include "Component.h"
#include "EAnimationMode.h"
#include "../FQCommon/IFQRenderResource.h"

namespace fq::game_module
{
	/// <summary>
	/// ���� �ִϸ��̼��� �����ϴ� ������Ʈ
	/// ����Ʈ ó�� �� �����Ϳ��� ������ ������ �� �ְ� �ϱ� ���� ����
	/// </summary>
	/// 
	struct AlphaAnimatorInfo
	{
		bool bIsUsed = true;

		float Alpha = 1.f;
		float TimePos = 0.f;

		bool bIsLooping = true; // �ݺ� ��� ����
		bool bIsUpdate = true; // ��ŸŸ�� ���� ����

		float Duration = 1.f; // �� ����ð�
		float Speed = 1.f; // ��ŸŸ�ӿ� ����ؼ� ������ ���İ�
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

		bool bIsLooping = true; // �ݺ� ��� ����
		bool bIsUpdate = true; // ��ŸŸ�� ���� ����

		float Duration = 1.f; // �� ����ð�
		float Speed = 1.f; // ��ŸŸ�ӿ� ����ؼ� ������ ���İ�
		float DelayTime = 0.f;

		float InitDissolveCutoff = 0.f;
		float MaxDissolveCutoff = 1.1f;
		float MinDissolveCutoff = 0.f;
		EAnimationMode AnimationMode = EAnimationMode::Increase;
	};

	// �ڽ� ���� �̾ ó��
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

