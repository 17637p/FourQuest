#include "DebugService.h"

#include "../FQGameModule/GameModule.h"
#include "../FQGameModule/TextUI.h"
#include "PlayerInfoVariable.h"
#include "GameManager.h"


std::shared_ptr<fq::game_module::Component> fq::client::DebugService::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	std::shared_ptr<DebugService> cloneMonster = std::dynamic_pointer_cast<DebugService>(clone);

	if (cloneMonster == nullptr) // ���� �����ؼ� ���纻�� �ش�
	{
		cloneMonster = game_module::ObjectPool::GetInstance()->Assign<DebugService>(*this);
	}
	else // clone�� �����͸� �����Ѵ�.
	{
		// �⺻ ���� ������ ȣ���Ѵ�.
		*cloneMonster = *this;
	}

	return cloneMonster;
}

fq::client::DebugService::DebugService()
	:mbOnDebugText(false)
	, mFPSUI(nullptr)
{
}

fq::client::DebugService::~DebugService()
{

}

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

}

void fq::client::DebugService::OnUpdate(float dt)
{
	updateInput();
	updateUI();
}

void fq::client::DebugService::SetDebug(bool val)
{
	mbOnDebugText = val;

	// FPS 
	mFPSUI->SetIsRender(mbOnDebugText);
}

void fq::client::DebugService::updateUI()
{
	if (!mbOnDebugText)
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

	// ����� ���� ǥ�� 
	if (input->IsKeyState(EKey::F5, EKeyState::Tap))
	{
		SetDebug(!mbOnDebugText);
	}

	// �÷��̾� ���� 
	if (input->IsKeyState(EKey::F6, EKeyState::Tap))
	{
	}

	// ĳ���� ����
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
