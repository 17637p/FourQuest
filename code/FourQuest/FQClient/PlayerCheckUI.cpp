#include "PlayerCheckUI.h"

#include "../FQGameModule/GameModule.h"
#include "../FQGameModule/ImageUI.h"
#include "../FQGameModule/Camera.h"
#include "../FQGameModule/Transform.h"
#include "../FQGameModule/CharacterController.h"
#include "Player.h"
#include "Soul.h"

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

	if (mImageUI->GetUIInfomations().size() != 16)
	{
		spdlog::warn("PlayerCheckUI Error");
	}
	else
	{
		auto playerObject = mTransform->GetParentTransform()->GetGameObject();
		auto player = playerObject->GetComponent<Player>();
		auto soul = playerObject->GetComponent<Soul>();
		auto controller = playerObject->GetComponent<game_module::CharacterController>();

		auto controllerID = controller->GetControllerID();
		auto soulType = player ? player->GetSoulType() : soul->GetSoulType();

		if (soulType == ESoulType::Sword && controllerID == 0) mImageIndex = Sword1P;
		else if (soulType == ESoulType::Staff && controllerID == 0) mImageIndex = Statff1P;
		else if (soulType == ESoulType::Axe && controllerID == 0) mImageIndex = Axe1P;
		else if (soulType == ESoulType::Bow && controllerID == 0) mImageIndex = Bow1P;
		
		else if (soulType == ESoulType::Sword && controllerID == 1) mImageIndex = Sword2P;
		else if (soulType == ESoulType::Staff && controllerID == 1) mImageIndex = Statff2P;
		else if (soulType == ESoulType::Axe && controllerID == 1) mImageIndex = Axe2P;
		else if (soulType == ESoulType::Bow && controllerID == 1) mImageIndex = Bow2P;
		
		else if (soulType == ESoulType::Sword && controllerID == 2) mImageIndex = Sword3P;
		else if (soulType == ESoulType::Staff && controllerID == 2) mImageIndex = Statff3P;
		else if (soulType == ESoulType::Axe && controllerID == 2) mImageIndex = Axe3P;
		else if (soulType == ESoulType::Bow && controllerID == 2) mImageIndex = Bow3P;
		
		else if (soulType == ESoulType::Sword && controllerID == 3) mImageIndex = Sword4P;
		else if (soulType == ESoulType::Staff && controllerID == 3) mImageIndex = Statff4P;
		else if (soulType == ESoulType::Axe && controllerID == 3) mImageIndex = Axe4P;
		else if (soulType == ESoulType::Bow && controllerID == 3) mImageIndex = Bow4P;
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

	fq::game_module::Camera* mainCamera = nullptr;
	GetScene()->GetEventManager()->FireEvent<fq::event::GetMainCamera>({ &mainCamera });
	auto viewProj = mainCamera->GetViewProjection();
	Vector3 screenPos = Vector3::Transform(pos, viewProj);
	auto infomations = mImageUI->GetUIInfomations();

	if (!infomations.empty())
	{
		auto& checkUI = infomations[mImageIndex];
		checkUI.StartX = width * 0.5f + (screenPos.x * width * 0.5f) + mOffset.x - infomations[0].Width * 0.5f;
		checkUI.StartY = height * 0.5f - (screenPos.y * height * 0.5f) + mOffset.y;
		checkUI.isRender = true;
		mImageUI->SetUIInfomations(infomations);
	}
}
