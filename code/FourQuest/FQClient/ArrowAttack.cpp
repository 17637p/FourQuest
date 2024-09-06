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

		if (cloneArmour == nullptr) // 새로 생성해서 복사본을 준다
		{
			cloneArmour = game_module::ObjectPool::GetInstance()->Assign<ArrowAttack>(*this);
		}
		else // clone에 데이터를 복사한다.
		{
			// 기본 대입 연산자 호출한다.
			*cloneArmour = *this;
		}

		return cloneArmour;
	}
}
