#include "Box.h"

#include "../FQGameModule/Transform.h"

#include "../FQGameModule/Scene.h"
#include "../FQGameModule/PrefabManager.h"
#include "../FQGameModule/Animator.h"
#include "../FQGameModule/AnimatorController.h"
#include "../FQGameModule/StaticMeshRenderer.h"

namespace fq::client
{
	Box::Box()
		: mBrokenBoxPrefebPath()
		, bIsBlock(false)
		, mRotation()
	{
	}

	Box::~Box()
	{
	}

	void Box::OnStart()
	{

	}

	void Box::OnTriggerEnter(const fq::game_module::Collision& collision)
	{
		if (!bIsBlock)
		{
			bIsBlock = true;
			mRotation = collision.other->GetRootObject()->GetTransform()->GetWorldRotation().ToEuler();

			// 기존 오브젝트의 스태틱 메시는 그리지 않는다. 
			auto staticMesh = GetComponent<fq::game_module::StaticMeshRenderer>();
			auto transform = GetComponent<fq::game_module::Transform>();
			auto worldPosition = transform->GetWorldPosition();

			if (staticMesh != nullptr)
			{
				staticMesh->SetIsRender(false);
			}

			for (const auto& object : collision.object->GetChildren())
			{
				auto childStaticMesh = object->GetComponent<fq::game_module::StaticMeshRenderer>();

				if (childStaticMesh != nullptr)
				{
					childStaticMesh->SetIsRender(false);
				}
			}

			// 부서지는 항아리 오브젝트를 생성하고 위치를 해당 위치로 세팅
			auto objects = GetScene()->GetPrefabManager()->LoadPrefab(mBrokenBoxPrefebPath);
			assert(!objects.empty());

			for (const auto& object : objects)
			{
				if (object->GetParent() == nullptr)
				{
					mObject = object;
					auto objectTransform = object->GetComponent<fq::game_module::Transform>();

					objectTransform->SetWorldMatrix(transform->GetWorldMatrix());
					objectTransform->SetWorldScale(transform->GetWorldScale() * 100.f);

					GetScene()->AddGameObject(object);
				}

				auto prefabObjectMesh = object->GetComponent<fq::game_module::StaticMeshRenderer>();

				if (prefabObjectMesh != nullptr)
					prefabObjectMesh->SetIsStatic(false);
			}
		}
	}

	void Box::OnFixedUpdate(float dt)
	{
		if (bIsBlock)
		{
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

		return cloneController;
	}

	entt::meta_handle Box::GetHandle()
	{
		return *this;
	}
}
