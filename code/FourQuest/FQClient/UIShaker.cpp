#include "UIShaker.h"
#include "../FQGameModule/Transform.h"
#include "../FQGameModule/ImageUI.h"

std::shared_ptr<fq::game_module::Component> fq::client::UIShaker::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	std::shared_ptr<UIShaker> cloneMonster = std::dynamic_pointer_cast<UIShaker>(clone);

	if (cloneMonster == nullptr) // 새로 생성해서 복사본을 준다
	{
		cloneMonster = game_module::ObjectPool::GetInstance()->Assign<UIShaker>(*this);
	}
	else // clone에 데이터를 복사한다.
	{
		// 기본 대입 연산자 호출한다.
		*cloneMonster = *this;
	}

	return cloneMonster;
}

void fq::client::UIShaker::OnStart()
{
	mCount = 0;
	mElapsed = 0.f;

	auto imageUI = GetGameObject()->GetComponent<fq::game_module::ImageUI>();

	if (imageUI != nullptr)
	{
		mUIInfos = imageUI->GetUIInfomations();
	}
}

void fq::client::UIShaker::OnUpdate(float dt)
{
	auto imageUI = GetGameObject()->GetComponent<fq::game_module::ImageUI>();

	if (imageUI == nullptr)
	{
		return;
	}
	if (mUIInfos.empty())
	{
		mUIInfos = imageUI->GetUIInfomations();
		return;
	}
	if (mCount == 0)
	{
		mElapsed = 0.f;
		imageUI->SetUIInfomations(mUIInfos);
		return;
	}

	// 델타타임 누적
	mElapsed += dt;

	if (mDuration < mElapsed)
	{
		--mCount;
		mElapsed -= mDuration;
	}

	// 보간 처리
	float ratio = mElapsed / mDuration;
	float period = (sin(ratio) + 1) * 0.5;
	DirectX::SimpleMath::Vector2 curOffset = DirectX::SimpleMath::Vector2::Lerp(mStartOffset, mEndOffset, period);

	auto uiInfos = imageUI->GetUIInfomations();

	for (size_t i = 0; i < uiInfos.size(); ++i)
	{
		uiInfos[i].StartX = mUIInfos[i].StartX + curOffset.x;
		uiInfos[i].StartY = mUIInfos[i].StartY + curOffset.y;
	}

	imageUI->SetUIInfomations(uiInfos);
}
