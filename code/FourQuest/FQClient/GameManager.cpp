#include "GameManager.h"

#include "ClientEvent.h"
#include "Player.h"
#include "Soul.h"
#include "SoulManagerModule.h"

#include "../FQGameModule/GameModule.h"
#include "../FQGameModule/CharacterController.h"
#include "../FQGameModule/Transform.h"

// temp
#include "PlayerInfoVariable.h"
#include "SoulVariable.h"

std::shared_ptr<fq::game_module::Component> fq::client::GameManager::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	auto cloneGameMgr = std::dynamic_pointer_cast<GameManager>(clone);

	if (cloneGameMgr == nullptr) // 새로 생성해서 복사본을 준다
	{
		cloneGameMgr = game_module::ObjectPool::GetInstance()->Assign<GameManager>(*this);
	}
	else // clone에 데이터를 복사한다.
	{
		// 기본 대입 연산자 호출한다.
		*cloneGameMgr = *this;
	}

	return cloneGameMgr;
}

fq::client::GameManager::~GameManager()
{

}

fq::client::GameManager::GameManager()
	:mIsStop(false),
	mIsAutoSpawn(false),
	mSoulManagerModule(std::make_shared<SoulManagerModule>())
	//	:mRegisterPlayerHandler{}
{}

fq::client::GameManager::GameManager(const GameManager& other)
	: mRegisterPlayerHandler{}
	, mUpdatePlayerStateHandler{}
	, mDestroyArmourHandler{}
	, mPlayers(other.mPlayers)
	, mPauseUI(other.mPauseUI)
	, mIsStop(false)
	, mIsAutoSpawn(other.mIsAutoSpawn)
	, mSoul(other.mSoul)
	, mKnight(other.mKnight)
	, mMagic(other.mMagic)
	, mArcher(other.mArcher)
	, mWarrior(other.mWarrior)
	, mSoulManagerModule(std::make_shared<SoulManagerModule>())
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
	mSoulManagerModule = std::make_shared<SoulManagerModule>();

	return *this;
}

void fq::client::GameManager::OnUpdate(float dt)
{
	mSoulManagerModule->OnUpdate(dt);

	mPlayers.erase(std::remove_if(mPlayers.begin(), mPlayers.end()
		, [](const std::shared_ptr<game_module::GameObject> object)
		{
			return object->IsDestroyed();
		}), mPlayers.end());

	// Pause 처리
	auto input = GetScene()->GetInputManager();
	if (!mIsStop)
	{
		for (int i = 0; i < 4; i++)
		{
			if (input->IsPadKeyState(i, EPadKey::Start, EKeyState::Tap) || 
				input->IsKeyState(EKey::ESC, EKeyState::Tap))
			{
				auto instance = GetScene()->GetPrefabManager()->InstantiatePrefabResoure(mPauseUI);
				auto pauseUIObject = *(instance.begin());

				GetScene()->AddGameObject(pauseUIObject);

				// 멈춤 처리
				GetScene()->GetTimeManager()->SetTimeScale(0);
				mIsStop = true;
			}
		}
	}

	testKey();

	// 모든 영혼이 파괴되었을 때, GameOver 씬으로 이동
	if (mSoulManagerModule->CheckGameOver())
	{
		GetScene()->GetEventManager()->FireEvent < fq::event::RequestChangeScene>({"GameOver", true});
	}
}

