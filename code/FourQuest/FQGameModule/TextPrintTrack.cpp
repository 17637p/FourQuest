#include "TextPrintTrack.h"

#include "Scene.h"
#include "TextUI.h"
#include "Transform.h"

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
	bool TextPrintTrack::Initialize(TextPrintTrackInfo info, Scene* scene)
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
		std::shared_ptr<GameObject> nameObject = std::make_shared<GameObject>();
		auto name = nameObject->AddComponent<TextUI>();
		auto nameTransform = nameObject->GetComponent<Transform>();
		std::shared_ptr<GameObject> textObject = std::make_shared<GameObject>();
		auto text = textObject->AddComponent<TextUI>();
		auto textTransform = textObject->GetComponent<Transform>();

		mScene->AddGameObject(nameObject);
		mScene->AddGameObject(textObject);

		mNameObject = nameObject.get();
		mTextObject = textObject.get();

		nameTransform->SetWorldPosition(DirectX::SimpleMath::Vector3(960.f, mNameFontCenterY, 0.f));
		textTransform->SetWorldPosition(DirectX::SimpleMath::Vector3(960.f, mTextFontCenterY, 0.f));
	}

	void TextPrintTrack::PlayOn()
	{
		auto name = mNameObject->GetComponent<TextUI>();
		auto text = mTextObject->GetComponent<TextUI>();

		fq::graphics::TextInfo textInfo;
		textInfo.Text = mName;
		textInfo.FontSize = mNameFontSize;
		textInfo.FontColor = mNameFontColor;
		textInfo.CenterX = 960.f;
		textInfo.CenterY = mNameFontCenterY;
		textInfo.Width = 300.f;
		name->SetTextInfo(textInfo);

		textInfo.Text = mText;
		textInfo.FontSize = mTextFontSize;
		textInfo.FontColor = mTextFontColor;
		textInfo.CenterX = 960.f;
		textInfo.CenterY = mTextFontCenterY;
		textInfo.Width = 1500.f;
		text->SetTextInfo(textInfo);
	}

	void TextPrintTrack::PlayExit()
	{
		mScene->DestroyGameObject(mNameObject);
		mScene->DestroyGameObject(mTextObject);
	}

	void TextPrintTrack::End()
	{
	}
}
