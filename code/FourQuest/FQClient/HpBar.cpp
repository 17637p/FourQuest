#define NOMINMAX
#include "HpBar.h"

#include "../FQGameModule/GameModule.h"
#include "../FQGameModule/ImageUI.h"
#include "../FQGameModule/Camera.h"
#include "../FQGameModule/Transform.h"

#include "UILayerDefine.h"
#include "ClientEvent.h"
#include "Player.h"

fq::client::HpBar::~HpBar()
{

}

fq::client::HpBar::HpBar()
	:mbIsVisible(true)
	, mMainCamera(nullptr)
	, mTransform(nullptr)
	, mImageUI(nullptr)
	, mHpRatio(1.f)
	, mDecreaseRatio(0.f)
	, mDecreaseOffset(0.5f)
	, mDeceraseTime(0.f)
	, mDecreaseSpeed(1.f)
	, mWorldOffset(0.f)
	, mScreenOffset(0.f)
	, mInnerOffset(5.f)
{}

std::shared_ptr<fq::game_module::Component> fq::client::HpBar::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	auto cloneHpBar = std::dynamic_pointer_cast<HpBar>(clone);

	if (cloneHpBar == nullptr) // 새로 생성해서 복사본을 준다소울라이크
	{
		cloneHpBar = game_module::ObjectPool::GetInstance()->Assign<HpBar>(*this);
	}
	else // clone에 데이터를 복사한다.
	{
		// 기본 대입 연산자 호출한다.
		*cloneHpBar = *this;
	}

	return cloneHpBar;
}

void fq::client::HpBar::OnUpdate(float dt)
{
	mDeceraseTime += dt;

	if (mDeceraseTime >= mDecreaseOffset)
		mDecreaseRatio = std::max(mDecreaseRatio - mDecreaseSpeed * dt, 0.f);

	if (!mbIsVisible) return;

	// UI 정도 전달 
	setUIInfo();
}

void fq::client::HpBar::SetVisible(bool isVisible)
{
	mbIsVisible = isVisible;

	auto infomations = mImageUI->GetUIInfomations();	
	for (int i = 0; i < 3; ++i)
	{
		infomations[i].isRender = isVisible;
	}
	mImageUI->SetUIInfomations(infomations);
}

void fq::client::HpBar::OnStart()
{
	mTransform = GetComponent<game_module::Transform>();
	mImageUI = GetComponent<game_module::ImageUI>();
	mImageUI->SetBindTransform(false);

	if (!mImageUI)
		spdlog::warn("{} has not ImageUI Component", GetGameObject()->GetName());

	auto view = GetScene()->GetComponentView<game_module::Camera>();

	// MainCamera 가져오기 
	for (auto& object : view)
	{
		auto camera = object.GetComponent<game_module::Camera>();

		if (object.GetName() == "MainCamera")
		{
			mMainCamera = camera;
		}
	}

	auto infomations = mImageUI->GetUIInfomations();
	assert(infomations.size() > 2);

	// OutBar
	infomations[0].Width = mBarSize.x;
	infomations[0].Height = mBarSize.y;
	infomations[0].Layer = layer::OutBar;

	// InnerBar
	infomations[1].Width = mBarSize.x - mInnerOffset;
	infomations[1].Height = mBarSize.y - mInnerOffset;
	infomations[1].Layer = layer::InnerBar;

	// DecreaseBar
	infomations[2].Width = mBarSize.x - mInnerOffset;
	infomations[2].Height = mBarSize.y - mInnerOffset;
	infomations[2].Layer = layer::DecreaseBar;

	mImageUI->SetUIInfomations(infomations);

	SetVisible(mbIsVisible);
}

void fq::client::HpBar::DecreaseHp(float ratio)
{
	if (mHpRatio == 0.f) return;

	if (!mbIsVisible)
	{
		SetVisible(true);
	}

	// ratio는 줄어들 HP 비율 (받은 데미지 양)
	// mHPRatio는 기존 비율, 따라서 - 한 것이 줄어든 비율 
	float decreaseHpRatio = mHpRatio - ratio;

	// 줄어든 비율이 0보다 크면 == 체력이 줄었다면 
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

	if (GetGameObject()->HasComponent<Player>())
	{
		int playerID = GetComponent<Player>()->GetPlayerID();
		GetScene()->GetEventManager()->FireEvent<event::DecreaseHPRatio>({ playerID, mDecreaseRatio });
	}

	mDeceraseTime = 0.f;
}


void fq::client::HpBar::setUIInfo()
{
	if (GetGameObject()->IsDestroyed())
		return;

	using namespace DirectX::SimpleMath;

	Vector3 pos = mTransform->GetWorldPosition();
	pos.y += mWorldOffset;

	float width = GetScene()->GetScreenManager()->GetFixScreenWidth();
	float height = GetScene()->GetScreenManager()->GetFixScreenHeight();

	auto viewProj = mMainCamera->GetViewProjection();
	Vector3 screenPos = Vector3::Transform(pos, viewProj);
	auto infomations = mImageUI->GetUIInfomations();

	// OutBar 
	auto& outBar = infomations[0];

	outBar.StartX = width * 0.5f + (screenPos.x * width * 0.5f) - mBarSize.x * 0.5f;
	outBar.StartY = height * 0.5f - (screenPos.y * height * 0.5f) + mScreenOffset;

	// InnerBar
	float halfInnerOffset = mInnerOffset * 0.5f;
	auto& innerBar = infomations[1];

	innerBar.Width = (mBarSize.x - mInnerOffset) * mHpRatio;

	innerBar.StartX = width * 0.5f + (screenPos.x * width * 0.5f) - mBarSize.x * 0.5f + halfInnerOffset;
	innerBar.StartY = height * 0.5f - (screenPos.y * height * 0.5f) + mScreenOffset + halfInnerOffset;

	// DecreaseBar
	auto& decreaseBar = infomations[2];

	decreaseBar.Width = (mBarSize.x - mInnerOffset) * (mHpRatio + mDecreaseRatio);

	decreaseBar.StartX = width * 0.5f + (screenPos.x * width * 0.5f) - mBarSize.x * 0.5f + halfInnerOffset;
	decreaseBar.StartY = height * 0.5f - (screenPos.y * height * 0.5f) + mScreenOffset + halfInnerOffset;

	mImageUI->SetUIInfomations(infomations);
}

void fq::client::HpBar::SetHp(float ratio)
{
	mHpRatio = std::clamp(ratio, 0.f, 1.f);
	mDecreaseRatio = 0.f;
}
