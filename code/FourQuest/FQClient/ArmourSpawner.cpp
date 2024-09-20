#include "ArmourSpawner.h"

#include "../FQGameModule/Transform.h"
#include "../FQGameModule/Scene.h"
#include "../FQGameModule/PrefabManager.h"
#include "../FQGameModule/NavigationAgent.h"

#include "SettingVariable.h"

#include <random>
#include <chrono>

// test
#include "../FQGameModule/InputManager.h"

std::shared_ptr<fq::game_module::Component> fq::client::ArmourSpawner::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	std::shared_ptr<ArmourSpawner> cloneArmourSpawn = std::dynamic_pointer_cast<ArmourSpawner>(clone);

	if (cloneArmourSpawn == nullptr) // 새로 생성해서 복사본을 준다
	{
		cloneArmourSpawn = game_module::ObjectPool::GetInstance()->Assign<ArmourSpawner>(*this);
	}
	else // clone에 데이터를 복사한다.
	{
		// 기본 대입 연산자 호출한다.
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
	// 갑옷 소환
	std::shared_ptr<game_module::GameObject> armourObject;

	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	std::mt19937 generator(seed);
	std::uniform_int_distribution<int> randomArmourDistribution(0, mArmourList.size() - 1);
	int randomArmour = randomArmourDistribution(generator);

	game_module::PrefabResource armour = mArmourList[randomArmour];

	//for (int aaa = 0; aaa < 1000; aaa++) 디버깅용
	{
		auto instance = GetScene()->GetPrefabManager()->InstantiatePrefabResoure(armour);
		armourObject = *(instance.begin());

		GetScene()->AddGameObject(armourObject);

		/// 갑옷 위치 설정
		// 카메라 중심 가져오기
		DirectX::SimpleMath::Vector3 center = GetTransform()->GetWorldPosition();
		DirectX::SimpleMath::Vector3 nearPos = { 0, 0, 0 };

		// 랜덤 위치 설정
		unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
		std::mt19937 generator(seed);
		std::uniform_real_distribution<float> distanceDistribution(0, SettingVariable::ArmourSpawnDistance);
		std::uniform_real_distribution<float> degreeDistribution(0, 360);

		float randomDistance = distanceDistribution(generator);
		float randomDegree = degreeDistribution(generator);

		float radian = randomDegree * 3.1415926535f / 180.0f;

		center.x += std::cosf(radian) * randomDistance;
		center.z += std::sinf(radian) * randomDistance;

		// 유효한 위치인지 확인
		int count = 0;
		bool isValid = false;
		while (!isValid && count < 100)
		{
			isValid = GetScene()->GetObjectByName("QuestUI")->GetComponent<game_module::NavigationAgent>()->IsValid(center, nearPos);
			count++;
		}
		nearPos.y += 1.0f;
		armourObject->GetComponent<game_module::Transform>()->SetLocalPosition(nearPos);
	}
}

void fq::client::ArmourSpawner::OnUpdate(float dt)
{
	/// 테스트 용 
	//auto input = GetScene()->GetInputManager();
	//if (input->GetKeyState(EKey::S) == EKeyState::Tap)
	//{
	//	SpawnArmour();
	//}
}
