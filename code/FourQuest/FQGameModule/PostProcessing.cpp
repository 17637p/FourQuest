#include "PostProcessing.h"

entt::meta_handle fq::game_module::PostProcessing::GetHandle()
{
	return *this;
}

std::shared_ptr<fq::game_module::Component> fq::game_module::PostProcessing::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	auto cloneMesh = std::dynamic_pointer_cast<PostProcessing>(clone);

	if (cloneMesh == nullptr) // 새로 생성해서 복사본을 준다
	{
		cloneMesh = ObjectPool::GetInstance()->Assign<PostProcessing>(*this);
	}
	else // clone에 데이터를 복사한다.
	{
		// 기본 대입 연산자 호출한다.
		*cloneMesh = *this;
	}

	return cloneMesh;
}

