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

	if (cloneController == nullptr) // 새로 생성해서 복사본을 준다
	{
		cloneController = game_module::ObjectPool::GetInstance()->Assign<DeadArmour>(*this);
	}
	else // clone에 데이터를 복사한다.
	{
		// 기본 대입 연산자 호출한다.
		*cloneController = *this;
	}

	return cloneController;
}

void fq::client::DeadArmour::SummonLivingArmour(ControllerID id)
{
	assert(id <= 3);

	auto instance = GetScene()->GetPrefabManager()->InstantiatePrefabResoure(GetLivingArmour());

	auto& livingArmour = *(instance.begin());

	// 컨트롤러 연결
	livingArmour->GetComponent<game_module::CharacterController>()->SetControllerID(id);

	// 위치 설정
	auto localMat = GetComponent<game_module::Transform>()->GetLocalMatrix();
	livingArmour->GetComponent<game_module::Transform>()->SetLocalMatrix(localMat);

	GetScene()->AddGameObject(livingArmour);

	// DeadArmour 삭제 
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

