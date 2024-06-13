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

	if (cloneSoul == nullptr) // ���� �����ؼ� ���纻�� �شټҿ����ũ
	{
		cloneSoul = game_module::ObjectPool::GetInstance()->Assign<Soul>(*this);
	}
	else // clone�� �����͸� �����Ѵ�.
	{
		// �⺻ ���� ������ ȣ���Ѵ�.
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


	// ī�޶� �÷��̾� ��� 
	GetScene()->ViewComponents<CameraMoving>([this](game_module::GameObject& object, CameraMoving& camera) 
		{
			camera.AddPlayerTransform(GetComponent<game_module::Transform>());
		});
}

void fq::client::Soul::DestorySoul()
{
	GetScene()->DestroyGameObject(GetGameObject());
	
	// ī�޶� �÷��̾� ���� 
	GetScene()->ViewComponents<CameraMoving>([this](game_module::GameObject& object, CameraMoving& camera)
		{
			camera.DeletePlayerTransform(GetComponent<game_module::Transform>());
		});

}

void fq::client::Soul::OnTriggerEnter(const fq::game_module::Collision& collision)
{

}

