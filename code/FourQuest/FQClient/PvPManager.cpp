#include "PvPManager.h"

#include "../FQGameModule/GameModule.h"
#include "../FQGameModule/Transform.h"
#include "../FQGameModule/Sequence.h"

#include "ClientHelper.h"
#include "GameManager.h"

std::shared_ptr<fq::game_module::Component> fq::client::PvPManager::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	auto cloneAssist = std::dynamic_pointer_cast<PvPManager>(clone);

	if (cloneAssist == nullptr) // 새로 생성해서 복사본을 준다
	{
		cloneAssist = game_module::ObjectPool::GetInstance()->Assign<PvPManager>(*this);
	}
	else // clone에 데이터를 복사한다.
	{
		// 기본 대입 연산자 호출한다.
		*cloneAssist = *this;
	}

	return cloneAssist;
}

void fq::client::PvPManager::OnUpdate(float dt)
{
	if (mbIsEndGame)
	{
		auto input = GetScene()->GetInputManager();

		for (int i = 0; i < 4; ++i)
		{
			if (input->IsPadKeyState(i, EPadKey::Start, EKeyState::Tap))
			{
				GetScene()->GetEventManager()->FireEvent<fq::event::RequestChangeScene>({ "PvP", true});
			}
			if (input->IsPadKeyState(i, EPadKey::Back, EKeyState::Tap))
			{
				GetScene()->GetEventManager()->FireEvent<fq::event::RequestChangeScene>({ "TitleUI", true });
			}
		}
	}
	else
	{
		mElapsedTime += dt;

		auto& players = mGameManager->GetPlayers();

		mbIsEndGame = players.size() <= 1 && mElapsedTime >= 10.f;

		if (mbIsEndGame)
		{
			auto object = GetScene()->GetObjectByName(mEndSeuqenceName);
			object->GetComponent<game_module::Sequence>()->SetIsPlay(true);
		}
	}
}

void fq::client::PvPManager::OnStart()
{
	SpawnArmour();

	mGameManager = GetScene()->GetObjectByName("GameManager")->GetComponent<GameManager>();
}

void fq::client::PvPManager::SpawnArmour()
{
	using namespace DirectX::SimpleMath;

	auto& random = helper::RandomGenerator::GetInstance();

	int spawnAromourNum = random.GetRandomNumber(mMinSpawnAromour, mMaxSpawnAromour);

	for (int i = 0; i < spawnAromourNum; ++i)
	{
		int spawnIndex = random.GetRandomNumber(0, mArmours.size() - 1);

		auto instance = GetScene()->GetPrefabManager()->InstantiatePrefabResoure(mArmours[spawnIndex]);

		auto& object = instance[0];
		GetScene()->AddGameObject(object);

		auto armourT = object->GetTransform();
		armourT->SetParent(GetTransform());
		auto position = GetTransform()->GetWorldPosition();

		auto foward = Matrix::CreateFromYawPitchRoll({ 0.f,random.GetRandomNumber(0.f, DirectX::XM_2PI),0.f }).Forward();
		position += foward * random.GetRandomNumber(mMinSpawnRange, mMaxSpawnRange);
		armourT->SetWorldPosition(position);
	}

}
