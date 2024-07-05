#include "GameManager.h"

#include "../FQGameModule/GameModule.h"
#include "ClientEvent.h"

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
//	:mRegisterPlayerHandler{}
{}

fq::client::GameManager::GameManager(const GameManager& other)
	:mRegisterPlayerHandler{}
	,mPlayers(other.mPlayers)
{
}

fq::client::GameManager& fq::client::GameManager::operator=(const GameManager& other)
{
	mPlayers = other.mPlayers;

	return *this;
}

void fq::client::GameManager::OnUpdate(float dt)
{
	mPlayers.erase(std::remove_if(mPlayers.begin(), mPlayers.end()
		, [](const std::shared_ptr<game_module::GameObject> object) 
		{
			return object->IsDestroyed();
		}), mPlayers.end());
}

void fq::client::GameManager::OnStart()
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

}

