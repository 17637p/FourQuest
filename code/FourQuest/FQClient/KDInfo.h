#pragma once
#include "../FQGameModule/Component.h"
#include "../FQGameModule/EventHandler.h"

namespace fq::game_module
{
	class ImageUI;
	class TextUI;
	class ScreenManager;
}

namespace fq::client
{
	class KDInfo : public game_module::Component
	{
	public:
		KDInfo();
		~KDInfo();

		KDInfo(const KDInfo& other);
		KDInfo& operator=(const KDInfo& other);

		virtual void OnUpdate(float dt) override;
		virtual void OnStart() override;
		virtual void OnDestroy() override;
		//
		//void play();
		//void SetName(std::string name) { mName = name; }

	private:
		entt::meta_handle GetHandle() override { return *this; }
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

		void setScaleScreen();
		void setPlayerPosition(int playerCount);

		void eventProcessPlayerArmourDeath();
		//void eventProcessSequenceExitSpeech();
		//void eventProcessOnUIRender();

	private:
		std::vector<game_module::ImageUI*> mPlayers;
		std::vector<game_module::TextUI*> mKillText;
		std::vector<game_module::TextUI*> mDeathText;
		std::vector<bool> mIsAlives;
		std::vector<int> mKillCount;
		std::vector<int> mDeathCount;
		std::vector<DirectX::SimpleMath::Color> mSoulColors;

		game_module::ScreenManager* mScreenManager;

		game_module::EventHandler mPlayerArmourDeathHandler;

		//std::string mName;
		//
		//DirectX::SimpleMath::Vector3 mWorldOffset;
		//float mScreenOffset;
		//
		//game_module::EventHandler mSequenceSpeechEnterHandler;
		//game_module::EventHandler mSequenceSpeechExitHandler;
		//game_module::EventHandler mUIRenderHandler;
		//
		//bool mIsOn;
		//
		//float mCurTime;
		//float mPlaytime;
		//int mCurIndex;

		//bool mIsRenderingUI;

	private:
		friend void RegisterMetaData();
	};
}
