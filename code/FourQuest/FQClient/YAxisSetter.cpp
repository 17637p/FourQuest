#define NOMINMAX
#include "YAxisSetter.h"

#include "../FQGameModule/GameModule.h"
#include "../FQGameModule/Transform.h"
#include "../FQGameModule/RigidBody.h"
#include "../FQGameModule/SphereCollider.h"

namespace fq::client
{
	YAxisSetter::YAxisSetter()
		: mOffsetYAxis(1.f)
		, mDistance(10.f)
	{
	}

	YAxisSetter::~YAxisSetter()
	{
	}

	void YAxisSetter::OnFixedUpdate(float dt)
	{
		//auto transform = GetComponent<fq::game_module::Transform>();
		//auto rigidBody = GetComponent<fq::game_module::RigidBody>();

		//DirectX::SimpleMath::Vector3 origin = transform->GetWorldPosition() + DirectX::SimpleMath::Vector3(0.f, mOffsetYAxis, 0.f);
		//DirectX::SimpleMath::Vector3 direction = DirectX::SimpleMath::Vector3(0.f, -1.f, 0.f);
		//fq::game_module::ETag tag = GetGameObject()->GetTag();
		//bool bUseDebugDraw = true;
		//bool isStatic = true;

		//fq::event::RayCast::ResultData data;

		//GetScene()->GetEventManager()->FireEvent<fq::event::RayCast>(
		//	fq::event::RayCast {origin, direction, mDistance, tag, & data, bUseDebugDraw, isStatic}
		//);

		//if (data.hitCount > 0)
		//{
		//	if (!(data.hitObjects[0]->GetTag() == fq::game_module::ETag::Floor))
		//		return;

		//	DirectX::SimpleMath::Vector3 contactPoint = data.hitContactPoints[0];
		//	float addYAxisValue = contactPoint.y - transform->GetWorldPosition().y;

		//	transform->AddLocalPosition(DirectX::SimpleMath::Vector3(0.f, addYAxisValue, 0.f));
		//}
	}

	void YAxisSetter::OnCollisionEnter(const fq::game_module::Collision& collision)
	{
		auto transform = GetComponent<fq::game_module::Transform>();
		auto shape = GetComponent<fq::game_module::SphereCollider>();

		if (collision.other->GetTag() == fq::game_module::ETag::Floor)
		{
			for (auto point : collision.contactPoints)
			{
				if (point.y == 0.f)
					return;

				float addYAxisValue = point.y - transform->GetWorldPosition().y + shape->GetRadius();

				transform->AddLocalPosition(DirectX::SimpleMath::Vector3(0.f, addYAxisValue, 0.f));
			}
		}
	}

	std::shared_ptr<fq::game_module::Component> YAxisSetter::Clone(std::shared_ptr<Component> clone) const
	{
		auto yAxisSetter = std::dynamic_pointer_cast<YAxisSetter>(clone);

		if (yAxisSetter == nullptr) // 새로 생성해서 복사본을 준다
		{
			yAxisSetter = game_module::ObjectPool::GetInstance()->Assign<YAxisSetter>(*this);
		}
		else // clone에 데이터를 복사한다.
		{
			// 기본 대입 연산자 호출한다.
			*yAxisSetter = *this;
		}

		return yAxisSetter;
	}

	entt::meta_handle YAxisSetter::GetHandle()
	{
		return *this;
	}
}
