#include "GaugeBar.h"

#include "../FQGameModule/GameModule.h"
#include "../FQGameModule/ImageUI.h"
#include "../FQGameModule/Camera.h"
#include "../FQGameModule/Transform.h"

#include "UILayerDefine.h"

fq::client::GaugeBar::GaugeBar()
	:mbIsVisible(false)
	, mMainCamera(nullptr)
	, mTransform(nullptr)
	, mImageUI(nullptr)
	, mGaugeRatio(1.f)
	, mWorldOffset(0.f)
	, mScreenOffset(0.f)
	, mInnerOffset(5.f)
{

}

fq::client::GaugeBar::~GaugeBar()
{

}

std::shared_ptr<fq::game_module::Component> fq::client::GaugeBar::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	auto cloneGaugeBar = std::dynamic_pointer_cast<GaugeBar>(clone);

	if (cloneGaugeBar == nullptr) // ���� �����ؼ� ���纻�� �شټҿ����ũ
	{
		cloneGaugeBar = game_module::ObjectPool::GetInstance()->Assign<GaugeBar>(*this);
	}
	else // clone�� �����͸� �����Ѵ�.
	{
		// �⺻ ���� ������ ȣ���Ѵ�.
		*cloneGaugeBar = *this;
	}

	return cloneGaugeBar;
}

void fq::client::GaugeBar::OnStart()
{
	mTransform = GetComponent<game_module::Transform>();
	mImageUI = GetComponent<game_module::ImageUI>();

	// MainCamera �������� 
	auto view = GetScene()->GetComponentView<game_module::Camera>();
	for (auto& object : view)
	{
		auto camera = object.GetComponent<game_module::Camera>();
		if (camera->IsMain())
		{
			mMainCamera = camera;
		}
	}

	auto infomations = mImageUI->GetUIInfomations();
	// OutBar
	infomations[3].Width = mBarSize.x;
	infomations[3].Height = mBarSize.y;
	infomations[3].Layer = layer::OutBar;

	// InnerBar
	infomations[4].Width = mBarSize.x - mInnerOffset;
	infomations[4].Height = mBarSize.y - mInnerOffset;
	infomations[4].Layer = layer::InnerBar;

	mImageUI->SetUIInfomations(infomations);

	SetVisible(false);
}

void fq::client::GaugeBar::SetVisible(bool isVisible)
{
	mbIsVisible = isVisible;

	auto infomations = mImageUI->GetUIInfomations();
	for (int i = 3; i < 5; ++i)
	{
		infomations[i].isRender = isVisible;
	}
	mImageUI->SetUIInfomations(infomations);
}

void fq::client::GaugeBar::OnUpdate(float dt)
{
	if (mbIsVisible)
	{
		setUIInfo();
	}
}

void fq::client::GaugeBar::setUIInfo()
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
	auto& outBar = infomations[3];

	outBar.StartX = width * 0.5f + (screenPos.x * width * 0.5f) - mBarSize.x * 0.5f;
	outBar.StartY = height * 0.5f - (screenPos.y * height * 0.5f) + mScreenOffset;

	// InnerBar
	float halfInnerOffset = mInnerOffset * 0.5f;
	auto& innerBar = infomations[4];

	innerBar.Width = (mBarSize.x - mInnerOffset) * mGaugeRatio;
	innerBar.StartX = width * 0.5f + (screenPos.x * width * 0.5f) - mBarSize.x * 0.5f + halfInnerOffset;
	innerBar.StartY = height * 0.5f - (screenPos.y * height * 0.5f) + mScreenOffset + halfInnerOffset;

	mImageUI->SetUIInfomations(infomations);
}

void fq::client::GaugeBar::SetRatio(float ratio)
{
	mGaugeRatio = std::clamp(ratio, 0.f, 1.f);
}
