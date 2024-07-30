#include "AutoPositionToScale.h"
#include "../FQGameModule/Transform.h"

fq::client::AutoPositionToScale::AutoPositionToScale()
{

}

fq::client::AutoPositionToScale::~AutoPositionToScale()
{

}

void fq::client::AutoPositionToScale::OnStart()
{
	game_module::Transform* myTransform = GetComponent<game_module::Transform>();

	mPosX = myTransform->GetLocalPosition().x;
	mPosY = myTransform->GetLocalPosition().y;
}

void fq::client::AutoPositionToScale::OnUpdate(float dt)
{
	game_module::Transform* myTransform = GetComponent<game_module::Transform>();

	DirectX::SimpleMath::Vector3 scale = myTransform->GetWorldScale();
	myTransform->SetLocalPosition({scale.x * mPosX, scale.y * mPosY, 0});
}

std::shared_ptr<fq::game_module::Component> fq::client::AutoPositionToScale::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	auto cloneAutoPositionToScale = std::dynamic_pointer_cast<AutoPositionToScale>(clone);

	if (cloneAutoPositionToScale == nullptr) // 새로 생성해서 복사본을 준다
	{
		cloneAutoPositionToScale = game_module::ObjectPool::GetInstance()->Assign<AutoPositionToScale>(*this);
	}
	else // clone에 데이터를 복사한다.
	{
		// 기본 대입 연산자 호출한다.
		*cloneAutoPositionToScale = *this;
	}

	return cloneAutoPositionToScale;
}
