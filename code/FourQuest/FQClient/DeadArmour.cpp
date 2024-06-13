#include "DeadArmour.h"

fq::client::DeadArmour::DeadArmour()
{

}

fq::client::DeadArmour::~DeadArmour()
{

}

std::shared_ptr<fq::game_module::Component> fq::client::DeadArmour::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	auto cloneController = std::dynamic_pointer_cast<DeadArmour>(clone);

	if (cloneController == nullptr) // ���� �����ؼ� ���纻�� �ش�
	{
		cloneController = game_module::ObjectPool::GetInstance()->Assign<DeadArmour>(*this);
	}
	else // clone�� �����͸� �����Ѵ�.
	{
		// �⺻ ���� ������ ȣ���Ѵ�.
		*cloneController = *this;
	}

	return cloneController;
}

void fq::client::DeadArmour::SummonLivingArmour(ControllerID id)
{
	assert(id <= 3);

	auto instance = GetScene()->GetPrefabManager()->InstantiatePrefabResoure(GetLivingArmour());

	auto& livingArmour = *(instance.begin());

	// ��Ʈ�ѷ� ����
	livingArmour->GetComponent<game_module::CharacterController>()->SetControllerID(id);

	// ��ġ ����
	auto localMat = GetComponent<game_module::Transform>()->GetLocalMatrix();
	livingArmour->GetComponent<game_module::Transform>()->SetLocalMatrix(localMat);

	GetScene()->AddGameObject(livingArmour);

	// DeadArmour ���� 
	GetScene()->DestroyGameObject(GetGameObject());
}

void fq::client::DeadArmour::OnTriggerEnter(const game_module::Collision& collision)
{
	for (auto& child : GetGameObject()->GetChildren())
	{
		if (child->HasComponent<game_module::SkinnedMeshRenderer>())
		{
			child->GetComponent<game_module::SkinnedMeshRenderer>()
				->SetOutlineColor({ 0.8f, 0.6f, 0.2f, 1.0f });
		}
	}
}

void fq::client::DeadArmour::OnTriggerExit(const game_module::Collision& collision)
{
	for (auto& child : GetGameObject()->GetChildren())
	{
		if (child->HasComponent<game_module::SkinnedMeshRenderer>())
		{
			child->GetComponent<game_module::SkinnedMeshRenderer>()
				->SetOutlineColor({ 0.f, 0.f, 0.f, 1.0f });
		}
	}
}

