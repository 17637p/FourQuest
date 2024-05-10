#include "PrefabTest.h"
#include "PrefabManager.h"
#include "Scene.h"
#include "Transform.h"
#include "Event.h"

fq::game_module::PrefabTest::PrefabTest()
	:mCreateTime(1.f)
	, mElapsedTime(0.f)
	, mFireObject{}
{}

fq::game_module::PrefabTest::~PrefabTest()
{

}

std::shared_ptr<fq::game_module::Component> fq::game_module::PrefabTest::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	auto cloneCamera = std::dynamic_pointer_cast<PrefabTest>(clone);

	if (cloneCamera == nullptr) // ���� �����ؼ� ���纻�� �ش�
	{
		cloneCamera = ObjectPool::GetInstance()->Assign<PrefabTest>(*this);
	}
	else // clone�� �����͸� �����Ѵ�.
	{
		// �⺻ ���� ������ ȣ���Ѵ�.
		*cloneCamera = *this;
	}

	return cloneCamera;
}

entt::meta_handle fq::game_module::PrefabTest::GetHandle()
{
	return *this;
}

void fq::game_module::PrefabTest::OnUpdate(float dt)
{
	mElapsedTime += dt;

	if (GetCreateTime() <= mElapsedTime)
	{
		mElapsedTime -= GetCreateTime();

		auto instance = GetScene()->GetPrefabManager()->InstantiatePrefabResoure(mFireObject);

		auto& object = *(instance.begin());

		auto transform = GetComponent<Transform>();

		object->GetComponent<Transform>()->SetWorldMatrix(transform->GetWorldMatrix());

		GetScene()->AddGameObject(object);
	}
}
