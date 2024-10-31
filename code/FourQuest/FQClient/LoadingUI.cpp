#include "LoadingUI.h"

#include "../FQGameModule/Transform.h"
#include "../FQGameModule/Scene.h"
#include "../FQGameModule/ScreenManager.h"
#include "../FQGameModule/ImageUI.h"
#include "../FQGameModule/TextUI.h"

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
	mImages.clear();
	mTitles.clear();
	mContents.clear();

	mScreenManager = GetScene()->GetScreenManager();

	// �ʱⰪ ����
	auto Bars = GetGameObject()->GetChildren()[0]->GetChildren();
	mProgressBar = Bars[0]->GetComponent<game_module::ImageUI>();
	mProgressBarMaxWidth = 1720;

	mProgressSoul = Bars[1]->GetComponent<game_module::Transform>();

	auto guide = GetGameObject()->GetChildren()[1]->GetChildren();
	auto images = guide[0]->GetChildren();
	for (auto& image : images)
	{
		mImages.push_back(image->GetComponent<game_module::ImageUI>());
	}

	auto texts = guide[1]->GetChildren();
	for (auto& title : texts)
	{
		mTitles.push_back(title->GetComponent<game_module::TextUI>());
	}

	texts = guide[2]->GetChildren();
	for (auto& content : texts)
	{
		mContents.push_back(content->GetComponent<game_module::TextUI>());
	}

	// ������ �� ��� Render Off
	for (int i = 0; i < mTitles.size(); i++)
	{
		mImages[i]->SetIsRender(0, false);
		mTitles[i]->SetIsRender(false);
		mContents[i]->SetIsRender(false);
	}
}

void fq::client::LoadingUI::OnUpdate(float dt)
{
	SetScaleScreen();

	mImages[mCurGuideID]->SetIsRender(0, true);

	mTitles[mCurGuideID]->SetIsRender(true);
	mTitles[mCurGuideID]->SetText(mTexts[mCurGuideID].Title);
	mContents[mCurGuideID]->SetIsRender(true);
	mContents[mCurGuideID]->SetText(mTexts[mCurGuideID].Content);
}

fq::client::LoadingUI::LoadingUI()
	:mScreenManager(nullptr),
	mProgressBar(nullptr),
	mProgressSoul(nullptr),
	mProgressBarMaxWidth(0),
	mTexts(),
	mImages(),
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
	mProgressSoul->SetLocalPosition({ uiInfo.Width - 50 , -90, 0 });
}

void fq::client::LoadingUI::SetScaleScreen()
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
