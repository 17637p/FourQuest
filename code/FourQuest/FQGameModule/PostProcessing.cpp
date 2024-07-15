#include "PostProcessing.h"

entt::meta_handle fq::game_module::PostProcessing::GetHandle()
{
	return *this;
}

std::shared_ptr<fq::game_module::Component> fq::game_module::PostProcessing::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	auto cloneMesh = std::dynamic_pointer_cast<PostProcessing>(clone);

	if (cloneMesh == nullptr) // ���� �����ؼ� ���纻�� �ش�
	{
		cloneMesh = ObjectPool::GetInstance()->Assign<PostProcessing>(*this);
	}
	else // clone�� �����͸� �����Ѵ�.
	{
		// �⺻ ���� ������ ȣ���Ѵ�.
		*cloneMesh = *this;
	}

	return cloneMesh;
}

