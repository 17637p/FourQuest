#define NOMINMAX
#include "PlayerHPBar.h"

#include "../FQGameModule/GameModule.h"
#include "../FQGameModule/ImageUI.h"
#include "../FQGameModule/Camera.h"
#include "../FQGameModule/Transform.h"
#include "../FQGameModule/CharacterController.h"

#include "ClientEvent.h"
#include "Player.h"

std::shared_ptr<fq::game_module::Component> fq::client::PlayerHPBar::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	auto cloneHpBar = std::dynamic_pointer_cast<PlayerHPBar>(clone);

	if (cloneHpBar == nullptr) // 새로 생성해서 복사본을 준다소울라이크
	{
		cloneHpBar = game_module::ObjectPool::GetInstance()->Assign<PlayerHPBar>(*this);
	}
	else // clone에 데이터를 복사한다.
	{
		// 기본 대입 연산자 호출한다.
		*cloneHpBar = *this;
	}

	return cloneHpBar;
}

fq::client::PlayerHPBar::PlayerHPBar()
	:mbIsVisible(false)
	,mDecreaseRatio(0.f)
	,mInnerOffset{}
	,mHpRatio(1.f)
	,mDecreaseTime(0.f)
	,mInnerBarSize{}
	,mTransform(nullptr)
	,mImageUI(nullptr)
{}

fq::client::PlayerHPBar::~PlayerHPBar()
{}

void fq::client::PlayerHPBar::OnUpdate(float dt)
{
	using namespace DirectX::SimpleMath;

	mDecreaseTime += dt;
	constexpr float DecreasePresentaionStartTime = 0.25f;
	constexpr float DecreaseSpeed = 1.f;
	if (mDecreaseTime >= DecreasePresentaionStartTime)
	{
		mDecreaseRatio = std::max(mDecreaseRatio - DecreaseSpeed * dt, 0.f);
	}

	if (GetGameObject()->IsDestroyed())
		return;

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
	outBar.StartX = width * 0.5f + (screenPos.x * width * 0.5f) - outBar.Width * 0.5f;
	outBar.StartY = height * 0.5f - (screenPos.y * height * 0.5f) + mScreenYOffset;

	// InnerBar
	auto& innerBar = infomations[1];
	innerBar.StartX = outBar.StartX + mInnerOffset.x;
	innerBar.StartY = outBar.StartY + mInnerOffset.y;
	innerBar.XRatio = mHpRatio; 
	innerBar.Width = mHpRatio * mInnerBarSize.x;

	// DecreaseBar 
	auto& decreaseBar = infomations[2];
	decreaseBar.StartX = innerBar.StartX;
	decreaseBar.StartY = innerBar.StartY;
	decreaseBar.Width = mHpRatio * mInnerBarSize.x;
	decreaseBar.XRatio = mHpRatio + mDecreaseRatio;
	decreaseBar.Width = (mHpRatio +mDecreaseRatio )* mInnerBarSize.x;

	mImageUI->SetUIInfomations(infomations);

}

void fq::client::PlayerHPBar::SetVisible(bool isVisible)
{
	mbIsVisible = isVisible;
	auto infomations = mImageUI->GetUIInfomations();
	for (int i = 0; i < infomations.size(); ++i)
	{
		infomations[i].isRender = isVisible;
	}
	mImageUI->SetUIInfomations(infomations);
}


void fq::client::PlayerHPBar::DecreaseHp(float ratio)
{
	if (mHpRatio == 0.f) return;

	if (!mbIsVisible)
	{
		SetVisible(true);
	}

	float decreaseHpRatio = mHpRatio - ratio;
	if (decreaseHpRatio >= 0.f)
	{
		mDecreaseRatio = ratio;
		mHpRatio = decreaseHpRatio;
	}
	else
	{
		mDecreaseRatio = mHpRatio;
		mHpRatio = 0.f;
	}

	if (mTransform->GetComponent<game_module::CharacterController>())
	{
		int playerID = mTransform->GetComponent<game_module::CharacterController>()->GetControllerID();
		GetScene()->GetEventManager()->FireEvent<event::DecreaseHPRatio>({ playerID, mDecreaseRatio });
	}

	mDecreaseTime = 0.f;
}

void fq::client::PlayerHPBar::OnStart()
{
	mTransform = GetComponent<game_module::Transform>()->GetParentTransform();
	mImageUI = GetComponent<game_module::ImageUI>();
	mImageUI->SetBindTransform(false);

	auto infomations = mImageUI->GetUIInfomations();
	
	auto& outBar = infomations[0];
	auto& innerBar = infomations[1];
//	auto& decreaseBar = infomations[2];

	mInnerOffset = { innerBar.StartX, innerBar.StartY };
	mInnerBarSize = { innerBar.Width , innerBar.Height };
	// OutBar


	// InnerBar 

	// DecreaseBar

	SetVisible(true);
}
