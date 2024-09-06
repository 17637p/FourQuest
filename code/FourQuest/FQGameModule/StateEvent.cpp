#include "StateEvent.h"

namespace fq::game_module
{
	StateEvent::StateEvent()
	{
	}
	StateEvent::~StateEvent()
	{
	}

	std::shared_ptr<Component> StateEvent::Clone(std::shared_ptr<Component> clone) const
	{
		auto cloneAnimator = std::dynamic_pointer_cast<StateEvent>(clone);

		if (cloneAnimator == nullptr) // ���� �����ؼ� ���纻�� �ش�
		{
			cloneAnimator = ObjectPool::GetInstance()->Assign<StateEvent>(*this);
		}
		else // clone�� �����͸� �����Ѵ�.
		{
			// �⺻ ���� ������ ȣ���Ѵ�.
			*cloneAnimator = *this;
		}

		return cloneAnimator;
	}

	entt::meta_handle StateEvent::GetHandle()
	{
		return *this;
	}
}