#pragma once
#include "../FQGameModule/Component.h"

namespace fq::game_module
{
	class ScreenManager;
	class ImageUI;
	class TextUI;
}

namespace fq::client
{
	class ResultUI : public game_module::Component
	{
	public:
		ResultUI();
		~ResultUI();

		virtual void OnStart() override;
		virtual void OnUpdate(float dt) override;

		void SetNextScene(std::string nextScene);

	private:
		entt::meta_handle GetHandle() override { return *this; }
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

		void setScaleScreen();

		void initBar();
		void OnOffBar(); // 없는 캐릭터 바 렌더 지우기 
		void setTotal();
		void setBar();

	private:
		game_module::ScreenManager* mScreenManager;

		std::vector<game_module::ImageUI*> mMonsterBars;
		std::vector<game_module::ImageUI*> mKnightBars;
		std::vector<game_module::ImageUI*> mMagicBars;
		std::vector<game_module::ImageUI*> mArcherBars;
		std::vector<game_module::ImageUI*> mWarriorBars;

		std::vector<game_module::TextUI*> mTotalTexts;

		std::vector<bool> mIsPlayers;

		std::vector<float> mMonsterRatios;
		std::vector<float> mKnightRatios;
		std::vector<float> mMagicRatios;
		std::vector<float> mWarriorRatios;
		std::vector<float> mArcherRatios;

		int mTotalKillMonster;
		int mTotalUseKnight;
		int mTotalUseMagic;
		int mTotalUseWarrior;
		int mTotalUseArcher;

		std::vector<int> mKillMonsterCount;
		std::vector<int> mUseKnightCount;
		std::vector<int> mUseMagicCount;
		std::vector<int> mUseArcherCount;
		std::vector<int> mUseWarriorCount;

		float mBarWidth;

		std::string mNextScene;

	private:
		friend void RegisterMetaData();
	};
}

//기사, 마법사, 워리어, 궁수 

