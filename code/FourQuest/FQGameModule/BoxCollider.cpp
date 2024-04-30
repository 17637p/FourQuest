#include "BoxCollider.h"

fq::game_module::BoxCollider::BoxCollider()
	:mBoxinfomation{}
{
	mBoxinfomation.boxExtent = { 1.f, 1.f, 1.f };
}

fq::game_module::BoxCollider::~BoxCollider()
{

}

void fq::game_module::BoxCollider::SetExtent(DirectX::SimpleMath::Vector3 extent)
{
	mBoxinfomation.boxExtent.x = max(extent.x, 0.000001f);
	mBoxinfomation.boxExtent.y = max(extent.y, 0.000001f);
	mBoxinfomation.boxExtent.z = max(extent.z, 0.000001f);
}

DirectX::SimpleMath::Vector3 fq::game_module::BoxCollider::GetExtent() const
{
	return mBoxinfomation.boxExtent;
}

entt::meta_handle fq::game_module::BoxCollider::GetHandle()
{
	return *this;
}

fq::game_module::Component* fq::game_module::BoxCollider::Clone(Component* clone /* = nullptr */) const
{
	BoxCollider* cloneCollider = static_cast<BoxCollider*>(clone);

	if (cloneCollider == nullptr) // 새로 생성해서 복사본을 준다
	{
		cloneCollider = new BoxCollider(*this);
	}
	else // clone에 데이터를 복사한다.
	{
		// 기본 대입 연산자 호출한다.
		*cloneCollider = *this;
	}

	return cloneCollider;
}

