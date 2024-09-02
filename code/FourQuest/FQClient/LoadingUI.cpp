#include "LoadingUI.h"

#include "../FQGameModule/Transform.h"
#include "../FQGameModule/Scene.h"
#include "../FQGameModule/ScreenManager.h"
#include "../FQGameModule/ImageUI.h"

std::shared_ptr<fq::game_module::Component> fq::client::LoadingUI::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	auto loadingUI = std::dynamic_pointer_cast<LoadingUI>(clone);

	if (loadingUI == nullptr) // 새로 생성해서 복사본을 준다
	{
		loadingUI = game_module::ObjectPool::GetInstance()->Assign<LoadingUI>(*this);
	}
	else // clone에 데이터를 복사한다.
	{
		// 기본 대입 연산자 호출한다.
		*loadingUI = *this;
	}

	return loadingUI;
}

void fq::client::LoadingUI::OnStart()
{
	fq::game_module::Scene* scene = GetScene();
	mScreenManager = GetScene()->GetScreenManager();

	// 초기값 세팅
	auto Bars = GetGameObject()->GetChildren()[0]->GetChildren();
	mProgressBar = Bars[0]->GetComponent<game_module::ImageUI>();
	mProgressBarMaxWidth = mProgressBar->GetUIInfomation(0).Width;

	mProgressSoul = Bars[1]->GetComponent<game_module::Transform>();
}

void fq::client::LoadingUI::OnUpdate(float dt)
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

fq::client::LoadingUI::LoadingUI()
	:mScreenManager(nullptr),
	mProgressBar(nullptr),
	mProgressSoul(nullptr),
	mProgressBarMaxWidth(0)
{

}

fq::client::LoadingUI::~LoadingUI()
{

}

void fq::client::LoadingUI::SetProgressBar(float progress)
{
	// Bar Percent Setting
	auto uiInfo = mProgressBar->GetUIInfomation(0);

	uiInfo.Width = mProgressBarMaxWidth * progress;
	uiInfo.XRatio = progress;

	mProgressBar->SetUIInfomation(0, uiInfo);

	// Soul Position Setting
	mProgressSoul->SetLocalPosition({ uiInfo.Width - 50 , -90, 0});
}
