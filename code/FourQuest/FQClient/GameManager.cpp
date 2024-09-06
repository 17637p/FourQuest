#include "GameManager.h"

#include "../FQGameModule/GameModule.h"
#include "ClientEvent.h"
#include "Player.h"

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
	:mIsStop(false)
	//	:mRegisterPlayerHandler{}
{}

fq::client::GameManager::GameManager(const GameManager& other)
	:mRegisterPlayerHandler{}
	, mPlayers(other.mPlayers)
	, mPauseUI(other.mPauseUI)
	, mIsStop(false)
{}

fq::client::GameManager& fq::client::GameManager::operator=(const GameManager& other)
{
	mPlayers = other.mPlayers;
	mPauseUI = other.mPauseUI;
	mIsStop = false;

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
			playerComponent->SetHp(100000.f);
		}
	}

	if (input->IsKeyState(EKey::F2, EKeyState::Tap))
	{
		GetScene()->GetEventManager()->FireEvent<fq::event::RequestChangeScene>({ "Scene2", true});
	}
	if (input->IsKeyState(EKey::F3, EKeyState::Tap))
	{
		GetScene()->GetEventManager()->FireEvent<fq::event::RequestChangeScene>({ "Scene3", true });
	}
	if (input->IsKeyState(EKey::F4, EKeyState::Tap))
	{
		GetScene()->GetEventManager()->FireEvent<fq::event::RequestChangeScene>({ "Scene4", true });	
	}
}



