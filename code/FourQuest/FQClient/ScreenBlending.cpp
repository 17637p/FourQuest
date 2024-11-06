#include "ScreenBlending.h"
#include "../FQGameModule/PostProcessing.h"
#include "../FQGameModule/Scene.h"
#include "../FQCommon/StringUtil.h"

std::shared_ptr<fq::game_module::Component> fq::client::ScreenBlending::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	std::shared_ptr<ScreenBlending> cloneMonster = std::dynamic_pointer_cast<ScreenBlending>(clone);

	if (cloneMonster == nullptr) // 새로 생성해서 복사본을 준다
	{
		cloneMonster = game_module::ObjectPool::GetInstance()->Assign<ScreenBlending>(*this);
	}
	else // clone에 데이터를 복사한다.
	{
		// 기본 대입 연산자 호출한다.
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
	mElapsed += dt;

	if (mDelayTime > mElapsed)
	{
		return;
	}
	if (mPostProcessingOrNull == nullptr)
	{
		return;
	}
	if (mPostProcessingOrNull->IsDestroyed())
	{
		mPostProcessingOrNull == nullptr;
		return;
	}

	applyTextureBlending(true);
}

void fq::client::ScreenBlending::OnDestroy()
{
	applyTextureBlending(false);
}

void fq::client::ScreenBlending::applyTextureBlending(bool bIsActive)
{
	if (mPostProcessingOrNull == nullptr)
	{
		return;
	}

	auto postProcessingComp = mPostProcessingOrNull->GetComponent<fq::game_module::PostProcessing>();

	if (postProcessingComp == nullptr)
	{
		return;
	}

	unsigned int index = std::min<unsigned int>(mIndex, fq::graphics::PostProcessingInfo::BLEND_TEXTURE_SIZE - 1);
	auto info = postProcessingComp->GetPostProcessingInfo();
	info.bUseBlendTextures[index] = bIsActive;

	if (bIsActive)
	{
		info.BlendTextureNames[index] = mWTexturePath;
		float ratio = std::clamp<float>((mElapsed - mDelayTime) / mDuration, 0, 1);
		info.BlendTextureColors[index] = DirectX::SimpleMath::Color::Lerp(mStartBlendColor, mEndBlendColor, ratio);
	}

	postProcessingComp->SetPostProcessingInfo(info);
}
