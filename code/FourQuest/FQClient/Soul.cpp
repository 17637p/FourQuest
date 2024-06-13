#include "Soul.h"

#include "DeadArmour.h"
#include "CameraMoving.h"

fq::client::Soul::Soul()
	:mController(nullptr)
{}

fq::client::Soul::~Soul()
{

}

std::shared_ptr<fq::game_module::Component> fq::client::Soul::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	auto cloneSoul = std::dynamic_pointer_cast<Soul>(clone);

	if (cloneSoul == nullptr) // 새로 생성해서 복사본을 준다소울라이크
	{
		cloneSoul = game_module::ObjectPool::GetInstance()->Assign<Soul>(*this);
	}
	else // clone에 데이터를 복사한다.
	{
		// 기본 대입 연산자 호출한다.
		*cloneSoul = *this;
	}

	return cloneSoul;
}

void fq::client::Soul::OnTriggerStay(const fq::game_module::Collision& collision)
{
	auto deadArmour = collision.other->GetComponent<fq::client::DeadArmour>();

	if (deadArmour == nullptr)
		return;
	
	mSelectArmour = deadArmour;

	auto input = GetScene()->GetInputManager();

	// Controller 
	if (input->IsPadKeyState(mController->GetControllerID(), EPadKey::B, EKeyState::Tap))
	{
		deadArmour->SummonLivingArmour(mController->GetControllerID());
		DestorySoul();
	}
}

void fq::client::Soul::OnStart()
{
	mController = GetComponent<game_module::CharacterController>();


	// 카메라에 플레이어 등록 
	GetScene()->ViewComponents<CameraMoving>([this](game_module::GameObject& object, CameraMoving& camera) 
		{
			camera.AddPlayerTransform(GetComponent<game_module::Transform>());
		});
}

void fq::client::Soul::DestorySoul()
{
	GetScene()->DestroyGameObject(GetGameObject());
	
	// 카메라에 플레이어 해제 
	GetScene()->ViewComponents<CameraMoving>([this](game_module::GameObject& object, CameraMoving& camera)
		{
			camera.DeletePlayerTransform(GetComponent<game_module::Transform>());
		});

}

void fq::client::Soul::OnTriggerEnter(const fq::game_module::Collision& collision)
{

}

