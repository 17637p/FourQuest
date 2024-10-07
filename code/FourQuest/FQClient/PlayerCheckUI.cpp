#include "PlayerCheckUI.h"

#include "../FQGameModule/GameModule.h"
#include "../FQGameModule/ImageUI.h"
#include "../FQGameModule/Camera.h"
#include "../FQGameModule/Transform.h"

std::shared_ptr<fq::game_module::Component> fq::client::PlayerCheckUI::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	auto clonePlayerCheckUI = std::dynamic_pointer_cast<PlayerCheckUI>(clone);

	if (clonePlayerCheckUI == nullptr) // 새로 생성해서 복사본을 준다소울라이크
	{
		clonePlayerCheckUI = game_module::ObjectPool::GetInstance()->Assign<PlayerCheckUI>(*this);
	}
	else // clone에 데이터를 복사한다.
	{
		// 기본 대입 연산자 호출한다.
		*clonePlayerCheckUI = *this;
	}

	return clonePlayerCheckUI;
}

void fq::client::PlayerCheckUI::OnStart()
{
	mTransform = GetComponent<game_module::Transform>();
	mImageUI = GetComponent<game_module::ImageUI>();
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

}

void fq::client::PlayerCheckUI::OnUpdate(float dt)
{
	using namespace DirectX::SimpleMath;

	if (GetGameObject()->IsDestroyed())
		return;

	Vector3 pos = mTransform->GetWorldPosition();
	pos.y += mWorldOffset;

	float width = GetScene()->GetScreenManager()->GetFixScreenWidth();
	float height = GetScene()->GetScreenManager()->GetFixScreenHeight();

	auto viewProj = mMainCamera->GetViewProjection();
	Vector3 screenPos = Vector3::Transform(pos, viewProj);
	auto infomations = mImageUI->GetUIInfomations();
	
	if (!infomations.empty())
	{
		auto& checkUI = infomations[0];
		checkUI.StartX = width * 0.5f + (screenPos.x * width * 0.5f) + mOffset.x - infomations[0].Width *0.5f;
		checkUI.StartY = height * 0.5f - (screenPos.y * height * 0.5f) + mOffset.y;
		mImageUI->SetUIInfomations(infomations);
	}
}
