#include "TextPrintTrack.h"

#include "Scene.h"
#include "TextUI.h"
#include "Transform.h"

#include "ScreenManager.h"

namespace fq::game_module
{
	TextPrintTrack::TextPrintTrack()
		: Track(ETrackType::TEXT_PRINT)
		, mName()
		, mNameFontCenterY()
		, mNameFontColor()
		, mNameFontSize()
		, mText()
		, mTextFontCenterY()
		, mTextFontColor()
		, mTextFontSize()
		, mNameObject(nullptr)
		, mTextObject(nullptr)
		, mScene(nullptr)
	{
	}
	TextPrintTrack::~TextPrintTrack()
	{
	}
	bool TextPrintTrack::Initialize(const TextPrintTrackInfo& info, Scene* scene)
	{
		mScene = scene;

		mStartTime = info.startTime;
		mTotalPlayTime = info.totalPlayTime;

		mName = info.name;
		mNameFontCenterY = info.nameFontCenterY;
		mNameFontColor = info.nameFontColor;
		mNameFontSize = info.nameFontSize;

		mText = info.text;
		mTextFontCenterY = info.textFontCenterY;
		mTextFontColor = info.textFontColor;
		mTextFontSize = info.textFontSize;

		return true;
	}

	void TextPrintTrack::PlayEnter()
	{
		mNameObject = std::make_shared<GameObject>();
		auto name = mNameObject->AddComponent<TextUI>();
		auto nameTransform = mNameObject->GetComponent<Transform>();
		mTextObject = std::make_shared<GameObject>();
		auto text = mTextObject->AddComponent<TextUI>();
		auto textTransform = mTextObject->GetComponent<Transform>();

		mScene->AddGameObject(mNameObject);
		mScene->AddGameObject(mTextObject);

		nameTransform->SetWorldPosition(DirectX::SimpleMath::Vector3(960.f, 1080.f - mNameFontCenterY, 0.f));
		textTransform->SetWorldPosition(DirectX::SimpleMath::Vector3(960.f, 1080.f - mTextFontCenterY, 0.f));
	}

	void TextPrintTrack::PlayOn()
	{
		auto name = mNameObject->GetComponent<TextUI>();
		auto text = mTextObject->GetComponent<TextUI>();
		auto nameTransform = mNameObject->GetComponent<Transform>();
		auto textTransform = mTextObject->GetComponent<Transform>();
		nameTransform->SetWorldPosition(DirectX::SimpleMath::Vector3(960.f, 1080.f - mNameFontCenterY, 0.f));
		textTransform->SetWorldPosition(DirectX::SimpleMath::Vector3(960.f, 1080.f - mTextFontCenterY, 0.f));

		fq::graphics::TextInfo textInfo;
		textInfo.Align = fq::graphics::ETextAlign::CenterTop;
		textInfo.BoxAlign = fq::graphics::ETextBoxAlign::CenterCenter;
		textInfo.Text = mName;
		textInfo.FontSize = mNameFontSize;
		textInfo.FontColor = mNameFontColor;
		textInfo.Width = mScene->GetScreenManager()->GetScreenWidth();
		name->SetTextInfo(textInfo);

		textInfo.Text = mText;
		textInfo.FontSize = mTextFontSize;
		textInfo.FontColor = mTextFontColor;
		textInfo.Width = mScene->GetScreenManager()->GetScreenWidth();
		text->SetTextInfo(textInfo);
	}

	void TextPrintTrack::PlayExit()
	{
		if (mNameObject != nullptr)
		{
			mScene->DestroyGameObject(mNameObject.get());
			mNameObject = nullptr;
		}
		if (mTextObject != nullptr)
		{
			mScene->DestroyGameObject(mTextObject.get());
			mTextObject = nullptr;
		}
	}

	void TextPrintTrack::End()
	{
		if (mNameObject != nullptr)
		{
			mScene->DestroyGameObject(mNameObject.get());
			mNameObject = nullptr;
		}
		if (mTextObject != nullptr)
		{
			mScene->DestroyGameObject(mTextObject.get());
			mTextObject = nullptr;
		}
	}
}
