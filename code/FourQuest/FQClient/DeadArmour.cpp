#include "DeadArmour.h"

#include "Soul.h"

fq::client::DeadArmour::DeadArmour()
	:mPlayerCount(0)
{}

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

	auto instance = GetScene()->GetPrefabManager()->InstantiatePrefabResoure(mLivingArmourPrefab);

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
	if (collision.object->HasComponent<Soul>())
	{
		++mPlayerCount;
	}

	for (auto& child : GetGameObject()->GetChildren())
	{
		if (child->HasComponent<game_module::SkinnedMeshRenderer>())
		{
			child->GetComponent<game_module::SkinnedMeshRenderer>()
				->SetOutlineColor({ 0.8f, 0.6f, 0.2f, 1.0f });
		}

		// 상호작용 UI 
		setUI(true);
	}
}

void fq::client::DeadArmour::OnTriggerExit(const game_module::Collision& collision)
{
	if (collision.object->HasComponent<Soul>())
	{
		--mPlayerCount;
	}

	if (mPlayerCount == 0)
	{
		for (auto& child : GetGameObject()->GetChildren())
		{
			// OutLine
			if (child->HasComponent<game_module::SkinnedMeshRenderer>())
			{
				child->GetComponent<game_module::SkinnedMeshRenderer>()
					->SetOutlineColor({ 0.f, 0.f, 0.f, 1.0f });
			}
			// 상호작용 UI 
			setUI(false);
		}
	}
}

void fq::client::DeadArmour::OnStart()
{
	assert(GetComponent<game_module::ImageUI>() != nullptr);

	setUI(false);
}

void fq::client::DeadArmour::setUI(bool isVisible)
{
	using namespace DirectX::SimpleMath;

	auto imageUI = GetComponent<game_module::ImageUI>();

	auto uiInfo = imageUI->GetUIInfomations();
	if (isVisible)
	{
		// UI 보이게 설정 
		uiInfo[0].ScaleX = 1.f;
		uiInfo[0].ScaleY = 1.f;
		fq::game_module::Camera* mainCamera = nullptr;

		GetScene()->GetEventManager()->FireEvent < fq::event::GetMainCamera>(
			{
				&mainCamera
			});
		Vector3 pos = GetComponent<game_module::Transform>()->GetWorldPosition();
		
		float height = GetScene()->GetScreenManager()->GetScreenHeight();
		float width = GetScene()->GetScreenManager()->GetScreenWidth();

		auto viewProj = mainCamera->GetViewProjection();
		Vector3 screenPos = Vector3::Transform(pos, viewProj);
	}
	else
	{
		uiInfo[0].ScaleX = 0.f;
		uiInfo[0].ScaleY = 0.f;
	}

	imageUI->SetUIInfomations(uiInfo);
}

void fq::client::DeadArmour::OnUpdate(float dt)
{
	// UI 위치 갱신 


}

