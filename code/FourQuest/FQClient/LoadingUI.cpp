#include "LoadingUI.h"

#include "../FQGameModule/Transform.h"
#include "../FQGameModule/Scene.h"
#include "../FQGameModule/ScreenManager.h"
#include "../FQGameModule/ImageUI.h"
#include "../FQGameModule/TextUI.h"

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
	mScreenManager = GetScene()->GetScreenManager();

	// 초기값 세팅
	auto Bars = GetGameObject()->GetChildren()[0]->GetChildren();
	mProgressBar = Bars[0]->GetComponent<game_module::ImageUI>();
	mProgressBarMaxWidth = mProgressBar->GetUIInfomation(0).Width;

	mProgressSoul = Bars[1]->GetComponent<game_module::Transform>();

	auto guide = GetGameObject()->GetChildren()[1]->GetChildren();
	mImages = guide[0]->GetChildren();

	auto texts = guide[1]->GetChildren();
	mTitle = texts[0]->GetComponent<game_module::TextUI>();
	mContent = texts[1]->GetComponent<game_module::TextUI>();

	texts = guide[2]->GetChildren();
	mBackgroundTitle = texts[0]->GetComponent<game_module::TextUI>();
	mBackgroundContent = texts[1]->GetComponent<game_module::TextUI>();

	// 시작할 때 모두 Render Off
	auto textInfo = mTitle->GetTextInfo();
	textInfo.IsRender = false;
	mTitle->SetTextInfo(textInfo);

	textInfo = mContent->GetTextInfo();
	textInfo.IsRender = false;
	mContent->SetTextInfo(textInfo);

	textInfo = mBackgroundTitle->GetTextInfo();
	textInfo.IsRender = false;
	mBackgroundTitle->SetTextInfo(textInfo);

	textInfo = mBackgroundContent->GetTextInfo();
	textInfo.IsRender = false;
	mBackgroundContent->SetTextInfo(textInfo);
}

void fq::client::LoadingUI::OnUpdate(float dt)
{
	SetScaleScreen();

	// 배경
	if (mCurGuideID > 5)
	{
		auto textInfo = mBackgroundTitle->GetTextInfo();
		textInfo.Text = mTexts[mCurGuideID].Title;
		textInfo.IsRender = true;
		mBackgroundTitle->SetTextInfo(textInfo);

		textInfo = mBackgroundContent->GetTextInfo();
		textInfo.Text = mTexts[mCurGuideID].Content;
		textInfo.IsRender = true;
		mBackgroundContent->SetTextInfo(textInfo);
	}
	else
	{
		auto textInfo = mTitle->GetTextInfo();
		textInfo.Text = mTexts[mCurGuideID].Title;
		textInfo.IsRender = true;
		mTitle->SetTextInfo(textInfo);

		textInfo = mContent->GetTextInfo();
		textInfo.Text = mTexts[mCurGuideID].Content;
		textInfo.IsRender = true;
		mContent->SetTextInfo(textInfo);
	}

	for (int i = 0; i < mImages.size(); i++)
	{
		mImages[i]->GetComponent<game_module::ImageUI>()->SetIsRender(0, false);
	}

	mImages[mCurGuideID]->GetComponent<game_module::ImageUI>()->SetIsRender(0, true);
}

fq::client::LoadingUI::LoadingUI()
	:mScreenManager(nullptr),
	mProgressBar(nullptr),
	mProgressSoul(nullptr),
	mProgressBarMaxWidth(0),
	mTexts(),
	mImages(),
	mContent(nullptr),
	mCurGuideID(7)
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

void fq::client::LoadingUI::SetScaleScreen()
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