void fq::client::GameManager::OnStart()
{
	mSoulManagerModule->OnStart(GetScene());


	EventProcessOffPopupPause();
	EventProcessOffPopupSetting();

	if (mIsAutoSpawn)
	{
		int playerID = 0;
		ESoulType playerSoulType = static_cast<ESoulType>(PlayerInfoVariable::Player1SoulType);
		switch (PlayerInfoVariable::Player1State)
		{
			case 0:
			{
				auto player = SpawnPlayer(mSoul, playerID);
				player->GetComponent<Soul>()->SetSoulType(playerSoulType);
				break;
			}
			case 1:
			{
				auto player = SpawnPlayer(mKnight, playerID);
				player->GetComponent<Player>()->SetSoulType(playerSoulType);
				break;
			}
			case 2:
			{
				auto player = SpawnPlayer(mMagic, playerID);
				player->GetComponent<Player>()->SetSoulType(playerSoulType);
				break;
			}
			case 3:
			{
				auto player = SpawnPlayer(mArcher, playerID);
				player->GetComponent<Player>()->SetSoulType(playerSoulType);
				break;
			}
			case 4:
			{
				auto player = SpawnPlayer(mWarrior, playerID);
				player->GetComponent<Player>()->SetSoulType(playerSoulType);
				break;
			}
			default:
				break;
		}
		playerID = 1;
		playerSoulType = static_cast<ESoulType>(PlayerInfoVariable::Player2SoulType);
		switch (PlayerInfoVariable::Player2State)
		{
			case 0:
			{
				auto player = SpawnPlayer(mSoul, playerID);
				player->GetComponent<Soul>()->SetSoulType(playerSoulType);
				break;
			}
			case 1:
			{
				auto player = SpawnPlayer(mKnight, playerID);
				player->GetComponent<Player>()->SetSoulType(playerSoulType);
				break;
			}
			case 2:
			{
				auto player = SpawnPlayer(mMagic, playerID);
				player->GetComponent<Player>()->SetSoulType(playerSoulType);
				break;
			}
			case 3:
			{
				auto player = SpawnPlayer(mArcher, playerID);
				player->GetComponent<Player>()->SetSoulType(playerSoulType);
				break;
			}
			case 4:
			{
				auto player = SpawnPlayer(mWarrior, playerID);
				player->GetComponent<Player>()->SetSoulType(playerSoulType);
				break;
			}
			default:
				break;
		}
		playerID = 2;
		playerSoulType = static_cast<ESoulType>(PlayerInfoVariable::Player3SoulType);
		switch (PlayerInfoVariable::Player3State)
		{
			case 0:
			{
				auto player = SpawnPlayer(mSoul, playerID);
				player->GetComponent<Soul>()->SetSoulType(playerSoulType);
				break;
			}
			case 1:
			{
				auto player = SpawnPlayer(mKnight, playerID);
				player->GetComponent<Player>()->SetSoulType(playerSoulType);
				break;
			}
			case 2:
			{
				auto player = SpawnPlayer(mMagic, playerID);
				player->GetComponent<Player>()->SetSoulType(playerSoulType);
				break;
			}
			case 3:
			{
				auto player = SpawnPlayer(mArcher, playerID);
				player->GetComponent<Player>()->SetSoulType(playerSoulType);
				break;
			}
			case 4:
			{
				auto player = SpawnPlayer(mWarrior, playerID);
				player->GetComponent<Player>()->SetSoulType(playerSoulType);
				break;
			}
			default:
				break;
		}
		playerID = 3;
		playerSoulType = static_cast<ESoulType>(PlayerInfoVariable::Player4SoulType);
		switch (PlayerInfoVariable::Player4State)
		{
			case 0:
			{
				auto player = SpawnPlayer(mSoul, playerID);
				player->GetComponent<Soul>()->SetSoulType(playerSoulType);
				break;
			}
			case 1:
			{
				auto player = SpawnPlayer(mKnight, playerID);
				player->GetComponent<Player>()->SetSoulType(playerSoulType);
				break;
			}
			case 2:
			{
				auto player = SpawnPlayer(mMagic, playerID);
				player->GetComponent<Player>()->SetSoulType(playerSoulType);
				break;
			}
			case 3:
			{
				auto player = SpawnPlayer(mArcher, playerID);
				player->GetComponent<Player>()->SetSoulType(playerSoulType);
				break;
			}
			case 4:
			{
				auto player = SpawnPlayer(mWarrior, playerID);
				player->GetComponent<Player>()->SetSoulType(playerSoulType);
				break;
			}
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
			mSoulManagerModule->SetPlayerType(event.player->GetComponent<fq::game_module::CharacterController>()->GetControllerID(), event.type);
		});

	mUpdatePlayerStateHandler = GetScene()->GetEventManager()->RegisterHandle<client::event::UpdatePlayerState>(
		[this](const client::event::UpdatePlayerState& event)
		{
			mSoulManagerModule->SetPlayerType(event.playerID, event.type);
		});

	mDestroyArmourHandler = GetScene()->GetEventManager()->RegisterHandle<client::event::SummonSoul>(
		[this](const client::event::SummonSoul& event)
		{
			mSoulManagerModule->SummonSoul(event.id, event.soulType, event.worldTransform, event.soulPrefab, event.isDestroyArmour);
			mSoulManagerModule->SetPlayerType(event.id, EPlayerType::ArmourDestroyed);

			if (event.isDestroyArmour)
				mSoulManagerModule->AddPlayerArmourDeathCount(event.id);
		});
}


