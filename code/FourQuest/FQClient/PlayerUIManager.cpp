#include "PlayerUIManager.h"
#include "PlayerUI.h"

std::shared_ptr<fq::game_module::Component> fq::client::PlayerUIManager::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	auto clonePlayerUIManager = std::dynamic_pointer_cast<PlayerUIManager>(clone);

	if (clonePlayerUIManager == nullptr) // ���� �����ؼ� ���纻�� �ش�
	{
		clonePlayerUIManager = game_module::ObjectPool::GetInstance()->Assign<PlayerUIManager>(*this);
	}
	else // clone�� �����͸� �����Ѵ�.
	{
		// �⺻ ���� ������ ȣ���Ѵ�.
		*clonePlayerUIManager = *this;
	}

	return clonePlayerUIManager;
}

fq::client::PlayerUIManager::PlayerUIManager()
	:mPlayerUIPrefab{},
	mPlayerUIs{},
	mHeightRatio(0.85f)
{
}

fq::client::PlayerUIManager::~PlayerUIManager()
{

}

void fq::client::PlayerUIManager::OnStart()
{
	mScreenManager = GetScene()->GetScreenManager();
}

void fq::client::PlayerUIManager::OnUpdate(float dt)
{
	SetPlayerUIPosition();
}

void fq::client::PlayerUIManager::AddPlayer(int playerID)
{
	// ���� ������ �������ϴ� ���䵵 �׷���...
	// PlayerHUD ����
	auto instance = GetScene()->GetPrefabManager()->InstantiatePrefabResoure(mPlayerUIPrefab);
	auto playerUIObject = *(instance.begin());
	playerUIObject->GetComponent<PlayerUI>()->SetPlayerID(playerID);

	mPlayerUIs.push_back(playerUIObject);
	GetScene()->AddGameObject(playerUIObject);

	game_module::Transform* myTransform = GetComponent<game_module::Transform>();
	playerUIObject->GetComponent<game_module::Transform>()->SetParent(myTransform);

	// player ID ������ ����
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
	if (playerSize == 1)
	{
		mPlayerUIs[0]->GetComponent<game_module::Transform>()->SetLocalPosition({ 
			mScreenManager->GetScreenWidth() / (float)2, 
			mScreenManager->GetScreenHeight() * mHeightRatio, 0 });
	}
	else if (playerSize == 2)
	{
		mPlayerUIs[0]->GetComponent<game_module::Transform>()->SetLocalPosition({
			mScreenManager->GetScreenWidth() / (float)3,
			mScreenManager->GetScreenHeight() * mHeightRatio, 0 });
		mPlayerUIs[1]->GetComponent<game_module::Transform>()->SetLocalPosition({
			mScreenManager->GetScreenWidth() / (float)3 * 2,
			mScreenManager->GetScreenHeight() * mHeightRatio, 0 });
	}
	else if (playerSize == 3)
	{
		mPlayerUIs[0]->GetComponent<game_module::Transform>()->SetLocalPosition({
			mScreenManager->GetScreenWidth() / (float)4,
			mScreenManager->GetScreenHeight() * mHeightRatio, 0 });
		mPlayerUIs[1]->GetComponent<game_module::Transform>()->SetLocalPosition({
			mScreenManager->GetScreenWidth() / (float)4 * 2,
			mScreenManager->GetScreenHeight() * mHeightRatio, 0 });
		mPlayerUIs[2]->GetComponent<game_module::Transform>()->SetLocalPosition({
			mScreenManager->GetScreenWidth() / (float)4 * 3,
			mScreenManager->GetScreenHeight() * mHeightRatio, 0 });
	}
	else if (playerSize == 4)
	{
		mPlayerUIs[0]->GetComponent<game_module::Transform>()->SetLocalPosition({
			mScreenManager->GetScreenWidth() / (float)5,
			mScreenManager->GetScreenHeight() * mHeightRatio, 0 });
		mPlayerUIs[1]->GetComponent<game_module::Transform>()->SetLocalPosition({
			mScreenManager->GetScreenWidth() / (float)5 * 2,
			mScreenManager->GetScreenHeight() * mHeightRatio, 0 });
		mPlayerUIs[2]->GetComponent<game_module::Transform>()->SetLocalPosition({
			mScreenManager->GetScreenWidth() / (float)5 * 3,
			mScreenManager->GetScreenHeight() * mHeightRatio, 0 });
		mPlayerUIs[3]->GetComponent<game_module::Transform>()->SetLocalPosition({
			mScreenManager->GetScreenWidth() / (float)5 * 4,
			mScreenManager->GetScreenHeight() * mHeightRatio, 0 });
	}
}
