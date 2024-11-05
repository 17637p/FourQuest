#include "DebugService.h"

#include "../FQGameModule/GameModule.h"
#include "../FQGameModule/TextUI.h"
#include "../FQGameModule/EventManager.h"

#include "PlayerInfoVariable.h"
#include "SoulVariable.h"
#include "GameManager.h"
#include "ClientEvent.h"

std::shared_ptr<fq::game_module::Component> fq::client::DebugService::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	std::shared_ptr<DebugService> cloneMonster = std::dynamic_pointer_cast<DebugService>(clone);

	if (cloneMonster == nullptr) // 새로 생성해서 복사본을 준다
	{
		cloneMonster = game_module::ObjectPool::GetInstance()->Assign<DebugService>(*this);
	}
	else // clone에 데이터를 복사한다.
	{
		// 기본 대입 연산자 호출한다.
		*cloneMonster = *this;
	}

	return cloneMonster;
}

fq::client::DebugService::DebugService()
	: mFPSUI(nullptr)
{}

fq::client::DebugService::~DebugService()
{}

void fq::client::DebugService::OnStart()
{
	auto children = GetGameObject()->GetChildren();

	for (auto& child : children)
	{
		if (child->GetName().find("FPS") != std::string::npos)
		{
			mFPSUI = child->GetComponent<game_module::TextUI>();
		}
	}

	SetDebug(OnDebugText);
}

void fq::client::DebugService::OnUpdate(float dt)
{
	updateInput();
	updateUI();
}

void fq::client::DebugService::SetDebug(bool val)
{
	OnDebugText = val;

	// FPS 
	mFPSUI->SetIsRender(OnDebugText);
}

void fq::client::DebugService::updateUI()
{
	if (!OnDebugText)
		return;

	int fps = GetScene()->GetTimeManager()->GetFPS();
	std::string sFPS = "FPS " + std::to_string(fps);
	mFPSUI->SetText(sFPS);
}

void fq::client::DebugService::updateInput()
{
	auto input = GetScene()->GetInputManager();

	if (input->IsKeyState(EKey::F1, EKeyState::Tap))
	{
		savePlayerState();
		GetScene()->GetEventManager()->FireEvent<fq::event::RequestChangeScene>({ "Scene1", true });
	}
	else if (input->IsKeyState(EKey::F2, EKeyState::Tap))
	{
		savePlayerState();
		GetScene()->GetEventManager()->FireEvent<fq::event::RequestChangeScene>({ "Scene2", true });
	}
	else if (input->IsKeyState(EKey::F3, EKeyState::Tap))
	{
		savePlayerState();
		GetScene()->GetEventManager()->FireEvent<fq::event::RequestChangeScene>({ "Scene3", true });
	}
	else if (input->IsKeyState(EKey::F4, EKeyState::Tap))
	{
		savePlayerState();
		GetScene()->GetEventManager()->FireEvent<fq::event::RequestChangeScene>({ "Scene4", true });
	}

	// 디버그 정보 표시 
	if (input->IsKeyState(EKey::F5, EKeyState::Tap))
	{
		SetDebug(!OnDebugText);
	}

	// 플레이어 무적 
	if (input->IsKeyState(EKey::F6, EKeyState::Tap))
	{
	}

	// 플레이어 부활 (영혼 상태인 플레이어)
	if (input->IsKeyState(EKey::F7, EKeyState::Tap))
	{
		resurrectAllSoul();
	}

	// 캐릭터 설정
	if (input->IsKeyState(EKey::Num1, EKeyState::Tap))
	{
		if (PlayerInfoVariable::Player1State == -1)
		{
			PlayerInfoVariable::Player1State = 0;
			PlayerInfoVariable::Player1SoulType = 0;
		}
		else
		{
			PlayerInfoVariable::Player1State = -1;
			PlayerInfoVariable::Player1SoulType = -1;
		}
	}
	if (input->IsKeyState(EKey::Num2, EKeyState::Tap))
	{
		if (PlayerInfoVariable::Player2State == -1)
		{
			PlayerInfoVariable::Player2State = 0;
			PlayerInfoVariable::Player2SoulType = 1;
		}
		else
		{
			PlayerInfoVariable::Player2State = -1;
			PlayerInfoVariable::Player2SoulType = -1;
		}
	}
	if (input->IsKeyState(EKey::Num3, EKeyState::Tap))
	{
		if (PlayerInfoVariable::Player3State == -1)
		{
			PlayerInfoVariable::Player3State = 0;
			PlayerInfoVariable::Player3SoulType = 2;
		}
		else
		{
			PlayerInfoVariable::Player3State = -1;
			PlayerInfoVariable::Player3SoulType = -1;
		}
	}
	if (input->IsKeyState(EKey::Num4, EKeyState::Tap))
	{
		if (PlayerInfoVariable::Player4State == -1)
		{
			PlayerInfoVariable::Player4State = 0;
			PlayerInfoVariable::Player4SoulType = 3;
		}
		else
		{
			PlayerInfoVariable::Player4State = -1;
			PlayerInfoVariable::Player4SoulType = -1;
		}
	}
}

void fq::client::DebugService::savePlayerState()
{
	auto gameMgr = GetScene()->GetObjectByName("GameManager");
	if (gameMgr)
	{
		gameMgr->GetComponent<GameManager>()->SavePlayerState();
	}
}

void fq::client::DebugService::resurrectAllSoul()
{
	std::vector<bool> isDeadPlayers(4, false);

	if (SoulVariable::Player1Type == EPlayerType::SoulDestoryed)
	{
		isDeadPlayers[0] = true;
		SoulVariable::Player1Type = EPlayerType::Soul;
		GetScene()->GetEventManager()->FireEvent<fq::client::event::UpdatePlayerState>(
			{ 0, EPlayerType::Soul });
	}
	if (SoulVariable::Player2Type == EPlayerType::SoulDestoryed)
	{
		isDeadPlayers[1] = true;
		SoulVariable::Player2Type = EPlayerType::Soul;
		GetScene()->GetEventManager()->FireEvent<fq::client::event::UpdatePlayerState>(
			{ 1, EPlayerType::Soul });
	}
	if (SoulVariable::Player3Type == EPlayerType::SoulDestoryed)
	{
		isDeadPlayers[2] = true;
		SoulVariable::Player3Type = EPlayerType::Soul;
		GetScene()->GetEventManager()->FireEvent<fq::client::event::UpdatePlayerState>(
			{ 2, EPlayerType::Soul });
	}
	if (SoulVariable::Player4Type == EPlayerType::SoulDestoryed)
	{
		isDeadPlayers[3] = true;
		SoulVariable::Player4Type = EPlayerType::Soul;
		GetScene()->GetEventManager()->FireEvent<fq::client::event::UpdatePlayerState>(
			{ 3, EPlayerType::Soul });
	}

	for (int i = 0; i < isDeadPlayers.size(); i++)
	{
		if (isDeadPlayers[i])
		{
			GetScene()->GetObjectByName("GameManager")->GetComponent<GameManager>()->SpawnPlayer(i);
		}
	}
}

