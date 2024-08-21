#include "LoadingUI.h"

#include "../FQGameModule/Transform.h"
#include "../FQGameModule/Scene.h"
#include "../FQGameModule/ScreenManager.h"

std::shared_ptr<fq::game_module::Component> fq::client::LoadingUI::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	auto loadingUI = std::dynamic_pointer_cast<LoadingUI>(clone);

	if (loadingUI == nullptr) // ���� �����ؼ� ���纻�� �ش�
	{
		loadingUI = game_module::ObjectPool::GetInstance()->Assign<LoadingUI>(*this);
	}
	else // clone�� �����͸� �����Ѵ�.
	{
		// �⺻ ���� ������ ȣ���Ѵ�.
		*loadingUI = *this;
	}

	return loadingUI;
}

void fq::client::LoadingUI::OnStart()
{
	fq::game_module::Scene* scene = GetScene();
	mScreenManager = GetScene()->GetScreenManager();
}

void fq::client::LoadingUI::OnUpdate(float dt)
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

fq::client::LoadingUI::LoadingUI()
	:mScreenManager(nullptr)
{

}

fq::client::LoadingUI::~LoadingUI()
{

}
