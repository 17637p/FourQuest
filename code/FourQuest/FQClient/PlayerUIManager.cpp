#include "PlayerUIManager.h"
#include "PlayerUI.h"

#include "../FQGameModule/Transform.h"
#include "../FQGameModule/GameModule.h"
#include "../FQGameModule/CharacterController.h"

#include "GameManager.h"
#include "Player.h"

std::shared_ptr<fq::game_module::Component> fq::client::PlayerUIManager::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	auto clonePlayerUIManager = std::dynamic_pointer_cast<PlayerUIManager>(clone);

	if (clonePlayerUIManager == nullptr) // 새로 생성해서 복사본을 준다
	{
		clonePlayerUIManager = game_module::ObjectPool::GetInstance()->Assign<PlayerUIManager>(*this);
	}
	else // clone에 데이터를 복사한다.
	{
		// 기본 대입 연산자 호출한다.
		*clonePlayerUIManager = *this;
	}

	return clonePlayerUIManager;
}

fq::client::PlayerUIManager::PlayerUIManager()
	:mPlayerUIPrefab{},
	mPlayerUIs{},
	mHeightRatio(0.90f)
{
}

fq::client::PlayerUIManager::~PlayerUIManager()
{}

void fq::client::PlayerUIManager::OnStart()
{
	mScreenManager = GetScene()->GetScreenManager();
	mGameManager = GetScene()->GetObjectByName("GameManager")->GetComponent<GameManager>();
}

void fq::client::PlayerUIManager::OnUpdate(float dt)
{
	SetPlayerUIPosition();

	int playerSize = mPlayerUIs.size();
	if (mGameManager != nullptr)
	{
		auto players = mGameManager->GetPlayers();
		if (players.size() > playerSize)
		{
			for (int i = 0; i < players.size() - playerSize; i++)
			{
				game_module::CharacterController* controller = players[i]->GetComponent<game_module::CharacterController>();
				AddPlayer(controller->GetControllerID());
			}
		}
	}

	for (int i = 0; i < mPlayerUIs.size(); i++)
	{
		mPlayerUIs[i]->GetComponent<PlayerUI>()->SetPlayer(mGameManager);
	}
}

void fq::client::PlayerUIManager::AddPlayer(int playerID)
{
	std::shared_ptr<game_module::GameObject> playerUIObject;
	bool isExist = false;
	for (int i = 0; i < mPlayerUIs.size(); i++)
	{
		if (mPlayerUIs[i]->GetComponent<PlayerUI>()->GetPlayerID() == playerID)
		{
			playerUIObject = mPlayerUIs[i];
			isExist = true;
		}
	}
	// PlayerHUD 생성
	if (!isExist)
	{
		auto instance = GetScene()->GetPrefabManager()->InstantiatePrefabResoure(mPlayerUIPrefab);
		playerUIObject = *(instance.begin());

		mPlayerUIs.push_back(playerUIObject);
		GetScene()->AddGameObject(playerUIObject);
	}

	playerUIObject->GetComponent<PlayerUI>()->SetPlayerID(playerID);
	playerUIObject->GetComponent<PlayerUI>()->SetPlayer(mGameManager);

	game_module::Transform* myTransform = GetComponent<game_module::Transform>();
	playerUIObject->GetComponent<game_module::Transform>()->SetParent(myTransform);

	// player ID 순으로 정렬
	std::sort(mPlayerUIs.begin(), mPlayerUIs.end(),
		[](std::shared_ptr<game_module::GameObject> obj, std::shared_ptr<game_module::GameObject> obj2)
		{
			return obj->GetComponent<PlayerUI>()->GetPlayerID() < obj2->GetComponent<PlayerUI>()->GetPlayerID();
		});
}

void fq::client::PlayerUIManager::DeletePlayer(int playerID)
{
	auto it = std::find_if(mPlayerUIs.begin(), mPlayerUIs.end(), 
		[playerID](std::shared_ptr<game_module::GameObject> obj)
		{
			return obj->GetComponent<PlayerUI>()->GetPlayerID() == playerID;
		});
	
	mPlayerUIs.erase(it);
}

void fq::client::PlayerUIManager::SetPlayerUIPosition()
{
	int playerSize = mPlayerUIs.size();
	int screenWidth = mScreenManager->GetFixScreenWidth();
	int screenHeight = mScreenManager->GetFixScreenHeight();
	if (playerSize == 1)
	{
		mPlayerUIs[0]->GetComponent<game_module::Transform>()->SetLocalPosition({ 
			screenWidth / (float)2,
			screenHeight* mHeightRatio, 0 });
	}
	else if (playerSize == 2)
	{
		mPlayerUIs[0]->GetComponent<game_module::Transform>()->SetLocalPosition({
			screenWidth / (float)3,
			screenHeight* mHeightRatio, 0 });
		mPlayerUIs[1]->GetComponent<game_module::Transform>()->SetLocalPosition({
			screenWidth / (float)3 * 2,
			screenHeight * mHeightRatio, 0 });
	}
	else if (playerSize == 3)
	{
		mPlayerUIs[0]->GetComponent<game_module::Transform>()->SetLocalPosition({
			screenWidth / (float)4,
			screenHeight* mHeightRatio, 0 });
		mPlayerUIs[1]->GetComponent<game_module::Transform>()->SetLocalPosition({
			screenWidth / (float)4 * 2,
			screenHeight* mHeightRatio, 0 });
		mPlayerUIs[2]->GetComponent<game_module::Transform>()->SetLocalPosition({
			screenWidth / (float)4 * 3,
			screenHeight* mHeightRatio, 0 });
	}
	else if (playerSize == 4)
	{
		mPlayerUIs[0]->GetComponent<game_module::Transform>()->SetLocalPosition({
			screenWidth / (float)5,
			screenHeight* mHeightRatio, 0 });
		mPlayerUIs[1]->GetComponent<game_module::Transform>()->SetLocalPosition({
			screenWidth / (float)5 * 2,
			screenHeight* mHeightRatio, 0 });
		mPlayerUIs[2]->GetComponent<game_module::Transform>()->SetLocalPosition({
			screenWidth / (float)5 * 3,
			screenHeight * mHeightRatio, 0 });
		mPlayerUIs[3]->GetComponent<game_module::Transform>()->SetLocalPosition({
			screenWidth / (float)5 * 4,
			screenHeight* mHeightRatio, 0 });
	}
}
