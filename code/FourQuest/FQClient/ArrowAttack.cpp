#include "ArrowAttack.h"

#include "../FQGameModule/Transform.h"
#include "../FQGameModule/RigidBody.h"
#include "../FQGameModule/BoxCollider.h"
#include "../FQGameModule/CapsuleCollider.h"
#include "../FQGameModule/Scene.h"

namespace fq::client
{
	ArrowAttack::ArrowAttack()
		: mbIsBlock(false)
		, mbIsStrongAttack(false)
		, mDurationTime()
		, mLifeTime(1.f)
	{
	}

	ArrowAttack::~ArrowAttack()
	{
	}

	void ArrowAttack::OnFixedUpdate(float dt)
	{
		if (mbIsBlock == true)
		{
			mDurationTime += dt;

			if (mDurationTime >= mLifeTime)
			{
				GetScene()->DestroyGameObject(GetGameObject());
			}
		}
	}

	void ArrowAttack::OnTriggerEnter(const game_module::Collision& collision)
	{
		auto transform = GetComponent<fq::game_module::Transform>();
		auto otherTransform = collision.other->GetComponent<fq::game_module::Transform>();

		transform->SetParent(otherTransform);

		GetGameObject()->RemoveComponent<fq::game_module::RigidBody>();
		GetGameObject()->RemoveComponent<fq::game_module::BoxCollider>();
		GetGameObject()->RemoveComponent<fq::game_module::CapsuleCollider>();

		mbIsBlock = true;
	}

	void ArrowAttack::OnTriggerStay(const game_module::Collision& collision)
	{

	}

	void ArrowAttack::OnTriggerExit(const game_module::Collision& collision)
	{

	}

	std::shared_ptr<fq::game_module::Component> ArrowAttack::Clone(std::shared_ptr<Component> clone) const
	{
		auto cloneArmour = std::dynamic_pointer_cast<ArrowAttack>(clone);

		if (cloneArmour == nullptr) // ���� �����ؼ� ���纻�� �ش�
		{
			cloneArmour = game_module::ObjectPool::GetInstance()->Assign<ArrowAttack>(*this);
		}
		else // clone�� �����͸� �����Ѵ�.
		{
			// �⺻ ���� ������ ȣ���Ѵ�.
			*cloneArmour = *this;
		}

		return cloneArmour;
	}
}
