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

	if (resultUI == nullptr) // 새로 생성해서 복사본을 준다
	{
		resultUI = game_module::ObjectPool::GetInstance()->Assign<ResultUI>(*this);
	}
	else // clone에 데이터를 복사한다.
	{
		// 기본 대입 연산자 호출한다.
		*resultUI = *this;
	}

	return resultUI;
}

void fq::client::ResultUI::SetScaleScreen()
{
	game_module::Transform* myTransform = GetComponent<game_module::Transform>();

	// Scale 자동 조정 
	UINT screenWidth = mScreenManager->GetFixScreenWidth();
	UINT screenHeight = mScreenManager->GetFixScreenHeight();
	float scaleX = screenWidth / (float)1920;
	float scaleY = screenHeight / (float)1080;
	{
		myTransform->SetLocalScale({ scaleX, scaleY , 1 });
	}
}
