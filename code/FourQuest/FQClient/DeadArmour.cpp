#include "DeadArmour.h"

#include "../FQGameModule/GameModule.h"
#include "../FQGameModule/Transform.h"
#include "../FQGameModule/SkinnedMeshRenderer.h"
#include "../FQGameModule/Camera.h"
#include "../FQGameModule/ImageUI.h"
#include "../FQGameModule/CharacterController.h"

#include "Soul.h"
#include "Player.h"
#include "ClientEvent.h"

fq::client::DeadArmour::DeadArmour()
	:mPlayerCount(0)
	, mbIsVisible(false)
{}

fq::client::DeadArmour::~DeadArmour()
{

}

std::shared_ptr<fq::game_module::Component> fq::client::DeadArmour::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	auto cloneController = std::dynamic_pointer_cast<DeadArmour>(clone);

	if (cloneController == nullptr) // ���� �����ؼ� ���纻�� �ش�
	{
		cloneController = game_module::ObjectPool::GetInstance()->Assign<DeadArmour>(*this);
	}
	else // clone�� �����͸� �����Ѵ�.
	{
		// �⺻ ���� ������ ȣ���Ѵ�.
		*cloneController = *this;
	}

	return cloneController;
}

void fq::client::DeadArmour::SummonLivingArmour(PlayerInfo info)
{
	assert(info.ControllerID <= 3);

	// �ν��Ͻ� ����
	auto instance = GetScene()->GetPrefabManager()->InstantiatePrefabResoure(mLivingArmourPrefab);
	auto& livingArmour = *(instance.begin());

	// ��Ʈ�ѷ� ����
	livingArmour->GetComponent<game_module::CharacterController>()->SetControllerID(info.ControllerID);

	// ��ȥ Ÿ�� ����
	livingArmour->GetComponent<Player>()->SetSoulType(info.SoulType);

	// ��ġ ����
	auto world = GetComponent<game_module::Transform>()->GetWorldMatrix();
	livingArmour->GetComponent<game_module::Transform>()->SetWorldMatrix(world);

	GetScene()->AddGameObject(livingArmour);

	// ��ȣ�ۿ� �̺�Ʈ �߻�
	GetScene()->GetEventManager()->FireEvent<client::event::ObjectInteractionEvent>(
		{ GetGameObject()->GetTag() });

	// DeadArmour ���� 
	GetScene()->DestroyGameObject(GetGameObject());
}

void fq::client::DeadArmour::OnTriggerEnter(const game_module::Collision& collision)
{
	if (collision.object->HasComponent<Soul>())
	{
		++mPlayerCount;
	}

	for (auto& child : GetGameObject()->GetChildren())
	{
		if (child->HasComponent<game_module::SkinnedMeshRenderer>())
		{
			auto skinnedMeshRenderer = child->GetComponent<game_module::SkinnedMeshRenderer>();
			auto info = skinnedMeshRenderer->GetMeshObjectInfomation();
			info.OutlineColor = { 0.8f, 0.6f, 0.2f, 1.0f };

			skinnedMeshRenderer->SetMeshObjectInfomation(info);
		}

		// ��ȣ�ۿ� UI 
		setUI(true);
	}
}

void fq::client::DeadArmour::OnTriggerExit(const game_module::Collision& collision)
{
	if (collision.object->HasComponent<Soul>())
	{
		--mPlayerCount;
	}

	if (mPlayerCount == 0)
	{
		for (auto& child : GetGameObject()->GetChildren())
		{
			// OutLine
			if (child->HasComponent<game_module::SkinnedMeshRenderer>())
			{
				child->GetComponent<game_module::SkinnedMeshRenderer>()->GetMeshObjectInfomation().OutlineColor = { 0.f, 0.f, 0.f, 1.0f };
			}
			// ��ȣ�ۿ� UI 
			setUI(false);
		}
	}
}

void fq::client::DeadArmour::OnStart()
{
	assert(GetComponent<game_module::ImageUI>() != nullptr);

	setUI(false);
}

void fq::client::DeadArmour::setUI(bool isVisible)
{
	using namespace DirectX::SimpleMath;

	mbIsVisible = isVisible;
	auto imageUI = GetComponent<game_module::ImageUI>();

	auto uiInfo = imageUI->GetUIInfomations();
	if (isVisible)
	{
		// UI ���̰� ���� 
		uiInfo[0].ScaleX = 1.f;
		uiInfo[0].ScaleY = 1.f;
	}
	else
	{
		uiInfo[0].ScaleX = 0.f;
		uiInfo[0].ScaleY = 0.f;
	}

	imageUI->SetUIInfomations(uiInfo);
}

void fq::client::DeadArmour::OnUpdate(float dt)
{
	using namespace DirectX::SimpleMath;

	// UI ��ġ ���� 
	if (mbIsVisible)
	{
		fq::game_module::Camera* mainCamera = nullptr;

		GetScene()->GetEventManager()->FireEvent < fq::event::GetMainCamera>(
			{
				&mainCamera
			});
		Vector3 pos = GetComponent<game_module::Transform>()->GetWorldPosition();

		float height = static_cast<float>(GetScene()->GetScreenManager()->GetScreenHeight());
		float width = static_cast<float>(GetScene()->GetScreenManager()->GetScreenWidth());

		auto viewProj = mainCamera->GetViewProjection();
		Vector3 screenPos = Vector3::Transform(pos, viewProj);
		auto imageUI = GetComponent<game_module::ImageUI>();
		assert(imageUI);

		auto uiInfomations = imageUI->GetUIInfomations();

		uiInfomations[0].StartX = width * 0.5f + (screenPos.x * width * 0.5f);
		uiInfomations[0].StartY = height * 0.5f - (screenPos.y * height * 0.5f);

		imageUI->SetUIInfomations(uiInfomations);
	}

}

