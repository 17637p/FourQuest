#include "ArmourSpawner.h"

#include "../FQGameModule/Transform.h"
#include "../FQGameModule/Scene.h"
#include "../FQGameModule/PrefabManager.h"
#include "../FQGameModule/NavigationAgent.h"
#include "../FQGameModule/MaterialAnimator.h"

#include "SettingVariable.h"

#include <random>
#include <chrono>

// test
#include "../FQGameModule/InputManager.h"

std::shared_ptr<fq::game_module::Component> fq::client::ArmourSpawner::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	std::shared_ptr<ArmourSpawner> cloneArmourSpawn = std::dynamic_pointer_cast<ArmourSpawner>(clone);

	if (cloneArmourSpawn == nullptr) // ���� �����ؼ� ���纻�� �ش�
	{
		cloneArmourSpawn = game_module::ObjectPool::GetInstance()->Assign<ArmourSpawner>(*this);
	}
	else // clone�� �����͸� �����Ѵ�.
	{
		// �⺻ ���� ������ ȣ���Ѵ�.
		*cloneArmourSpawn = *this;
	}

	return cloneArmourSpawn;
}

fq::client::ArmourSpawner::ArmourSpawner()
{
}

fq::client::ArmourSpawner::~ArmourSpawner()
{
}

void fq::client::ArmourSpawner::SpawnArmour()
{
	// ���� ��ȯ
	std::shared_ptr<game_module::GameObject> armourObject;

	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	std::mt19937 generator(seed);
	std::uniform_int_distribution<int> randomArmourDistribution(0, mArmourList.size() - 1);
	int randomArmour = randomArmourDistribution(generator);

	game_module::PrefabResource armour = mArmourList[randomArmour];

	//for (int aaa = 0; aaa < 1000; aaa++) ������
	{
		auto instance = GetScene()->GetPrefabManager()->InstantiatePrefabResoure(armour);
		armourObject = *(instance.begin());

		GetScene()->AddGameObject(armourObject);

		/// ���� ��ġ ����
		// ī�޶� �߽� ��������
		DirectX::SimpleMath::Vector3 center = GetTransform()->GetWorldPosition();
		DirectX::SimpleMath::Vector3 nearPos = { 0, 0, 0 };

		// ���� ��ġ ����
		unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
		std::mt19937 generator(seed);
		std::uniform_real_distribution<float> distanceDistribution(0, SettingVariable::ArmourSpawnDistance);
		std::uniform_real_distribution<float> degreeDistribution(0, 360);

		float randomDistance = distanceDistribution(generator);
		float randomDegree = degreeDistribution(generator);

		float radian = randomDegree * 3.1415926535f / 180.0f;

		center.x += std::cosf(radian) * randomDistance;
		center.z += std::sinf(radian) * randomDistance;

		// ��ȿ�� ��ġ���� Ȯ��
		int count = 0;
		bool isValid = false;
		while (!isValid && count < 100)
		{
			isValid = GetScene()->GetObjectByName("QuestUI")->GetComponent<game_module::NavigationAgent>()->IsValid(center, nearPos);
			count++;
		}
		nearPos.y += 1.0f;
		armourObject->GetComponent<game_module::Transform>()->SetLocalPosition(nearPos);

		// ������Ʈ ���
		for (auto child : armourObject->GetChildren())
		{
			auto materialAnimator = child->GetComponent<fq::game_module::MaterialAnimator>();

			if (materialAnimator == nullptr)
			{
				continue;
			}

			auto info = materialAnimator->GetDissolveAnimatorInfo();
			info.bIsUsed = true;
			materialAnimator->SetDissolveAnimatorInfo(info);
		}
	}
}

void fq::client::ArmourSpawner::OnUpdate(float dt)
{
	/// �׽�Ʈ �� 
	//auto input = GetScene()->GetInputManager();
	//if (input->GetKeyState(EKey::S) == EKeyState::Tap)
	//{
	//	SpawnArmour();
	//}
}

void fq::client::ArmourSpawner::SpawnArmourAll()
{
	// ���� ��ȯ
	std::shared_ptr<game_module::GameObject> armourObject;

	for (const auto& armour : mArmourList)
	{
		auto instance = GetScene()->GetPrefabManager()->InstantiatePrefabResoure(armour);
		armourObject = *(instance.begin());

		GetScene()->AddGameObject(armourObject);

		/// ���� ��ġ ����
		// ī�޶� �߽� ��������
		DirectX::SimpleMath::Vector3 center = GetTransform()->GetWorldPosition();
		DirectX::SimpleMath::Vector3 nearPos = { 0, 0, 0 };

		// ���� ��ġ ����
		unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
		std::mt19937 generator(seed);
		std::uniform_real_distribution<float> distanceDistribution(0, SettingVariable::ArmourSpawnDistance);
		std::uniform_real_distribution<float> degreeDistribution(0, 360);

		float randomDistance = distanceDistribution(generator);
		float randomDegree = degreeDistribution(generator);

		float radian = randomDegree * 3.1415926535f / 180.0f;

		center.x += std::cosf(radian) * randomDistance;
		center.z += std::sinf(radian) * randomDistance;

		// ��ȿ�� ��ġ���� Ȯ��
		int count = 0;
		bool isValid = false;
		while (!isValid && count < 100)
		{
			isValid = GetScene()->GetObjectByName("QuestUI")->GetComponent<game_module::NavigationAgent>()->IsValid(center, nearPos);
			count++;
		}
		nearPos.y += 1.0f;
		armourObject->GetComponent<game_module::Transform>()->SetLocalPosition(nearPos);

		// ������Ʈ ���
		for (auto child : armourObject->GetChildren())
		{
			auto materialAnimator = child->GetComponent<fq::game_module::MaterialAnimator>();

			if (materialAnimator == nullptr)
			{
				continue;
			}

			auto info = materialAnimator->GetDissolveAnimatorInfo();
			info.bIsUsed = true;
			materialAnimator->SetDissolveAnimatorInfo(info);
		}
	}
}

void fq::client::ArmourSpawner::OnStart()
{
	// �ƸӸ���Ʈ ����ִ� ��� ƨ�� ����
	mArmourList.erase(std::remove_if(mArmourList.begin(), mArmourList.end(), [this](game_module::PrefabResource res)
		{
			if (res.GetPrefabPath().empty())
			{
				spdlog::warn("armour prefanResource empty {}", this->GetGameObject()->GetName());
				return true;
			}

			return false;
		}), mArmourList.end());
}
