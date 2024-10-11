#include "MaterialManagedGroup.h"
#include "ClientEvent.h"
#include "GameManager.h"
#include "../FQGameModule/StaticMeshRenderer.h"
#include "../FQGameModule/Camera.h"
#include "../FQGameModule/Transform.h"
#include "../FQGameModule/CharacterController.h"
#include "../FQCommon/FQCommonGraphics.h"

fq::client::MaterialManagedGroup::MaterialManagedGroup()
	:mbUsed(false)
{
	mStaticMeshObjects.reserve(256);
}

fq::client::MaterialManagedGroup::MaterialManagedGroup(const MaterialManagedGroup& other)
	: mStaticMeshObjects(other.mStaticMeshObjects)
	, mbUsed(other.mbUsed)
{
}

fq::client::MaterialManagedGroup& fq::client::MaterialManagedGroup::operator=(const MaterialManagedGroup& other)
{
	if (this == &other)
	{
		return *this;
	}

	mStaticMeshObjects = other.mStaticMeshObjects;
	mbUsed = other.mbUsed;

	return *this;
}


std::shared_ptr<fq::game_module::Component> fq::client::MaterialManagedGroup::Clone(std::shared_ptr<fq::game_module::Component> clone) const
{
	auto cloned = std::dynamic_pointer_cast<MaterialManagedGroup>(clone);

	if (cloned == nullptr) // 새로 생성해서 복사본을 준다
	{
		cloned = game_module::ObjectPool::GetInstance()->Assign<MaterialManagedGroup>(*this);
	}
	else // clone에 데이터를 복사한다.
	{
		// 기본 대입 연산자 호출한다.
		*cloned = *this;
	}

	return cloned;
}

void fq::client::MaterialManagedGroup::OnStart()
{
	mStaticMeshObjects.clear();

	std::function<void(fq::game_module::GameObject*)> registerRecursive = [this, &registerRecursive](fq::game_module::GameObject* object)
		{
			if (GetComponent<fq::game_module::StaticMeshRenderer>() != nullptr)
			{
				this->mStaticMeshObjects.push_back(object);
			}

			for (auto* child : object->GetChildren())
			{
				registerRecursive(child);
			}
		};

	// 메쉬에 계층구조 연결
	registerRecursive(GetGameObject());
}

