#include "TextPrintTrack.h"

#include "Scene.h"
#include "TextUI.h"
#include "Transform.h"

#include "ScreenManager.h"

#include "Event.h"
#include "EventManager.h"

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

		mTrackObjectName.push_back(mNameObject->GetName());
		mTrackObjectName.push_back(mTextObject->GetName());

		nameTransform->SetWorldPosition(DirectX::SimpleMath::Vector3(mScene->GetScreenManager()->GetFixScreenWidth() / 2.f, mScene->GetScreenManager()->GetFixScreenHeight() - mNameFontCenterY, 0.f));
		textTransform->SetWorldPosition(DirectX::SimpleMath::Vector3(mScene->GetScreenManager()->GetFixScreenWidth() / 2.f, mScene->GetScreenManager()->GetFixScreenHeight() - mTextFontCenterY, 0.f));

		// 이벤트 재생
		mScene->GetEventManager()->FireEvent<fq::event::SequenceEnterSpeech>({ mName });
	}

	void TextPrintTrack::PlayOn()
	{
		auto name = mNameObject->GetComponent<TextUI>();
		auto text = mTextObject->GetComponent<TextUI>();
		auto nameTransform = mNameObject->GetComponent<Transform>();
		auto textTransform = mTextObject->GetComponent<Transform>();
		nameTransform->SetWorldPosition(DirectX::SimpleMath::Vector3(mScene->GetScreenManager()->GetFixScreenWidth() / 2.f, mScene->GetScreenManager()->GetFixScreenHeight() - mNameFontCenterY, 0.f));
		textTransform->SetWorldPosition(DirectX::SimpleMath::Vector3(mScene->GetScreenManager()->GetFixScreenWidth() / 2.f, mScene->GetScreenManager()->GetFixScreenHeight() - mTextFontCenterY, 0.f));

		// Scale 자동 조정 
		UINT screenWidth = mScene->GetScreenManager()->GetFixScreenWidth();
		UINT screenHeight = mScene->GetScreenManager()->GetFixScreenHeight();
		{
			nameTransform->SetLocalScale({ screenWidth / (float)1920, screenHeight / (float)1080, 1 });
			textTransform->SetLocalScale({ screenWidth / (float)1920, screenHeight / (float)1080, 1 });
		}

		fq::graphics::TextInfo textInfo;
		textInfo.Align = fq::graphics::ETextAlign::CenterTop;
		textInfo.BoxAlign = fq::graphics::ETextBoxAlign::CenterCenter;
		textInfo.Text = mName;
		textInfo.FontSize = mNameFontSize;
		textInfo.FontColor = mNameFontColor;
		textInfo.CenterX = 960.f;
		textInfo.CenterY = 1080.f - mTextFontCenterY;
		textInfo.Width = mScene->GetScreenManager()->GetScreenWidth();
		textInfo.FontPath = "던파 연단된 칼날";
		name->SetTextInfo(textInfo);

		textInfo.Text = mText;
		textInfo.FontSize = mTextFontSize;
		textInfo.FontColor = mTextFontColor;
		textInfo.CenterX = 960.f;
		textInfo.CenterY = 1080.f - mTextFontCenterY;
		textInfo.Width = mScene->GetScreenManager()->GetScreenWidth();
		text->SetTextInfo(textInfo);
	}

	void TextPrintTrack::PlayExit()
	{
		// 이벤트 재생
		mScene->GetEventManager()->FireEvent<fq::event::SequenceExitSpeech>({ mName });

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
