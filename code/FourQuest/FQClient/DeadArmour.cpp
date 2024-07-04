#include "DeadArmour.h"

#include "Soul.h"
#include "Player.h"

fq::client::DeadArmour::DeadArmour()
	:mPlayerCount(0)
	, mbIsVisible(false)
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

void fq::client::DeadArmour::SummonLivingArmour(PlayerInfo info)
{
	assert(info.ControllerID <= 3);

	// 인스턴스 생성
	auto instance = GetScene()->GetPrefabManager()->InstantiatePrefabResoure(mLivingArmourPrefab);
	auto& livingArmour = *(instance.begin());

	// 컨트롤러 연결
	livingArmour->GetComponent<game_module::CharacterController>()->SetControllerID(info.ControllerID);

	// 영혼 타입 설정
	livingArmour->GetComponent<Player>()->SetSoulType(info.SoulType);

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

	mbIsVisible = isVisible;
	auto imageUI = GetComponent<game_module::ImageUI>();

	auto uiInfo = imageUI->GetUIInfomations();
	if (isVisible)
	{
		// UI 보이게 설정 
		uiInfo[0].ScaleX = 1.f;
		uiInfo[0].ScaleY = 1.f;
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
	using namespace DirectX::SimpleMath;

	// UI 위치 갱신 
	if (mbIsVisible)
	{
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
		auto imageUI = GetComponent<game_module::ImageUI>();
		assert(imageUI);

		auto uiInfomations = imageUI->GetUIInfomations();

		uiInfomations[0].StartX = width * 0.5f + (screenPos.x * width * 0.5f);
		uiInfomations[0].StartY = height * 0.5f - (screenPos.y * height * 0.5f);

		imageUI->SetUIInfomations(uiInfomations);
	}

}

