#define NOMINMAX 
#include "PlayerDummy.h"
#include "../FQGameModule/GameObject.h"
#include "../FQGameModule/Scene.h"

fq::client::PlayerDummy::PlayerDummy()
	: mElapsedTime(0.f)
	, mDuration(0.f)
{
}

fq::client::PlayerDummy::~PlayerDummy()
{}

std::shared_ptr<fq::game_module::Component> fq::client::PlayerDummy::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	auto cloneController = std::dynamic_pointer_cast<PlayerDummy>(clone);

	if (cloneController == nullptr) // ���� �����ؼ� ���纻�� �ش�
	{
		cloneController = game_module::ObjectPool::GetInstance()->Assign<PlayerDummy>(*this);
	}
	else // clone�� �����͸� �����Ѵ�.
	{
		// �⺻ ���� ������ ȣ���Ѵ�.
		*cloneController = *this;
	}

	return cloneController;
}

void fq::client::PlayerDummy::OnStart()
{
	mElapsedTime = 0.f;
}

void fq::client::PlayerDummy::OnUpdate(float dt)
{
	mElapsedTime += dt;

	if (mDuration < mElapsedTime)
	{
		GetScene()->DestroyGameObject(GetGameObject());
	}
}
