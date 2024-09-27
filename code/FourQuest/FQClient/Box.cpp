#include "Box.h"

#include "../FQGameModule/Transform.h"

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
		, bIsBlock(false)
		, mRotation()
		, mDeadTime(1.f)
		, mDurationTime(0.f)
	{
	}

	Box::~Box()
	{
	}

	void Box::OnStart()
	{
		auto transform = GetComponent<fq::game_module::Transform>();

		// �μ����� �׾Ƹ� ������Ʈ�� �����ϰ� ��ġ�� �ش� ��ġ�� ����
		auto objects = GetScene()->GetPrefabManager()->LoadPrefab(mBrokenBoxPrefebPath);
		assert(!objects.empty());

		for (const auto& object : objects)
		{
			if (object->GetParent() == nullptr)
			{
				mObject = object;

				auto objectTransform = object->GetComponent<fq::game_module::Transform>();

				objectTransform->SetWorldMatrix(transform->GetWorldMatrix());
				objectTransform->SetWorldScale(transform->GetWorldScale());

				GetScene()->AddGameObject(object);
			}

			auto prefabObjectMesh = object->GetComponent<fq::game_module::StaticMeshRenderer>();

			if (prefabObjectMesh != nullptr)
			{
				prefabObjectMesh->SetIsRender(false);
				prefabObjectMesh->SetIsStatic(false);
			}
		}
	}

	void Box::OnTriggerEnter(const fq::game_module::Collision& collision)
	{
		if (collision.other->GetTag() != fq::game_module::ETag::PlayerAttack)
			return;

		if (!bIsBlock)
		{
			bIsBlock = true;
			mRotation = collision.other->GetRootObject()->GetTransform()->GetWorldRotation().ToEuler();

			// ���� ������Ʈ�� ����ƽ �޽ô� �׸��� �ʰ� �ݸ��� ����
			auto staticMesh = GetComponent<fq::game_module::StaticMeshRenderer>();
			GetGameObject()->RemoveComponent<fq::game_module::RigidBody>();
			GetGameObject()->RemoveComponent<fq::game_module::BoxCollider>();

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

			// �μ����� �׾Ƹ� ������Ʈ�� �����ϰ� ��ġ�� �ش� ��ġ�� ����
			for (const auto& object : mObject->GetChildren())
			{
				auto prefabObjectMesh = object->GetComponent<fq::game_module::StaticMeshRenderer>();

				if (prefabObjectMesh != nullptr)
					prefabObjectMesh->SetIsRender(true);
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

			// ������ ������ ��� ����� ��ȯ
			if (normalizedAngle < 0) {
				normalizedAngle += 360.0f;
			}

			animator->SetParameterTrigger("Idle");

			//// ���� ������ ���� 8������ �����մϴ�.
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

				// ������ ���� ������ ������Ʈ�� �ִٸ� ���� ����
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

		if (cloneController == nullptr) // ���� �����ؼ� ���纻�� �ش�
		{
			cloneController = game_module::ObjectPool::GetInstance()->Assign<Box>(*this);
		}
		else // clone�� �����͸� �����Ѵ�.
		{
			// �⺻ ���� ������ ȣ���Ѵ�.
			*cloneController = *this;
		}

		return cloneController;
	}

	entt::meta_handle Box::GetHandle()
	{
		return *this;
	}
}
