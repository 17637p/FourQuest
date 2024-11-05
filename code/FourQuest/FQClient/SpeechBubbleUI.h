#pragma once
#include "../FQGameModule/Component.h"
#include "../FQGameModule/EventHandler.h"

namespace fq::game_module
{
	class ImageUI;
	class ScreenManager;
	class Camera;
}

namespace fq::client
{
	class SpeechBubbleUI : public game_module::Component
	{
	public:
		SpeechBubbleUI();
		~SpeechBubbleUI();

		SpeechBubbleUI(const SpeechBubbleUI& other);
		SpeechBubbleUI& operator=(const SpeechBubbleUI& other);

		virtual void OnStart() override;
		virtual void OnUpdate(float dt) override;
		virtual void OnDestroy() override;

		void play();
		void SetName(std::string name) { mName = name; }

	private:
		entt::meta_handle GetHandle() override { return *this; }
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

		void eventProcessSequenceEnterSpeech();
		void eventProcessSequenceExitSpeech();
		void eventProcessOnUIRender();

	private:
		std::string mName;

		DirectX::SimpleMath::Vector3 mWorldOffset;
		float mScreenOffset;

		game_module::EventHandler mSequenceSpeechEnterHandler;
		game_module::EventHandler mSequenceSpeechExitHandler;
		game_module::EventHandler mUIRenderHandler;

		bool mIsOn;

		float mCurTime;
		float mPlaytime;
		int mCurIndex;
		std::vector<game_module::ImageUI*> mImageUIs;
		game_module::ScreenManager* mScreenManager;
		game_module::Camera* mMainCamera;

		bool mIsRenderingUI;

	private:
		friend void RegisterMetaData();
	};
}

