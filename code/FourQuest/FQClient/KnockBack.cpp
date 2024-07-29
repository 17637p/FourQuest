#include "KnockBack.h"

#include "Attack.h"

fq::client::KnockBack::KnockBack()
	:mTransform(nullptr)
	, mVelocity{}
	, mSpeed(0.f)
	, mFriction(1.f)
{}

fq::client::KnockBack::~KnockBack()
{

}


std::shared_ptr<fq::game_module::Component> fq::client::KnockBack::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	auto cloneComponent = std::dynamic_pointer_cast<KnockBack>(clone);

	if (cloneComponent == nullptr) // ���� �����ؼ� ���纻�� �ش�
	{
		cloneComponent = game_module::ObjectPool::GetInstance()->Assign<KnockBack>(*this);
	}
	else // clone�� �����͸� �����Ѵ�.
	{
		// �⺻ ���� ������ ȣ���Ѵ�.
		*cloneComponent = *this;
	}

	cloneComponent->mTransform = nullptr;

	return cloneComponent;
}

void fq::client::KnockBack::updateVelocity(float dt)
{
	// ������ ����
	mVelocity.x = std::lerp(mVelocity.x, 0.f, mFriction);
	mVelocity.y = std::lerp(mVelocity.y, 0.f, mFriction);
	mVelocity.z = std::lerp(mVelocity.z, 0.f, mFriction);

	mSpeed = mVelocity.Length();

	if (mSpeed >= 0.001f)
	{
		// Transform ����
		DirectX::SimpleMath::Vector3 distance{};
		distance.x = mVelocity.x * dt;
		distance.y = mVelocity.y * dt;
		distance.z = mVelocity.z * dt;

		auto position = mTransform->GetWorldPosition();
		position += distance;
		mTransform->SetWorldPosition(position);
	}
}

void fq::client::KnockBack::OnFixedUpdate(float dt)
{
	updateVelocity(dt);
}

void fq::client::KnockBack::OnStart()
{
	mTransform = GetComponent<game_module::Transform>();
}

void fq::client::KnockBack::Set(float power, DirectX::SimpleMath::Vector3 direction)
{
	mVelocity += direction * power;
}

