#define NOMINMAX
#include "MonsterHP.h"
#include "../FQGameModule/GameModule.h"
#include "../FQGameModule/Transform.h"
#include "../FQGameModule/Camera.h"
#include "../FQGameModule/ImageUI.h"

#include "UILayerDefine.h"
#include "ClientEvent.h"

std::shared_ptr<fq::game_module::Component> fq::client::MonsterHP::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	auto cloneHpBar = std::dynamic_pointer_cast<MonsterHP>(clone);

	if (cloneHpBar == nullptr) // 새로 생성해서 복사본을 준다소울라이크
	{
		cloneHpBar = game_module::ObjectPool::GetInstance()->Assign<MonsterHP>(*this);
	}
	else // clone에 데이터를 복사한다.
	{
		// 기본 대입 연산자 호출한다.
		*cloneHpBar = *this;
	}

	return cloneHpBar;
}

void fq::client::MonsterHP::DecreaseHp(float ratio)
{
	if (mHpRatio == 0.f) return;

	if (!mbIsVisible)
	{
		SetVisible(true);
	}

	float decreaseHpRatio = mHpRatio - ratio;

	if (decreaseHpRatio >= 0.f)
	{
		mDecreaseRatio += ratio;
		mHpRatio = decreaseHpRatio;
	}
	else
	{
		mDecreaseRatio += mHpRatio;
		mHpRatio = 0.f;
	}

	mDecreaseTime = 0.f;
}


void fq::client::MonsterHP::SetVisible(bool isVisible)
{
	mbIsVisible = isVisible;

	auto infomations = mImageUI->GetUIInfomations();
	for (int i = 0; i < infomations.size(); ++i)
	{
		infomations[i].isRender = isVisible;
	}
	mImageUI->SetUIInfomations(infomations);
}

void fq::client::MonsterHP::OnUpdate(float dt)
{
	using namespace DirectX::SimpleMath;

	if (GetGameObject()->IsDestroyed())
		return;

	mDecreaseTime += dt;

	constexpr float DecreasePresentaionStartTime = 0.25f;
	if (mDecreaseTime >= DecreasePresentaionStartTime)
	{
		mDecreaseRatio = std::max(mDecreaseRatio - mDecreaseSpeed * dt, 0.f);
	}

	if (!mbIsVisible) return;

	Vector3 pos = mTransform->GetWorldPosition();
	pos.y += mWorldYOffset;

	float width = GetScene()->GetScreenManager()->GetFixScreenWidth();
	float height = GetScene()->GetScreenManager()->GetFixScreenHeight();

	fq::game_module::Camera* mainCamera = nullptr;
	GetScene()->GetEventManager()->FireEvent<fq::event::GetMainCamera>({ &mainCamera });
	auto viewProj = mainCamera->GetViewProjection();
	Vector3 screenPos = Vector3::Transform(pos, viewProj);

	auto infomations = mImageUI->GetUIInfomations();

	// OutBar
	auto& outBar = infomations[0];
	outBar.StartX = width * 0.5f + (screenPos.x * width * 0.5f) - mBarSize.x * 0.5f;
	outBar.StartY = height * 0.5f - (screenPos.y * height * 0.5f) + mScreenYOffset;

	// HpBar
	auto& innerBar = infomations[1];
	innerBar.StartX = outBar.StartX;
	innerBar.StartY = outBar.StartY;
	innerBar.XRatio = mHpRatio + mDecreaseRatio;
	innerBar.Width = mBarSize.x * (mHpRatio + mDecreaseRatio);

	// DecreaseBar 
	auto& decreaseBar = infomations[2];
	decreaseBar.StartX = outBar.StartX + mInnerOffset.x + (mHpRatio * (mBarSize.x - mInnerOffset.x * 2.f));
	decreaseBar.StartY = outBar.StartY + mInnerOffset.y;
	decreaseBar.Width = mDecreaseRatio * (mBarSize.x - mInnerOffset.x * 2.f);

	// RankBar
	if (infomations.size() > 3)
	{
		auto& rankBar = infomations[3];
		rankBar.StartX = outBar.StartX - mRankOffset.x;
		rankBar.StartY = outBar.StartY - mRankOffset.y;
	}


	mImageUI->SetUIInfomations(infomations);
}


void fq::client::MonsterHP::OnStart()
{
	mTransform = GetComponent<game_module::Transform>()->GetParentTransform();
	mImageUI = GetComponent<game_module::ImageUI>();
	mImageUI->SetBindTransform(false);

	if (!mImageUI)
		spdlog::warn("{} has not ImageUI Component", GetGameObject()->GetName());

	auto infomations = mImageUI->GetUIInfomations();
	assert(infomations.size() > 2);

	// OutBar
	infomations[0].Width = mBarSize.x;
	infomations[0].Height = mBarSize.y;
	infomations[0].Layer = 230;

	// InnerBar	
	infomations[1].Width = mBarSize.x;
	infomations[1].Height = mBarSize.y;
	infomations[1].Layer = 229;

	// DecreaseBar
	infomations[2].Width = 0.f;
	infomations[2].Height = mBarSize.y - mInnerOffset.y * 2.f;
	infomations[2].Layer = 228;

	// RankBar
	if (infomations.size() > 3)
	{
		infomations[3].Width = mBarSize.x + mRankOffset.x * 2.f;
		infomations[3].Height = mBarSize.y + mRankOffset.y * 2.f;
	}

	mImageUI->SetUIInfomations(infomations);

	SetVisible(mbIsVisible);
}

fq::client::MonsterHP::MonsterHP()
	:mbIsVisible(false)
	, mHpRatio(1.f)
	, mDecreaseRatio(0.f)
	, mDecreaseTime(0.f)
	, mWorldYOffset(0.f)
	, mScreenYOffset(0.f)
	, mBarSize{}
	, mInnerOffset{}
	, mTransform(nullptr)
	, mImageUI(nullptr)
	, mDecreaseSpeed(1.f)
	, mRankOffset(5.f,5.f)
{}

fq::client::MonsterHP::~MonsterHP()
{

}
