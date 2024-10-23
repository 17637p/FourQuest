#include "CollisionRenderChanger.h"
#include "../FQGameModule/Transform.h"
#include "../FQGameModule/StaticMeshRenderer.h"

std::shared_ptr<fq::game_module::Component> fq::client::CollisionRenderChanger::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	std::shared_ptr<CollisionRenderChanger> cloneMonster = std::dynamic_pointer_cast<CollisionRenderChanger>(clone);

	if (cloneMonster == nullptr) // 새로 생성해서 복사본을 준다
	{
		cloneMonster = game_module::ObjectPool::GetInstance()->Assign<CollisionRenderChanger>(*this);
	}
	else // clone에 데이터를 복사한다.
	{
		// 기본 대입 연산자 호출한다.
		*cloneMonster = *this;
	}

	return cloneMonster;
}

void fq::client::CollisionRenderChanger::OnStart()
{
	mMeshGameObjects.clear();
	mCollisionCount = 0;

	std::function<void(fq::game_module::GameObject*)> registerRecursive = [this, &registerRecursive](fq::game_module::GameObject* object)
		{
			if (object->GetComponent<fq::game_module::StaticMeshRenderer>() != nullptr)
			{
				this->mMeshGameObjects.push_back(object);
			}

			for (auto* child : object->GetChildren())
			{
				registerRecursive(child);
			}
		};

	// 메쉬에 계층구조 연결
	registerRecursive(GetGameObject());
}

void fq::client::CollisionRenderChanger::OnUpdate(float dt)
{
	for (auto iter = mMeshGameObjects.begin(); iter != mMeshGameObjects.end();)
	{
		auto* gameObject = *iter;
		if (gameObject->IsDestroyed())
		{
			iter = mMeshGameObjects.erase(iter);
		}
		else
		{
			++iter;
		}
	}
}

void fq::client::CollisionRenderChanger::OnTriggerEnter(const game_module::Collision& collision)
{
	if (!checkContainTag(collision.other->GetTag()))
	{
		return;
	}

	if (mCollisionCount == 0)
	{
		for (auto* gameObject : mMeshGameObjects)
		{
			auto staticMeshRenderer = gameObject->GetComponent<fq::game_module::StaticMeshRenderer>();
			if (staticMeshRenderer != nullptr)
			{
				staticMeshRenderer->SetIsRender(mbIsRender);
			}
		}
	}

	++mCollisionCount;
}

void fq::client::CollisionRenderChanger::OnTriggerExit(const game_module::Collision& collision)
{
	if (!checkContainTag(collision.other->GetTag()))
	{
		return;
	}

	--mCollisionCount;

	assert(mCollisionCount < 0);
	if (mCollisionCount == 0)
	{
		for (auto* gameObject : mMeshGameObjects)
		{
			auto staticMeshRenderer = gameObject->GetComponent<fq::game_module::StaticMeshRenderer>();
			if (staticMeshRenderer != nullptr)
			{
				staticMeshRenderer->SetIsRender(!mbIsRender);
			}
		}
	}
}

bool fq::client::CollisionRenderChanger::checkContainTag(const fq::game_module::ETag& tag) const
{
	for (auto checkedTag : mTags)
	{
		if (checkedTag.Tag == tag)
		{
			return true;
		}
	}

	return false;
}

