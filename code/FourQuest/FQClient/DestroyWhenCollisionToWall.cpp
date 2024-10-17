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
		// �ٴڰ� �ε����� ��, �θ� ������Ʈ�� ������ �θ� ������Ʈ ����, �ƴϸ� �ڱ� �ڽ� ������Ʈ ����
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

		if (yAxisSetter == nullptr) // ���� �����ؼ� ���纻�� �ش�
		{
			yAxisSetter = game_module::ObjectPool::GetInstance()->Assign<DestroyWhenCollisionToWall>(*this);
		}
		else // clone�� �����͸� �����Ѵ�.
		{
			// �⺻ ���� ������ ȣ���Ѵ�.
			*yAxisSetter = *this;
		}

		return yAxisSetter;
	}

	entt::meta_handle DestroyWhenCollisionToWall::GetHandle()
	{
		return *this;
	}
}
