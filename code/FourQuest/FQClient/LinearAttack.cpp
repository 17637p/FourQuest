#include "LinearAttack.h"

fq::client::LinearAttack::LinearAttack()
	:mMoveDirection{ DirectX::SimpleMath::Vector3::Up }
	, mMoveSpeed(1.f)
{

}

fq::client::LinearAttack::~LinearAttack()
{

}

std::shared_ptr<fq::game_module::Component> fq::client::LinearAttack::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	std::shared_ptr<LinearAttack> cloneMonster = std::dynamic_pointer_cast<LinearAttack>(clone);

	if (cloneMonster == nullptr) // 새로 생성해서 복사본을 준다
	{
		cloneMonster = game_module::ObjectPool::GetInstance()->Assign<LinearAttack>(*this);
	}
	else // clone에 데이터를 복사한다.
	{
		// 기본 대입 연산자 호출한다.
		*cloneMonster = *this;
	}

	return cloneMonster;
}

void fq::client::LinearAttack::OnUpdate(float dt)
{
	auto transform = GetComponent<game_module::Transform>();
	DirectX::SimpleMath::Vector3 deltaPosition = mMoveDirection * dt * mMoveSpeed;
	transform->AddPosition(deltaPosition);
}
