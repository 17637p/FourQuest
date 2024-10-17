#pragma once

#include "SequenceStructure.h"
#include "Track.h"

namespace fq::game_module
{
	class TextPrintTrack : public Track
	{
	public:
		TextPrintTrack();
		~TextPrintTrack();

		bool Initialize(const TextPrintTrackInfo& info, Scene* scene);

	private:
		virtual void PlayEnter() override;
		virtual void PlayOn() override;
		virtual void PlayExit() override;
		virtual void End() override;

	private:
		std::string mFontPath;
		float mNameFontCenterY;
		float mNameFontSize;
		DirectX::SimpleMath::Color mNameFontColor;
		std::string mName;

		float mTextFontCenterY;
		float mTextFontSize;
		DirectX::SimpleMath::Color mTextFontColor;
		std::string mText;

		Scene* mScene;
		std::shared_ptr<GameObject> mNameObject;
		std::shared_ptr<GameObject> mTextObject;
	};
}

