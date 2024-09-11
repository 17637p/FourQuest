#include "GameManager.h"

#include "ClientEvent.h"
#include "Player.h"

#include "../FQGameModule/GameModule.h"
#include "../FQGameModule/CharacterController.h"

// temp
#include "PlayerInfoVariable.h"

std::shared_ptr<fq::game_module::Component> fq::client::GameManager::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	auto cloneGameMgr = std::dynamic_pointer_cast<GameManager>(clone);

	if (cloneGameMgr == nullptr) // ���� �����ؼ� ���纻�� �ش�
	{
		cloneGameMgr = game_module::ObjectPool::GetInstance()->Assign<GameManager>(*this);
	}
	else // clone�� �����͸� �����Ѵ�.
	{
		// �⺻ ���� ������ ȣ���Ѵ�.
		*cloneGameMgr = *this;
	}

	return cloneGameMgr;
}

fq::client::GameManager::~GameManager()
{

}

fq::client::GameManager::GameManager()
	:mIsStop(false),
	mIsAutoSpawn(false)
	//	:mRegisterPlayerHandler{}
{}

fq::client::GameManager::GameManager(const GameManager& other)
	:mRegisterPlayerHandler{}
	, mPlayers(other.mPlayers)
	, mPauseUI(other.mPauseUI)
	, mIsStop(false)
	, mIsAutoSpawn(other.mIsAutoSpawn)
	, mSoul(other.mSoul)
	, mKnight(other.mKnight)
	, mMagic(other.mMagic)
	, mArcher(other.mArcher)
	, mWarrior(other.mWarrior)
{}

fq::client::GameManager& fq::client::GameManager::operator=(const GameManager& other)
{
	mPlayers = other.mPlayers;
	mPauseUI = other.mPauseUI;
	mIsStop = false;
	mIsAutoSpawn = other.mIsAutoSpawn;
	mSoul = other.mSoul;
	mKnight = other.mKnight;
	mMagic = other.mMagic;
	mArcher = other.mArcher;
	mWarrior = other.mWarrior;

	return *this;
}

void fq::client::GameManager::OnUpdate(float dt)
{
	mPlayers.erase(std::remove_if(mPlayers.begin(), mPlayers.end()
		, [](const std::shared_ptr<game_module::GameObject> object)
		{
			return object->IsDestroyed();
		}), mPlayers.end());

	// Pause ó��
	// P, O �� �ӽ� b 
	auto input = GetScene()->GetInputManager();
	if (!mIsStop)
	{
		if (input->IsPadKeyState(0, EPadKey::Start, EKeyState::Tap)
			|| input->IsPadKeyState(1, EPadKey::Start, EKeyState::Tap)
			|| input->IsPadKeyState(2, EPadKey::Start, EKeyState::Tap)
			|| input->IsPadKeyState(3, EPadKey::Start, EKeyState::Tap)
			|| input->IsKeyState(EKey::P, EKeyState::Tap))
		{
			auto instance = GetScene()->GetPrefabManager()->InstantiatePrefabResoure(mPauseUI);
			auto pauseUIObject = *(instance.begin());

			GetScene()->AddGameObject(pauseUIObject);

			// ���� ó��
			GetScene()->GetTimeManager()->SetTimeScale(0);
			mIsStop = true;
		}
	}

	testKey();
}

void fq::client::GameManager::OnStart()
{
	EventProcessOffPopupPause();
	EventProcessOffPopupSetting();

	if (mIsAutoSpawn)
	{
		int playerID = 0;
		switch (PlayerInfoVariable::Player1State)
		{
			case 0:
				SpawnPlayer(mSoul, playerID);
				break;
			case 1:
				SpawnPlayer(mKnight, playerID);
				break;
			case 2:
				SpawnPlayer(mMagic, playerID);
				break;
			case 3:
				SpawnPlayer(mArcher, playerID);
				break;
			case 4:
				SpawnPlayer(mWarrior, playerID);
				break;
			default:
				break;
		}
		playerID = 1;
		switch (PlayerInfoVariable::Player2State)
		{
			case 0:
				SpawnPlayer(mSoul, playerID);
				break;
			case 1:
				SpawnPlayer(mKnight, playerID);
				break;
			case 2:
				SpawnPlayer(mMagic, playerID);
				break;
			case 3:
				SpawnPlayer(mArcher, playerID);
				break;
			case 4:
				SpawnPlayer(mWarrior, playerID);
				break;
			default:
				break;
		}
		playerID = 2;
		switch (PlayerInfoVariable::Player3State)
		{
			case 0:
				SpawnPlayer(mSoul, playerID);
				break;
			case 1:
				SpawnPlayer(mKnight, playerID);
				break;
			case 2:
				SpawnPlayer(mMagic, playerID);
				break;
			case 3:
				SpawnPlayer(mArcher, playerID);
				break;
			case 4:
				SpawnPlayer(mWarrior, playerID);
				break;
			default:
				break;
		}
		playerID = 3;
		switch (PlayerInfoVariable::Player4State)
		{
			case 0:
				SpawnPlayer(mSoul, playerID);
				break;
			case 1:
				SpawnPlayer(mKnight, playerID);
				break;
			case 2:
				SpawnPlayer(mMagic, playerID);
				break;
			case 3:
				SpawnPlayer(mArcher, playerID);
				break;
			case 4:
				SpawnPlayer(mWarrior, playerID);
				break;
			default:
				break;
		}
	}
}

void fq::client::GameManager::OnAwake()
{
	mRegisterPlayerHandler = GetScene()->GetEventManager()->RegisterHandle<client::event::RegisterPlayer>(
		[this](const client::event::RegisterPlayer& event)
		{
			mPlayers.push_back(event.player->shared_from_this());
		});
}


