#include "MonsterManager.h"

#include "../FQGameModule/GameModule.h"

fq::client::MonsterManager::MonsterManager()
	:mOnDestroyHandler{}
	,mAddGameObjectHandler{}
{}


fq::client::MonsterManager::MonsterManager(const MonsterManager& other)
	:mOnDestroyHandler{}
	,mAddGameObjectHandler{}
	,mRes{}
{}

fq::client::MonsterManager& fq::client::MonsterManager::operator=(const MonsterManager& other)
{
	return *this;
}

fq::client::MonsterManager::~MonsterManager()
{}

std::shared_ptr<fq::game_module::Component> fq::client::MonsterManager::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	auto cloneMgr = std::dynamic_pointer_cast<MonsterManager>(clone);

	if (cloneMgr == nullptr) // ���� �����ؼ� ���纻�� �ش�
	{
		cloneMgr = game_module::ObjectPool::GetInstance()->Assign<MonsterManager>(*this);
	}
	else // clone�� �����͸� �����Ѵ�.
	{
		// �⺻ ���� ������ ȣ���Ѵ�.
		*cloneMgr = *this;
	}

	cloneMgr->mMonsters.clear();

	return cloneMgr;
}

void fq::client::MonsterManager::OnAwake()
{
	mMonsters.reserve(255);

	auto eventMgr = GetScene()->GetEventManager();

	// ���� ���� �̺�Ʈ ó��
	mOnDestroyHandler = eventMgr->RegisterHandle<fq::event::OnDestoryedGameObject>([this](const fq::event::OnDestoryedGameObject& event)
		{
			if (event.object->GetTag() != game_module::ETag::Monster)
				return;

			auto id = event.object->GetID();

			mMonsters.erase(std::remove_if(mMonsters.begin(), mMonsters.end()
				, [id](const std::shared_ptr<game_module::GameObject>& object)
				{
					return id == object->GetID();
				}), mMonsters.end());
		});

	// ���� �߰� �̺�Ʈ ó��
	mAddGameObjectHandler = eventMgr->RegisterHandle<fq::event::AddGameObject>([this](const fq::event::AddGameObject& event)
		{
			if (event.object->GetTag() == game_module::ETag::Monster)
			{
				mMonsters.push_back(event.object->shared_from_this());
			}
		});

	// Scene ��ġ�� ���� �߰�
	for (auto& object : GetScene()->GetObjectView())
	{
		if (object.GetTag() == game_module::ETag::Monster)
		{
			mMonsters.push_back(object.shared_from_this());
		}
	}
}

void fq::client::MonsterManager::OnDestroy()
{
	GetScene()->GetEventManager()->RemoveHandle(mOnDestroyHandler);
	GetScene()->GetEventManager()->RemoveHandle(mAddGameObjectHandler);
	mMonsters.clear();
}
