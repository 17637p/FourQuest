#include "AimAssist.h"

#include "../FQGameModule/Transform.h"

// Test
#include "../FQGameModule/Scene.h"
#include "../FQGameModule/InputManager.h"


void fq::client::AimAssist::OnTriggerEnter(const game_module::Collision& collision)
{
	mInRangeMonsters.emplace_back(collision.other);
}

void fq::client::AimAssist::OnTriggerExit(const game_module::Collision& collision)
{
	mInRangeMonsters.remove(collision.other);
}

fq::client::AimAssist::~AimAssist()
{
}

fq::client::AimAssist::AimAssist()
	:mInRangeMonsters(),
	mTheta(180)
{
}

std::shared_ptr<fq::game_module::Component> fq::client::AimAssist::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	auto cloneAssist = std::dynamic_pointer_cast<AimAssist>(clone);

	if (cloneAssist == nullptr) // 새로 생성해서 복사본을 준다
	{
		cloneAssist = game_module::ObjectPool::GetInstance()->Assign<AimAssist>(*this);
	}
	else // clone에 데이터를 복사한다.
	{
		// 기본 대입 연산자 호출한다.
		*cloneAssist = *this;
	}

	return cloneAssist;
}

void fq::client::AimAssist::SetNearMonsterDirection()
{
	auto playerTransform = GetTransform()->GetParentTransform();

	DirectX::SimpleMath::Vector3 forwardVec = playerTransform->GetLookAtVector();
	float minDistance = FLT_MAX;
	game_module::GameObject* minDistanceMonster = nullptr;
	// 충돌한 몬스터 중에서 범위 각 안에 있고 가장 가까운 몬스터 찾기 
	for (auto monster : mInRangeMonsters)
	{
		DirectX::SimpleMath::Vector3 myPos = playerTransform->GetWorldPosition();
		DirectX::SimpleMath::Vector3 monsterPos = monster->GetTransform()->GetWorldPosition();

		DirectX::SimpleMath::Vector3 playerToMonsterVec = monsterPos - myPos;

		forwardVec.Normalize();
		playerToMonsterVec.Normalize();

		float cosTheta = playerToMonsterVec.Dot(forwardVec);
		if (cosTheta >= 0) // 전방 180도 
		{
			float radian = std::acosf(cosTheta);
			float degree = radian * (180.0 / 3.1415926535f);

			if (degree <= (mTheta / 2)) 
			{
				float distance = DirectX::SimpleMath::Vector3::Distance(myPos, monsterPos);
				if (distance < minDistance)
				{
					minDistance = distance;
					minDistanceMonster = monster;
				}
			}
		}
	}

	// 회전시키기 
	if (minDistanceMonster != nullptr)
	{
		auto targetTransform = minDistanceMonster->GetComponent<game_module::Transform>();

		auto targetPos = targetTransform->GetWorldPosition();
		auto myPos = playerTransform->GetWorldPosition();

		auto directV = targetPos - myPos;
		directV.y = 0.f;
		directV.Normalize();

		constexpr float RotationSpeed = 0.1f;

		auto currentRotation = playerTransform->GetWorldRotation();
		DirectX::SimpleMath::Quaternion directionQuaternion = DirectX::SimpleMath::Quaternion::LookRotation(directV, { 0, 1, 0 });
		directionQuaternion.Normalize();
		//DirectX::SimpleMath::Quaternion result =
		//	DirectX::SimpleMath::Quaternion::Slerp(currentRotation, directionQuaternion, RotationSpeed);

		DirectX::SimpleMath::Matrix rotationMatrix = DirectX::SimpleMath::Matrix::CreateFromQuaternion(directionQuaternion);

		// UpVector가 뒤집힌 경우
		if (rotationMatrix._22 <= -0.9f)
		{
			rotationMatrix._22 = 1.f;
		}
		playerTransform->SetLocalRotationToMatrix(rotationMatrix);
	}
}

void fq::client::AimAssist::OnUpdate(float dt)
{
	// Test 용 
	auto input = GetScene()->GetInputManager();
	if (input->GetKeyState(EKey::V) == EKeyState::Tap)
	{
		SetNearMonsterDirection();
	}
}

// Collision Matrix를 설정하면 됨
		//if (monster->GetTag() == game_module::ETag::Monster ||
		//	monster->GetTag() == game_module::ETag::Spawner)

// 콜라이더 달기
// transform lookat 으로 벡터 가져오고 몬스터 위치 - 내위치 빼서 각도 가져와서 내적 하면
// 내적 으로 각도 연산 
// 
// 둘다 만족하는 모든 몬스터 벡터 만들기
// 가까운 몬스터 찾기
// 
// 플레이어 방향 회전