void fq::client::GameManager::OnDestroy()
{
	GetScene()->GetEventManager()->RemoveHandle(mRegisterPlayerHandler);
	GetScene()->GetEventManager()->RemoveHandle(mOffPopupSettingHandler);
	GetScene()->GetEventManager()->RemoveHandle(mOffPopupPauseHandler);
}

void fq::client::GameManager::EventProcessOffPopupPause()
{
	mOffPopupSettingHandler = GetScene()->GetEventManager()->RegisterHandle<client::event::OffPopupSetting>(
		[this]()
		{
			mIsStop = false;
			GetScene()->GetTimeManager()->SetTimeScale(1);
		});
}

void fq::client::GameManager::EventProcessOffPopupSetting()
{
	mOffPopupPauseHandler = GetScene()->GetEventManager()->RegisterHandle<client::event::OffPopupPause>(
		[this]()
		{
			mIsStop = false;
			GetScene()->GetTimeManager()->SetTimeScale(1);
		});
}

void fq::client::GameManager::testKey()
{
	auto input = GetScene()->GetInputManager();

	if (input->IsKeyState(EKey::F1, EKeyState::Tap))
	{
		for (auto& player : mPlayers)
		{
			auto playerComponent = player->GetComponent<Player>();
			if (playerComponent != nullptr)
			{
				playerComponent->SetHp(100000.f);
			}
		}
	}

	if (input->IsKeyState(EKey::F2, EKeyState::Tap))
	{
		SavePlayerState();
		GetScene()->GetEventManager()->FireEvent<fq::event::RequestChangeScene>({ "Scene2", true});
	}
	if (input->IsKeyState(EKey::F3, EKeyState::Tap))
	{
		SavePlayerState();
		GetScene()->GetEventManager()->FireEvent<fq::event::RequestChangeScene>({ "Scene3", true });
	}
	if (input->IsKeyState(EKey::F4, EKeyState::Tap))
	{
		SavePlayerState();
		GetScene()->GetEventManager()->FireEvent<fq::event::RequestChangeScene>({ "Scene4", true });	
	}
}

void fq::client::GameManager::SavePlayerState()
{
	for (auto& player : mPlayers)
	{
		auto playerComponent = player->GetComponent<Player>();
		auto controller = player->GetComponent<game_module::CharacterController>();
		if (controller->GetControllerID() == 0)
		{
			if (playerComponent == nullptr)
			{
				PlayerInfoVariable::Player1State = 0;
			}
			else
			{
				if (playerComponent->GetArmourType() == EArmourType::Knight)
				{
					PlayerInfoVariable::Player1State = 1;
				}
				if (playerComponent->GetArmourType() == EArmourType::Magic)
				{
					PlayerInfoVariable::Player1State = 2;
				}
				if (playerComponent->GetArmourType() == EArmourType::Archer)
				{
					PlayerInfoVariable::Player1State = 3;
				}
				if (playerComponent->GetArmourType() == EArmourType::Warrior)
				{
					PlayerInfoVariable::Player1State = 4;
				}
			}
		}
		if (controller->GetControllerID() == 1)
		{
			if (playerComponent == nullptr)
			{
				PlayerInfoVariable::Player2State = 0;
			}
			else
			{
				if (playerComponent->GetArmourType() == EArmourType::Knight)
				{
					PlayerInfoVariable::Player2State = 1;
				}
				if (playerComponent->GetArmourType() == EArmourType::Magic)
				{
					PlayerInfoVariable::Player2State = 2;
				}
				if (playerComponent->GetArmourType() == EArmourType::Archer)
				{
					PlayerInfoVariable::Player2State = 3;
				}
				if (playerComponent->GetArmourType() == EArmourType::Warrior)
				{
					PlayerInfoVariable::Player2State = 4;
				}
			}
		}
		if (controller->GetControllerID() == 2)
		{
			if (playerComponent == nullptr)
			{
				PlayerInfoVariable::Player3State = 0;
			}
			else
			{
				if (playerComponent->GetArmourType() == EArmourType::Knight)
				{
					PlayerInfoVariable::Player3State = 1;
				}
				if (playerComponent->GetArmourType() == EArmourType::Magic)
				{
					PlayerInfoVariable::Player3State = 2;
				}
				if (playerComponent->GetArmourType() == EArmourType::Archer)
				{
					PlayerInfoVariable::Player3State = 3;
				}
				if (playerComponent->GetArmourType() == EArmourType::Warrior)
				{
					PlayerInfoVariable::Player3State = 4;
				}
			}
		}
		if (controller->GetControllerID() == 3)
		{
			if (playerComponent == nullptr)
			{
				PlayerInfoVariable::Player4State = 0;
			}
			else
			{
				if (playerComponent->GetArmourType() == EArmourType::Knight)
				{
					PlayerInfoVariable::Player4State = 1;
				}
				if (playerComponent->GetArmourType() == EArmourType::Magic)
				{
					PlayerInfoVariable::Player4State = 2;
				}
				if (playerComponent->GetArmourType() == EArmourType::Archer)
				{
					PlayerInfoVariable::Player4State = 3;
				}
				if (playerComponent->GetArmourType() == EArmourType::Warrior)
				{
					PlayerInfoVariable::Player4State = 4;
				}
			}
		}
	}
}

void fq::client::GameManager::SpawnPlayer(fq::game_module::PrefabResource prefab, int index)
{
	std::shared_ptr<game_module::GameObject> newObject;

	auto instance = GetScene()->GetPrefabManager()->InstantiatePrefabResoure(prefab);
	newObject = *(instance.begin());

	GetScene()->AddGameObject(newObject);
	newObject->GetComponent<game_module::CharacterController>()->SetControllerID(index);
}

