#include "ResultUI.h"

#include "../FQGameModule/Transform.h"
#include "../FQGameModule/Scene.h"
#include "../FQGameModule/ScreenManager.h"

fq::client::ResultUI::ResultUI()
	:mScreenManager(nullptr)
{

}

fq::client::ResultUI::~ResultUI()
{

}

void fq::client::ResultUI::OnStart()
{
	mScreenManager = GetScene()->GetScreenManager();
}

void fq::client::ResultUI::OnUpdate(float dt)
{

}

std::shared_ptr<fq::game_module::Component> fq::client::ResultUI::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	auto resultUI = std::dynamic_pointer_cast<ResultUI>(clone);

	if (resultUI == nullptr) // ���� �����ؼ� ���纻�� �ش�
	{
		resultUI = game_module::ObjectPool::GetInstance()->Assign<ResultUI>(*this);
	}
	else // clone�� �����͸� �����Ѵ�.
	{
		// �⺻ ���� ������ ȣ���Ѵ�.
		*resultUI = *this;
	}

	return resultUI;
}

void fq::client::ResultUI::SetScaleScreen()
{
	game_module::Transform* myTransform = GetComponent<game_module::Transform>();

	// Scale �ڵ� ���� 
	UINT screenWidth = mScreenManager->GetFixScreenWidth();
	UINT screenHeight = mScreenManager->GetFixScreenHeight();
	float scaleX = screenWidth / (float)1920;
	float scaleY = screenHeight / (float)1080;
	{
		myTransform->SetLocalScale({ scaleX, scaleY , 1 });
	}
}
