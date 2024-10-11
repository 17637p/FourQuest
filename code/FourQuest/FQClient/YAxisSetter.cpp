#include "YAxisSetter.h"

#include "../FQGameModule/GameModule.h"
#include "../FQGameModule/Transform.h"

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
		auto transform = GetComponent<fq::game_module::Transform>();

		DirectX::SimpleMath::Vector3 origin = transform->GetWorldPosition() + DirectX::SimpleMath::Vector3(0.f, mOffsetYAxis, 0.f);
		DirectX::SimpleMath::Vector3 direction = DirectX::SimpleMath::Vector3(0.f, -1.f, 0.f);
		fq::game_module::ETag tag = GetGameObject()->GetTag();
		bool bUseDebugDraw = true;
		bool isStatic = true;

		fq::event::RayCast::ResultData data;

		GetScene()->GetEventManager()->FireEvent<fq::event::RayCast>(
			fq::event::RayCast {origin, direction, mDistance, tag, & data, bUseDebugDraw, isStatic}
		);

		if (data.hitCount > 0)
		{
			DirectX::SimpleMath::Vector3 contactPoint = data.hitContactPoints[0];

			transform->SetWorldPosition(contactPoint);
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
