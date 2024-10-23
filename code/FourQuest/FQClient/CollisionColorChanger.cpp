#include "CollisionColorChanger.h"
#include "../FQGameModule/Transform.h"
#include "../FQGameModule/StaticMeshRenderer.h"

std::shared_ptr<fq::game_module::Component> fq::client::CollisionColorChanger::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	std::shared_ptr<CollisionColorChanger> cloneMonster = std::dynamic_pointer_cast<CollisionColorChanger>(clone);

	if (cloneMonster == nullptr) // 새로 생성해서 복사본을 준다
	{
		cloneMonster = game_module::ObjectPool::GetInstance()->Assign<CollisionColorChanger>(*this);
	}
	else // clone에 데이터를 복사한다.
	{
		// 기본 대입 연산자 호출한다.
		*cloneMonster = *this;
	}

	return cloneMonster;
}

void fq::client::CollisionColorChanger::OnStart()
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

void fq::client::CollisionColorChanger::OnUpdate(float dt)
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

void fq::client::CollisionColorChanger::OnTriggerEnter(const game_module::Collision& collision)
{
	if (!checkContainTag(collision.other->GetTag()))
	{
		return;
	}

	if (mCollisionCount == 0)
	{
		// 색상 변경
		for (auto* gameObject : mMeshGameObjects)
		{
			auto staticMeshRenderer = gameObject->GetComponent<fq::game_module::StaticMeshRenderer>();

			if (staticMeshRenderer != nullptr)
			{
				auto materialInstanceInfo = staticMeshRenderer->GetMaterialInstanceInfo();

				materialInstanceInfo.bUseBaseColor = mbUseBaseColor;
				materialInstanceInfo.BaseColor = mBaseColor;
				materialInstanceInfo.bUseEmissiveColor = mbUseEmissiveColor;
				materialInstanceInfo.EmissiveColor = mEmissiveColor;

				staticMeshRenderer->SetMaterialInstanceInfo(materialInstanceInfo);
			}
		}
	}

	++mCollisionCount;
}

void fq::client::CollisionColorChanger::OnTriggerExit(const game_module::Collision& collision)
{
	if (!checkContainTag(collision.other->GetTag()))
	{
		return;
	}

	--mCollisionCount;

	assert(mCollisionCount < 0);
	if (mCollisionCount == 0)
	{
		// 색상 제거
		for (auto* gameObject : mMeshGameObjects)
		{
			auto staticMeshRenderer = gameObject->GetComponent<fq::game_module::StaticMeshRenderer>();

			if (staticMeshRenderer != nullptr)
			{
				auto materialInstanceInfo = staticMeshRenderer->GetMaterialInstanceInfo();

				materialInstanceInfo.bUseBaseColor = false;
				materialInstanceInfo.bUseEmissiveColor = false;

				staticMeshRenderer->SetMaterialInstanceInfo(materialInstanceInfo);
			}
		}
	}
}

bool fq::client::CollisionColorChanger::checkContainTag(const fq::game_module::ETag& tag) const
{
	for (auto checkedTag : mTags)
	{
		if (checkedTag == tag)
		{
			return true;
		}
	}

	return false;
}

