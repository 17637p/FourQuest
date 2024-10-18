#include "Box.h"

#include "../FQGameModule/Transform.h"

#include <spdlog/stopwatch.h>
#include "../FQGameModule/Scene.h"
#include "../FQGameModule/PrefabManager.h"
#include "../FQGameModule/Animator.h"
#include "../FQGameModule/AnimatorController.h"
#include "../FQGameModule/StaticMeshRenderer.h"
#include "../FQGameModule/RigidBody.h"
#include "../FQGameModule/BoxCollider.h"

#include "ArmourSpawner.h"

namespace fq::client
{
	Box::Box()
		: mBrokenBoxPrefebPath()
		, mBrokenBoxPrefabPath{}
		, bIsBlock(false)
		, mRotation()
		, mDeadTime(1.f)
		, mDurationTime(0.f)
	{
	}

	Box::~Box()
	{
	}

	void Box::OnTriggerEnter(const fq::game_module::Collision& collision)
	{
		if (collision.other->GetTag() != fq::game_module::ETag::PlayerAttack)
			return;

		if (!bIsBlock)
		{
			bIsBlock = true;
			mRotation = collision.other->GetRootObject()->GetTransform()->GetWorldRotation().ToEuler();

			addbreakBoxPrefab();

			// 콜리전 삭제
			GetGameObject()->RemoveComponent<fq::game_module::RigidBody>();
			GetGameObject()->RemoveComponent<fq::game_module::BoxCollider>();
		
			// 기존 오브젝트의 스태틱 메시는 Off
			auto staticMesh = GetComponent<fq::game_module::StaticMeshRenderer>();
			if (staticMesh != nullptr)
			{
				staticMesh->SetIsRender(false);
			}
			for (auto& child : GetGameObject()->GetChildren())
			{
				auto staticMesh = child->GetComponent<game_module::StaticMeshRenderer>();
				if (staticMesh)
				{
					staticMesh->SetIsRender(false);
				}
			}
		}
	}

	void Box::OnFixedUpdate(float dt)
	{
		if (bIsBlock && mObject != nullptr)
		{
			mDurationTime += dt;

			auto animator = mObject->GetComponent<fq::game_module::Animator>();

			float radian = fmod(mRotation.y, 360.0f);;
			float normalizedAngle = radian / 3.14f * 180.f;

			// 각도가 음수일 경우 양수로 변환
			if (normalizedAngle < 0) {
				normalizedAngle += 360.0f;
			}

			animator->SetParameterTrigger("Idle");

			//// 각도 범위에 따라 8방향을 지정합니다.
			//if (normalizedAngle > 337.5f || normalizedAngle <= 22.5f) {
			//	animator->SetParameterTrigger("Up");
			//}
			//else if (normalizedAngle > 22.5f && normalizedAngle <= 67.5f) {
			//	animator->SetParameterTrigger("RightUp");
			//}
			//else if (normalizedAngle > 67.5f && normalizedAngle <= 112.5f) {
			//	animator->SetParameterTrigger("Right");
			//}
			//else if (normalizedAngle > 112.5f && normalizedAngle <= 157.5f) {
			//	animator->SetParameterTrigger("RightButtom");
			//}
			//else if (normalizedAngle > 157.5f && normalizedAngle <= 202.5f) {
			//	animator->SetParameterTrigger("Buttom");
			//}
			//else if (normalizedAngle > 202.5f && normalizedAngle <= 247.5f) {
			//	animator->SetParameterTrigger("LeftButtom");
			//}
			//else if (normalizedAngle > 247.5f && normalizedAngle <= 292.5f) {
			//	animator->SetParameterTrigger("Left");
			//}
			//else if (normalizedAngle > 292.5f && normalizedAngle <= 337.5f) {
			//	animator->SetParameterTrigger("LeftUp");
			//}

			if (mDurationTime >= mDeadTime)
			{
				GetScene()->DestroyGameObject(mObject.get());
				GetScene()->DestroyGameObject(GetGameObject());
				mObject = nullptr;

				// 깨지고 갑옷 스포너 컴포넌트가 있다면 갑옷 스폰
				auto armourSpawner = GetComponent<ArmourSpawner>();
				if (armourSpawner)
				{
					armourSpawner->SpawnArmour();
				}
			}
		}
	}

	std::shared_ptr<fq::game_module::Component> Box::Clone(std::shared_ptr<Component> clone) const
	{
		auto cloneController = std::dynamic_pointer_cast<Box>(clone);

		if (cloneController == nullptr) // 새로 생성해서 복사본을 준다
		{
			cloneController = game_module::ObjectPool::GetInstance()->Assign<Box>(*this);
		}
		else // clone에 데이터를 복사한다.
		{
			// 기본 대입 연산자 호출한다.
			*cloneController = *this;
		}

		// 이전버전 호환하기 위한 꼼수입니다. 
		cloneController->mBrokenBoxPrefabPath = cloneController->mBrokenBoxPrefebPath;

		return cloneController;
	}

	entt::meta_handle Box::GetHandle()
	{
		return *this;
	}

	void Box::addbreakBoxPrefab()
	{
		auto transform = GetComponent<fq::game_module::Transform>();

		// 부서지는 항아리 오브젝트를 생성하고 위치를 해당 위치로 세팅
		auto objects = GetScene()->GetPrefabManager()->InstantiatePrefabResoure(mBrokenBoxPrefabPath);
		assert(!objects.empty());

		mObject = objects[0];
		auto objectTransform = mObject->GetComponent<fq::game_module::Transform>();
		objectTransform->SetWorldMatrix(transform->GetWorldMatrix());
		objectTransform->SetWorldScale(transform->GetWorldScale());
		GetScene()->AddGameObject(mObject);
	}

}
