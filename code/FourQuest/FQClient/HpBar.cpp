#define NOMINMAX

#include "HpBar.h"
#include "UILayerDefine.h"

fq::client::HpBar::~HpBar()
{

}

fq::client::HpBar::HpBar()
	:mIsVisible(true)
	, mMainCamera(nullptr)
	, mTransform(nullptr)
	, mImageUI(nullptr)
	, mHpRatio(1.f)
	, mDecreaseRatio(0.f)
	, mDecreaseOffset(0.5f)
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

	if (!mIsVisible) return;

	// UI 정도 전달 
	setUIInfo();
}

void fq::client::HpBar::SetVisible(bool isVisible)
{
	mIsVisible = isVisible;

	if (!mIsVisible)
	{
		auto infomations = mImageUI->GetUIInfomations();
		for (int i = 0; i < 3; ++i)
		{
			infomations[i].Width = 0.f;
			infomations[i].Height = 0.f;
		}
		mImageUI->SetUIInfomations(infomations);
	}
	else
	{
		auto infomations = mImageUI->GetUIInfomations();
		assert(infomations.size() == 3);

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
	}
}

void fq::client::HpBar::OnStart()
{
	mTransform = GetComponent<game_module::Transform>();
	mImageUI = GetComponent<game_module::ImageUI>();

	if (!mImageUI)
		spdlog::warn("{} has not ImageUI Component", GetGameObject()->GetName());

	auto view = GetScene()->GetComponentView<game_module::Camera>();

	// MainCamera 가져오기 
	for (auto& object : view)
	{
		auto camera = object.GetComponent<game_module::Camera>();

		if (camera->IsMain())
		{
			mMainCamera = camera;
		}
	}

	SetVisible(mIsVisible);
}

void fq::client::HpBar::DecreaseHp(float ratio)
{
	if (mHpRatio == 0.f) return;

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

	mDeceraseTime = 0.f;
}


void fq::client::HpBar::setUIInfo()
{
	using namespace DirectX::SimpleMath;

	Vector3 pos = mTransform->GetWorldPosition();
	pos.y += mWorldOffset;

	float height = GetScene()->GetScreenManager()->GetScreenHeight();
	float width = GetScene()->GetScreenManager()->GetScreenWidth();

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
