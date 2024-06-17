#include "HpBar.h"

fq::client::HpBar::~HpBar()
{

}

fq::client::HpBar::HpBar()
	:mIsVisible(false)
	, mMainCamera(nullptr)
	, mTransform(nullptr)
	, mImageUI(nullptr)
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
	using namespace DirectX::SimpleMath;

	auto viewProj = mMainCamera->GetViewProjection();

	Vector3 pos = mTransform->GetWorldPosition();
	Vector3 screenPos = Vector3::Transform(pos, viewProj);
	auto infomations = mImageUI->GetUIInfomations();

	float height = GetScene()->GetScreenManager()->GetScreenHeight();
	float width = GetScene()->GetScreenManager()->GetScreenWidth();

	for (auto& info : infomations)
	{
		info.StartX = width * 0.5f + screenPos.x * width * 0.5f;
		info.StartY = height * 0.5f - screenPos.y * height * 0.5f;
	}
	mImageUI->SetUIInfomations(infomations);

	//spdlog::debug("UI Pos : {} {} {}", pos.x, pos.y, pos.z);
}

void fq::client::HpBar::SetVisible(bool isVisible)
{
	mIsVisible = isVisible;
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
}
