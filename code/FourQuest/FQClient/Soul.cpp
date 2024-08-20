#include "Soul.h"

#include "../FQGameModule/GameModule.h"
#include "../FQGameModule/CharacterController.h"
#include "../FQGameModule/Transform.h"
#include "../FQGameModule/Particle.h"
#include "DeadArmour.h"
#include "CameraMoving.h"

fq::client::Soul::Soul()
	:mController(nullptr)
	, mSelectArmours{}
{}

fq::client::Soul::~Soul()
{

}

std::shared_ptr<fq::game_module::Component> fq::client::Soul::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	auto cloneSoul = std::dynamic_pointer_cast<Soul>(clone);

	if (cloneSoul == nullptr) // 새로 생성해서 복사본을 준다소울라이크
	{
		cloneSoul = game_module::ObjectPool::GetInstance()->Assign<Soul>(*this);
	}
	else // clone에 데이터를 복사한다.
	{
		// 기본 대입 연산자 호출한다.
		*cloneSoul = *this;
	}

	return cloneSoul;
}

void fq::client::Soul::OnStart()
{
	mController = GetComponent<game_module::CharacterController>();

	// 카메라에 플레이어 등록 
	GetScene()->ViewComponents<CameraMoving>([this](game_module::GameObject& object, CameraMoving& camera)
		{
			camera.AddPlayerTransform(GetComponent<game_module::Transform>());
		});

	// 소울 색깔 지정 
	SetSoulColor();
}

void fq::client::Soul::DestorySoul()
{
	GetScene()->DestroyGameObject(GetGameObject());

	// 카메라에 플레이어 해제 
	GetScene()->ViewComponents<CameraMoving>([this](game_module::GameObject& object, CameraMoving& camera)
		{
			camera.DeletePlayerTransform(GetComponent<game_module::Transform>());
		});

}

void fq::client::Soul::OnTriggerEnter(const fq::game_module::Collision& collision)
{
	auto deadArmour = collision.other->GetComponent<fq::client::DeadArmour>();

	if (deadArmour == nullptr)
		return;

	mSelectArmours.push_back(deadArmour);
}

void fq::client::Soul::OnTriggerExit(const fq::game_module::Collision& collision)
{
	auto deadArmour = collision.other->GetComponent<fq::client::DeadArmour>();

	if (deadArmour == nullptr)
		return;

	mSelectArmours.erase(std::remove(mSelectArmours.begin(), mSelectArmours.end(), deadArmour)
		, mSelectArmours.end()
	);
}

void fq::client::Soul::OnUpdate(float dt)
{
	auto input = GetScene()->GetInputManager();

	// 갑옷과 상호작용합니다.
	if (!mSelectArmours.empty()
		&& input->IsPadKeyState(mController->GetControllerID(), EPadKey::B, EKeyState::Tap))
	{
		// 가장 가까운 갑옷 쿼리
		auto soulPos = GetComponent<game_module::Transform>()->GetWorldPosition();
		float minDistance = 100000.f;
		DeadArmour* closestArmour = nullptr;

		for (auto& armour : mSelectArmours)
		{
			if (armour->GetGameObject()->IsDestroyed())
				continue;

			auto pos = armour->GetComponent<game_module::Transform>()->GetWorldPosition();
			float distance = (soulPos - pos).Length();

			if (minDistance > distance)
			{
				minDistance = distance;
				closestArmour = armour;
			}
		}

		// 가장 가까운 갑옷으로 영혼화
		assert(closestArmour);

		PlayerInfo info{ mController->GetControllerID(), mSoulType };

		closestArmour->SummonLivingArmour(info);
		DestorySoul();
	}

}

void fq::client::Soul::SetSoulColor()
{
	if (GetGameObject() == nullptr)
		return;

	for (auto child : GetGameObject()->GetChildren())
	{
		if (child->HasComponent<game_module::Particle>())
		{
			auto particle = child->GetComponent<game_module::Particle>();
			auto matInfo = particle->GetParticleMaterialInfo();

			switch (mSoulType)
			{
				case fq::client::ESoulType::Sword:
					matInfo.EmissiveColor = mSwordColor;
					break;
				case fq::client::ESoulType::Staff:
					matInfo.EmissiveColor = mStaffColor;
					break;
				case fq::client::ESoulType::Axe:
					matInfo.EmissiveColor = mAxeColor;
					break;
				case fq::client::ESoulType::Bow:
					matInfo.EmissiveColor = mBowColor;
					break;
			}

			particle->SetParticleMaterialInfo(matInfo);
		}
	}

}

void fq::client::Soul::SetSoulType(fq::client::ESoulType type)
{
	mSoulType = type;

	SetSoulColor();
}

