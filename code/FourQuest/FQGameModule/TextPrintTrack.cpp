#include "TextPrintTrack.h"

#include "Scene.h"
#include "TextUI.h"
#include "Transform.h"

#include "ScreenManager.h"

#include "Event.h"
#include "EventManager.h"

#include <boost/locale.hpp>

namespace fq::game_module
{
	TextPrintTrack::TextPrintTrack()
		: Track(ETrackType::TEXT_PRINT)
		, mName()
		, mNameFontCenterY()
		, mNameFontColor()
		, mNameFontSize()
		, mText()
		, mTextFontCenterX(0.f)
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
		mNameFontCenterX = info.nameFontCenterX;
		mNameFontCenterY = info.nameFontCenterY;
		mNameFontColor = info.nameFontColor;
		mNameFontSize = info.nameFontSize;

		mText = info.text;
		mTextFontCenterX = info.textFontCenterX;
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

		//nameTransform->SetWorldPosition(DirectX::SimpleMath::Vector3(mScene->GetScreenManager()->GetFixScreenWidth() / 2.f, mScene->GetScreenManager()->GetFixScreenHeight() - mNameFontCenterY, 0.f));
		//textTransform->SetWorldPosition(DirectX::SimpleMath::Vector3(mScene->GetScreenManager()->GetFixScreenWidth() / 2.f, mScene->GetScreenManager()->GetFixScreenHeight() - mTextFontCenterY, 0.f));

		// 이벤트 재생
		mScene->GetEventManager()->FireEvent<fq::event::SequenceEnterSpeech>({ mName });
	}

	void TextPrintTrack::PlayOn()
	{
		auto name = mNameObject->GetComponent<TextUI>();
		auto text = mTextObject->GetComponent<TextUI>();
		auto nameTransform = mNameObject->GetComponent<Transform>();
		auto textTransform = mTextObject->GetComponent<Transform>();
		auto screenMgr = mScene->GetScreenManager();
		auto fixedWidth = screenMgr->GetFixScreenWidth();
		auto fixedHeight = screenMgr->GetFixScreenHeight();

		float nameX = (fixedWidth * 0.5f) + (mNameFontCenterX * (1 / (fixedWidth / (float)1920)));
		float textX = (fixedWidth * 0.5f) + (mTextFontCenterX *(1 / (fixedWidth / (float)1920)));

		nameTransform->SetWorldPosition(DirectX::SimpleMath::Vector3(nameX, fixedHeight - mNameFontCenterY, 0.f));
		textTransform->SetWorldPosition(DirectX::SimpleMath::Vector3(textX, fixedHeight - mTextFontCenterY, 0.f));

		// Scale 자동 조정 
		UINT screenWidth = mScene->GetScreenManager()->GetFixScreenWidth();
		UINT screenHeight = mScene->GetScreenManager()->GetFixScreenHeight();
		nameTransform->SetLocalScale({ screenWidth / (float)1920, screenHeight / (float)1080, 1 });
		textTransform->SetLocalScale({ screenWidth / (float)1920, screenHeight / (float)1080, 1 });

		fq::graphics::TextInfo textInfo;
		textInfo.Align = fq::graphics::ETextAlign::LeftTop;
		textInfo.BoxAlign = fq::graphics::ETextBoxAlign::LeftTop;
		textInfo.Text = mName;
		textInfo.FontSize = mNameFontSize;
		textInfo.FontColor = mNameFontColor;
		//textInfo.CenterX = 960.f + mNameFontCenterX;
		//textInfo.CenterY = 1080.f - mNameFontCenterY;
		textInfo.Layer = 150;
		textInfo.Width = mScene->GetScreenManager()->GetScreenWidth();
		std::string font = boost::locale::conv::from_utf(L"던파 연단된 칼날", "UTF-8");
		textInfo.FontPath = font;
		textInfo.isUseAutoCenterAlign = true;
		name->SetTextInfo(textInfo);

		textInfo.Text = mText;
		textInfo.FontSize = mTextFontSize;
		textInfo.FontColor = mTextFontColor;
		textInfo.Layer = 150;
		//textInfo.CenterX = 960.f + mTextFontCenterX;
		//textInfo.CenterY = 1080.f - mTextFontCenterY;
		textInfo.Width = mScene->GetScreenManager()->GetScreenWidth();
		textInfo.isUseAutoCenterAlign = true;
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
