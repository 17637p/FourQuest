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

	if (cloneAutoPositionToScale == nullptr) // ���� �����ؼ� ���纻�� �ش�
	{
		cloneAutoPositionToScale = game_module::ObjectPool::GetInstance()->Assign<AutoPositionToScale>(*this);
	}
	else // clone�� �����͸� �����Ѵ�.
	{
		// �⺻ ���� ������ ȣ���Ѵ�.
		*cloneAutoPositionToScale = *this;
	}

	return cloneAutoPositionToScale;
}
