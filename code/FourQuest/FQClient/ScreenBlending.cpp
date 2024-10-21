#include "ScreenBlending.h"
#include "../FQGameModule/PostProcessing.h"
#include "../FQGameModule/Scene.h"
#include "../FQCommon/StringUtil.h"

std::shared_ptr<fq::game_module::Component> fq::client::ScreenBlending::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	std::shared_ptr<ScreenBlending> cloneMonster = std::dynamic_pointer_cast<ScreenBlending>(clone);

	if (cloneMonster == nullptr) // ���� �����ؼ� ���纻�� �ش�
	{
		cloneMonster = game_module::ObjectPool::GetInstance()->Assign<ScreenBlending>(*this);
	}
	else // clone�� �����͸� �����Ѵ�.
	{
		// �⺻ ���� ������ ȣ���Ѵ�.
		*cloneMonster = *this;
	}

	return cloneMonster;
}

void fq::client::ScreenBlending::OnStart()
{
	mPostProcessingOrNull = GetScene()->GetObjectByName("PostProcessing").get();
	mElapsed = 0.f;

	mWTexturePath = common::StringUtil::ToWide(mTexturePath);
}

void fq::client::ScreenBlending::OnUpdate(float dt)
{
	if (mPostProcessingOrNull == nullptr)
	{
		return;
	}
	if (mDuration == mElapsed)
	{
		applyTextureBlending(false);
		GetScene()->DestroyGameObject(GetGameObject());
		return;
	}
	if (mPostProcessingOrNull->IsDestroyed())
	{
		mPostProcessingOrNull == nullptr;
		return;
	}

	mElapsed += dt;
	mElapsed = std::min<float>(mElapsed, mDuration);

	applyTextureBlending(true);
}

void fq::client::ScreenBlending::applyTextureBlending(bool bIsActive)
{
	auto postProcessingOrNull = mPostProcessingOrNull->GetComponent<fq::game_module::PostProcessing>();

	if (postProcessingOrNull == nullptr)
	{
		return;
	}

	auto info = postProcessingOrNull->GetPostProcessingInfo();
	info.bUseBlendTexture = bIsActive;

	if (bIsActive)
	{
		info.BlendTextureName = mWTexturePath;
		float ratio = mElapsed / mDuration;
		info.BlendColor = DirectX::SimpleMath::Color::Lerp(mStartBlendColor, mEndBlendColor, ratio);
	}

	postProcessingOrNull->SetPostProcessingInfo(info);
}