void fq::client::GameManager::OnDestroy()
{
	GetScene()->GetEventManager()->RemoveHandle(mRegisterPlayerHandler);
	GetScene()->GetEventManager()->RemoveHandle(mUpdatePlayerStateHandler);
	GetScene()->GetEventManager()->RemoveHandle(mDestroyArmourHandler);
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
				PlayerInfoVariable::Player1SoulType = static_cast<int>(player->GetComponent<Soul>()->GetSoulType());
			}
			else
			{
				PlayerInfoVariable::Player1SoulType = static_cast<int>(playerComponent->GetSoulType());
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
				PlayerInfoVariable::Player2SoulType = static_cast<int>(player->GetComponent<Soul>()->GetSoulType());
			}
			else
			{
				PlayerInfoVariable::Player2SoulType = static_cast<int>(playerComponent->GetSoulType());
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
				PlayerInfoVariable::Player3SoulType = static_cast<int>(player->GetComponent<Soul>()->GetSoulType());
			}
			else
			{
				PlayerInfoVariable::Player3SoulType = static_cast<int>(playerComponent->GetSoulType());
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
				PlayerInfoVariable::Player4SoulType = static_cast<int>(player->GetComponent<Soul>()->GetSoulType());
			}
			else
			{
				PlayerInfoVariable::Player4SoulType = static_cast<int>(playerComponent->GetSoulType());
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

	// 임시 ( 테스트 )
	// 갑옷이 깨져 있는 상태 ( n초 간 Scene에 Player, Soul이 없기 때문에 이 값을 알고 있는 SoulManager에서 해당 타입의 값이 있으면 다음 씬에도 적용 )
	if (SoulVariable::Player1Type == EPlayerType::ArmourDestroyed)
	{
		PlayerInfoVariable::Player1State = 0;
		PlayerInfoVariable::Player1SoulType = static_cast<int>(mSoulManagerModule->GetDestroyArmourSoulType(0));
		SoulVariable::Player1Type = EPlayerType::Soul;
	}
	if (SoulVariable::Player2Type == EPlayerType::ArmourDestroyed)
	{
		PlayerInfoVariable::Player2State = 0;
		PlayerInfoVariable::Player2SoulType = static_cast<int>(mSoulManagerModule->GetDestroyArmourSoulType(1));
		SoulVariable::Player1Type = EPlayerType::Soul;
	}
	if (SoulVariable::Player3Type == EPlayerType::ArmourDestroyed)
	{
		PlayerInfoVariable::Player3State = 0;
		PlayerInfoVariable::Player3SoulType = static_cast<int>(mSoulManagerModule->GetDestroyArmourSoulType(2));
		SoulVariable::Player1Type = EPlayerType::Soul;
	}
	if (SoulVariable::Player4Type == EPlayerType::ArmourDestroyed)
	{
		PlayerInfoVariable::Player4State = 0;
		PlayerInfoVariable::Player4SoulType = static_cast<int>(mSoulManagerModule->GetDestroyArmourSoulType(3));
		SoulVariable::Player1Type = EPlayerType::Soul;
	}
}

std::shared_ptr<fq::game_module::GameObject> fq::client::GameManager::SpawnPlayer(fq::game_module::PrefabResource prefab, int index)
{
	std::shared_ptr<game_module::GameObject> newObject;

	auto instance = GetScene()->GetPrefabManager()->InstantiatePrefabResoure(prefab);
	newObject = *(instance.begin());

	GetScene()->AddGameObject(newObject);
	newObject->GetComponent<game_module::CharacterController>()->SetControllerID(index);

	DirectX::SimpleMath::Vector3 spawnPos;
	
	switch (index)
	{
		case 0:
			spawnPos = GetScene()->GetObjectByName(PlayerInfoVariable::Player1SpawnPosObject)->GetTransform()->GetLocalPosition();
			break;
		case 1:
			spawnPos = GetScene()->GetObjectByName(PlayerInfoVariable::Player2SpawnPosObject)->GetTransform()->GetLocalPosition();
			break;
		case 2:
			spawnPos = GetScene()->GetObjectByName(PlayerInfoVariable::Player3SpawnPosObject)->GetTransform()->GetLocalPosition();
			break;
		case 3:
			spawnPos = GetScene()->GetObjectByName(PlayerInfoVariable::Player4SpawnPosObject)->GetTransform()->GetLocalPosition();
			break;
		default:
			break;
	}

	newObject->GetTransform()->SetLocalPosition(spawnPos);
	return newObject;
}

