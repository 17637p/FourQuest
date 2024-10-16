#include "DestroyWhenCollisionToWall.h"

#include "../FQGameModule/Scene.h"

namespace fq::client
{
	DestroyWhenCollisionToWall::DestroyWhenCollisionToWall()
	{
	}
	DestroyWhenCollisionToWall::~DestroyWhenCollisionToWall()
	{
	}

	void DestroyWhenCollisionToWall::OnTriggerEnter(const fq::game_module::Collision& collision)
	{
		// 바닥과 부딪혔을 때, 부모 오브젝트가 있으면 부모 오브젝트 삭제, 아니면 자기 자신 오브젝트 삭제
		if (collision.other->GetTag() == fq::game_module::ETag::Wall)
		{
			auto gameObject = GetGameObject()->GetParent();

			if (GetGameObject()->GetParent() != nullptr)
			{
				GetScene()->DestroyGameObject(gameObject);
			}
			else
			{
				gameObject = GetGameObject();

				GetScene()->DestroyGameObject(gameObject);
			}
		}
	}

	std::shared_ptr<fq::game_module::Component> DestroyWhenCollisionToWall::Clone(std::shared_ptr<Component> clone) const
	{
		auto yAxisSetter = std::dynamic_pointer_cast<DestroyWhenCollisionToWall>(clone);

		if (yAxisSetter == nullptr) // 새로 생성해서 복사본을 준다
		{
			yAxisSetter = game_module::ObjectPool::GetInstance()->Assign<DestroyWhenCollisionToWall>(*this);
		}
		else // clone에 데이터를 복사한다.
		{
			// 기본 대입 연산자 호출한다.
			*yAxisSetter = *this;
		}

		return yAxisSetter;
	}

	entt::meta_handle DestroyWhenCollisionToWall::GetHandle()
	{
		return *this;
	}
}
