#include "SphereCollider.h"

fq::game_module::SphereCollider::~SphereCollider()
{}

fq::game_module::SphereCollider::SphereCollider()
{

}

fq::game_module::Component* fq::game_module::SphereCollider::Clone(Component* clone /* = nullptr */) const
{
	SphereCollider* cloneCollider = static_cast<SphereCollider*>(clone);

	if (cloneCollider == nullptr) // 새로 생성해서 복사본을 준다
	{
		cloneCollider = new SphereCollider(*this);
	}
	else // clone에 데이터를 복사한다.
	{
		// 기본 대입 연산자 호출한다.
		*cloneCollider = *this;
	}

	return cloneCollider;
}

entt::meta_handle fq::game_module::SphereCollider::GetHandle()
{
	return *this;
}
