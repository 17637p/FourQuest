#pragma once

#include "Sequence.h"
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
		float mNameFontCenterY = 500.f;
		float mNameFontSize = 12.f;
		DirectX::SimpleMath::Color mNameFontColor = { 0.f, 0.f, 1.f, 1.f };
		std::string mName = {};

		float mTextFontCenterY = 550.f;
		float mTextFontSize = 10.f;
		DirectX::SimpleMath::Color mTextFontColor = { 1.f, 1.f, 1.f, 1.f };
		std::string mText = {};

		Scene* mScene;
		std::shared_ptr<GameObject> mNameObject;
		std::shared_ptr<GameObject> mTextObject;
	};
}

