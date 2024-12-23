#pragma once
#include "../FQGameModule/Component.h"
#include "../FQGameModule/GameModule.h"
#include "../FQGameModule/PrefabResource.h"

#include "Quest.h"

#include <list>
#include <queue>

namespace fq::game_module
{
	class TextUI;
	class ImageUI;
}

namespace fq::client
{
	struct StartSubQuestIndex
	{
		int index = 0;
	};

	struct StartQuests
	{
		int startMainQuestIndex = 0;
		std::vector<StartSubQuestIndex> startSubQuestIndex;
	};

	class MonsterGroup;
	class QuestManager : public game_module::Component
	{
	public:
		QuestManager();
		~QuestManager();

		// 대입 연산자를 자동으로 만들어 주지 않으므로 추가 (EventHandler 때문인데 이유는 모르겠음)
		QuestManager(const QuestManager& other);
		QuestManager& operator=(const QuestManager& other);

		virtual void OnStart() override;
		virtual void OnUpdate(float dt) override;
		virtual void OnDestroy() override;

		std::vector<Quest> GetMainQuests() const { return mMainQuests; }
		std::vector<Quest> GetSubQuests() const { return mSubQuests; }

		void ViewQuestInformation(Quest quest, game_module::TextUI* textUI);
		void RenderOnSubQuest(int i, bool isOn);
		void RenderOnAllSubQuest();
		void SpawnArmour(fq::game_module::PrefabResource armour);

	private:
		entt::meta_handle GetHandle() override { return *this; }
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

		void setScaleAndPositionScreen();

		void eventProcessKillMonster();
		void eventProcessPlayerCollideTrigger();
		void eventProcessCompleteDefence();
		void eventProcessClearQuest();
		void eventProcessAllColliderTrigger();
		void eventProcessObjectInteraction();
		void eventProcessClearGoddessStatue();
		void eventProcessChangePlayerNumCollideTrigger();
		void eventProcessInProgressDefence();
		void eventProcessPushButtonEvent();
		void eventProcessUpdatePlayerState();

		void eventProcessOnUIRender();

		void playNew(float dt);
		void playComplete(float dt);
		void PlayFail(float dt);
		void playTimer(int index, Quest& quest, float dt, bool isPlay); // isPlay: 연출 재생 

		void setColliderTriggerChecker();
		void setMonsterGroup();

		// Refactoring
		void initSTL();
		void startNew(int index);
		void startComplete(int index, float time = 3.0f);

		// pushButton
		int getMaxPlayer();
		int getXPushPlayer();
		int getRPushPlayer();
		int getAPushPlayer();
		int getYPushPlayer();

		void initPushPlayer(std::vector<bool>& pushVec);

		void checkAllPushButton();

	private:
		// 인스펙터 용
		StartQuests mStartQuests;

		std::vector<Quest> mMainQuests;
		std::vector<Quest> mSubQuests;

		float mArmourSpawnDistance;
		fq::game_module::PrefabResource mPortalPrefab;

		// 스크립트 용
		Quest mCurMainQuest;
		Quest mViewMainQuest; // 연출용 UI 보여줄 이전 퀘스트 
		std::vector<Quest> mCurSubQuest;
		std::vector<Quest> mViewSubQuest; // 연출용 UI 보여줄 서브 퀘스트 목록 
		std::list<Quest> mNextSubQuests;

		/// OnStart 시 채워주는 것들 (복사 안해도 되는 거)
		// 각종 이벤트 처리
		game_module::EventHandler mKillMonsterHandler;
		game_module::EventHandler mPlayerCollideTriggerHandler;
		game_module::EventHandler mCompleteDefenceHandler;
		game_module::EventHandler mClearQuestHandler;
		game_module::EventHandler mAllCollideTriggerHandler;
		game_module::EventHandler mObjectInteractionHandler;
		game_module::EventHandler mClearGoddessStatueHandler;
		game_module::EventHandler mChangePlayerNumCollideTriggereHandler;
		game_module::EventHandler mInProgressDefenceHandler;
		game_module::EventHandler mPushButtonHandler;
		game_module::EventHandler mUpdatePlayerStateHandler;
		game_module::EventHandler mUIRenderHandler;

		// ClearCondition - ClearQuest 전용
		std::vector<Quest> mClearEvents; // 이벤트 안에서 이벤트 호출이 안되어서 다음 틱에 처리
		std::vector<int> mClearEventIndexes;

		// SubQuest Timer Defence 진행중일 때는 멈추도록 설정
		std::vector<bool> mIsInProgressDefence;
		std::vector<bool> mIsInProgressDefenceView;

		// TextUI
		game_module::TextUI* mMainQuestText;
		std::vector<game_module::TextUI*> mSubQuestTexts;

		std::vector<game_module::ImageUI*> mQuestBoxes;

		// 신규 퀘스트 연출
		std::vector<game_module::ImageUI*> mNewImages;
		std::vector<float> mNewImageCounts;

		std::vector<game_module::ImageUI*> mCompleteImages;
		std::vector<float> mCompleteImageCounts;
		std::vector<game_module::ImageUI*> mLeftChecks;
		std::vector<game_module::ImageUI*> mRightChecks;
		std::vector<bool> mIsFinishedCompleteAnimation;

		// Fail은 서브 퀘스트만 사용
		std::vector<game_module::ImageUI*> mFailImages;
		std::vector<float> mFailImageCounts;
		std::vector<bool> mIsFinishedFailedAnimation;

		// Gauge Width
		int mGaugeMaxWidth;
		int mFontSize;
		game_module::ScreenManager* mScreenManager;

		// 버튼 퀘스트 용
		std::vector<bool> mIsAlive;
		std::vector<bool> mIsUseX;
		std::vector<bool> mIsUseY;
		std::vector<bool> mIsUseR;
		std::vector<bool> mIsUseA;

		std::queue<Quest> mRemoveViewQuests;

		bool mIsRenderingUI;

		friend void RegisterMetaData();
	};
}

