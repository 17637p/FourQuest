#include "MultiPlayerObjectCreator.h"

#include "../FQGameModule/PrefabManager.h"
#include "../FQGameModule/Scene.h"
#include "../FQGameModule/Transform.h"

#include "PlayerInfoVariable.h"
#include "SettingVariable.h"

#include <random>
#include <chrono>

std::shared_ptr<fq::game_module::Component> fq::client::MultiPlayerObjectCreator::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	std::shared_ptr<MultiPlayerObjectCreator> cloneMultiPlayerObjectCreator = std::dynamic_pointer_cast<MultiPlayerObjectCreator>(clone);

	if (cloneMultiPlayerObjectCreator == nullptr) // ���� �����ؼ� ���纻�� �ش�
	{
		cloneMultiPlayerObjectCreator = game_module::ObjectPool::GetInstance()->Assign<MultiPlayerObjectCreator>(*this);
	}
	else // clone�� �����͸� �����Ѵ�.
	{
		// �⺻ ���� ������ ȣ���Ѵ�.
		*cloneMultiPlayerObjectCreator = *this;
	}

	return cloneMultiPlayerObjectCreator;
}

fq::client::MultiPlayerObjectCreator::MultiPlayerObjectCreator()
	:mNum(0),
	mSpawnObjectPrefab()
{
}

fq::client::MultiPlayerObjectCreator::~MultiPlayerObjectCreator()
{
}

void fq::client::MultiPlayerObjectCreator::OnStart()
{
	spawn();
}

void fq::client::MultiPlayerObjectCreator::spawn()
{
	int numberOfPlayer = 0;
	if (PlayerInfoVariable::Player1SoulType != -1)
	{
		numberOfPlayer++;
	}
	if (PlayerInfoVariable::Player2SoulType != -1)
	{
		numberOfPlayer++;
	}
	if (PlayerInfoVariable::Player3SoulType != -1)
	{
		numberOfPlayer++;
	}
	if (PlayerInfoVariable::Player4SoulType != -1)
	{
		numberOfPlayer++;
	}

	// �÷��� ���� �ο��� ��� ���� ��ȯ 
	if (numberOfPlayer < mNum)
	{
		for (int i = 0; i < numberOfPlayer; i++)
		{
			std::shared_ptr<game_module::GameObject> spawnObject;
			auto instance = GetScene()->GetPrefabManager()->InstantiatePrefabResoure(mSpawnObjectPrefab);
			spawnObject = *(instance.begin());
			GetScene()->AddGameObject(spawnObject);

			// ���� ��ġ ����
			auto myPos = GetTransform()->GetWorldPosition();

			unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
			std::mt19937 generator(seed);
			std::uniform_real_distribution<float> distanceDistribution(0, 1.5f);
			std::uniform_real_distribution<float> degreeDistribution(0, 360);

			float randomDistance = distanceDistribution(generator);
			float randomDegree = degreeDistribution(generator);

			float radian = randomDegree * 3.1415926535f / 180.0f;

			myPos.x += std::cosf(radian) * randomDistance;
			myPos.z += std::sinf(radian) * randomDistance;

			spawnObject->GetTransform()->SetWorldPosition(myPos);
		}
	}
}
