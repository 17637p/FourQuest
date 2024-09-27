#define NOMINMAX
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
#include "SoulVariable.h"

fq::client::DeadArmour::DeadArmour()
	:mPlayerCount(0)
	, mbIsVisible(false)
	, mUnequippedPlayerId(-1)
	, mPlayerArmourCoolTime(0.f)
	, mbIsSummonAble(true)
{}

fq::client::DeadArmour::~DeadArmour()
{

}

std::shared_ptr<fq::game_module::Component> fq::client::DeadArmour::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	auto cloneController = std::dynamic_pointer_cast<DeadArmour>(clone);

	if (cloneController == nullptr) // 새로 생성해서 복사본을 준다
	{
		cloneController = game_module::ObjectPool::GetInstance()->Assign<DeadArmour>(*this);
	}
	else // clone에 데이터를 복사한다.
	{
		// 기본 대입 연산자 호출한다.
		*cloneController = *this;
	}

	return cloneController;
}

bool fq::client::DeadArmour::SummonLivingArmour(PlayerInfo info)
{
	assert(info.ControllerID <= 3);

	if (mUnequippedPlayerId == info.ControllerID && mPlayerArmourCoolTime < SoulVariable::ArmourCoolTime)
		return false;

	// 인스턴스 생성
	auto instance = GetScene()->GetPrefabManager()->InstantiatePrefabResoure(mLivingArmourPrefab);
	auto& livingArmour = *(instance.begin());

	// 컨트롤러 연결
	livingArmour->GetComponent<game_module::CharacterController>()->SetControllerID(info.ControllerID);

	// 영혼 타입 설정
	livingArmour->GetComponent<Player>()->SetSoulType(info.SoulType);

	// 위치 설정
	auto world = GetComponent<game_module::Transform>()->GetWorldMatrix();
	livingArmour->GetComponent<game_module::Transform>()->SetWorldMatrix(world);

	GetScene()->AddGameObject(livingArmour);

	// 상호작용 이벤트 발생
	GetScene()->GetEventManager()->FireEvent<client::event::ObjectInteractionEvent>(
		{ GetGameObject()->GetTag() });

	// DeadArmour 삭제 
	GetScene()->DestroyGameObject(GetGameObject());

	return true;
}

void fq::client::DeadArmour::OnTriggerEnter(const game_module::Collision& collision)
{
	if (collision.other->HasComponent<Soul>())
	{
		mPlayerCount++;
	}

	if (mPlayerCount > 0 && mbIsSummonAble)
	{
		constexpr DirectX::SimpleMath::Color Yellow{ 0.8f,0.6f,0.2f,1.f };
		setOutlineColor(Yellow);
		setUI(true);
	}
}

void fq::client::DeadArmour::OnTriggerExit(const game_module::Collision& collision)
{
	if (collision.other->HasComponent<Soul>())
	{
		if (mPlayerCount > 0)
		{
			--mPlayerCount;
		}
	}

	if (mPlayerCount == 0 && mbIsSummonAble)
	{
		constexpr DirectX::SimpleMath::Color NoOutLine{ 0.f,0.f,0.f,1.f };

		setOutlineColor(NoOutLine);
		setUI(false);
	}
}

void fq::client::DeadArmour::OnStart()
{
	assert(GetComponent<game_module::ImageUI>() != nullptr);

	setUI(false);
	mbIsSummonAble = true;


}

void fq::client::DeadArmour::setUI(bool isVisible)
{
	using namespace DirectX::SimpleMath;

	mbIsVisible = isVisible;
	auto imageUI = GetComponent<game_module::ImageUI>();
	auto uiInfo = imageUI->GetUIInfomations();

	if (!uiInfo.empty())
	{
		uiInfo[0].isRender = true;

		if (uiInfo.size() > 1)
			uiInfo[1].isRender = false;
	}
	imageUI->SetUIInfomations(uiInfo);
}

void fq::client::DeadArmour::OnUpdate(float dt)
{
	using namespace DirectX::SimpleMath;

	mPlayerArmourCoolTime += dt;

	// UI 위치 갱신 
	{
		fq::game_module::Camera* mainCamera = nullptr;

		GetScene()->GetEventManager()->FireEvent <fq::event::GetMainCamera>(
			{
				&mainCamera
			});
		Vector3 pos = GetComponent<game_module::Transform>()->GetWorldPosition();

		float height = static_cast<float>(GetScene()->GetScreenManager()->GetFixScreenHeight());
		float width = static_cast<float>(GetScene()->GetScreenManager()->GetFixScreenWidth());

		auto viewProj = mainCamera->GetViewProjection();
		Vector3 screenPos = Vector3::Transform(pos, viewProj);
		auto imageUI = GetComponent<game_module::ImageUI>();
		assert(imageUI);

		auto uiInfomations = imageUI->GetUIInfomations();

		uiInfomations[0].StartX = width * 0.5f + (screenPos.x * width * 0.5f) - 25.f;
		uiInfomations[0].StartY = height * 0.5f - (screenPos.y * height * 0.5f);

		if (uiInfomations.size() > 1)
		{
			uiInfomations[1].StartX = uiInfomations[0].StartX + 60.f;
			uiInfomations[1].StartY = uiInfomations[0].StartY;
		}

		imageUI->SetUIInfomations(uiInfomations);
	}

}

void fq::client::DeadArmour::SetSummonAble(bool isSummonAble)
{
	mbIsSummonAble = isSummonAble;

	if (!mbIsSummonAble)
	{
		constexpr DirectX::SimpleMath::Color Red = { 1.f,0.f,0.f,1.f };
		setOutlineColor(Red);
		setUI(false);
	}
	else
	{
		constexpr DirectX::SimpleMath::Color NoOutLine{ 0.f,0.f,0.f,1.f };
		setOutlineColor(NoOutLine);
	}
}

bool fq::client::DeadArmour::IsSummonAble() const
{
	return !GetGameObject()->IsDestroyed() && mbIsSummonAble;
}

void fq::client::DeadArmour::CheckArmourCoolTime(unsigned int playerID) const
{

}

void fq::client::DeadArmour::setOutlineColor(DirectX::SimpleMath::Color color)
{
	for (auto& child : GetGameObject()->GetChildren())
	{
		// OutLine
		if (child->HasComponent<game_module::SkinnedMeshRenderer>())
		{
			auto skinnedMeshRenderer = child->GetComponent<game_module::SkinnedMeshRenderer>();
			auto info = skinnedMeshRenderer->GetMeshObjectInfomation();
			info.OutlineColor = color;
			skinnedMeshRenderer->SetMeshObjectInfomation(info);
		}
	}
}

