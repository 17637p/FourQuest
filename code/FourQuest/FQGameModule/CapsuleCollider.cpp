#include "CapsuleCollider.h"

fq::game_module::CapsuleCollider::CapsuleCollider()
{

}

fq::game_module::CapsuleCollider::~CapsuleCollider()
{

}

fq::game_module::Component* fq::game_module::CapsuleCollider::Clone(Component* clone /* = nullptr */) const
{
	CapsuleCollider* cloneCollider = static_cast<CapsuleCollider*>(clone);

	if (cloneCollider == nullptr) // 새로 생성해서 복사본을 준다
	{
		cloneCollider = new CapsuleCollider(*this);
	}
	else // clone에 데이터를 복사한다.
	{
		// 기본 대입 연산자 호출한다.
		*cloneCollider = *this;
	}

	return cloneCollider;
}

entt::meta_handle fq::game_module::CapsuleCollider::GetHandle()
{
	return *this;
}

float fq::game_module::CapsuleCollider::GetRadius() const
{
	return mCapsuleInfomation.raidus;
}

void fq::game_module::CapsuleCollider::SetRadius(float radius)
{
	mCapsuleInfomation.raidus = radius;
}
