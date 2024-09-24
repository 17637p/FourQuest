#pragma once
#include "../FQGameModule/Component.h"
#include "../FQGameModule/PrefabResource.h"

namespace fq::game_module
{
	class ScreenManager;
	class TextUI;
	class SpriteAnimationUI;
	class Transform;
}

namespace fq::client
{
	class SoulSelectUI : public game_module::Component
	{
	public:
		SoulSelectUI();
		~SoulSelectUI();

		virtual void OnStart() override;
		virtual void OnUpdate(float dt) override;

	private:
		void setScaleScreen();
		void setSpawnButton(int index, bool isSpawned);
		void setSoulBox(int index, bool isSpawned);
		void setReadyUI(int index, bool isSpawned);

		std::string wstringToString(std::wstring wStr);

		entt::meta_handle GetHandle() override { return *this; }
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;
		void SaveSoulType();
		void SetSelectPoints(int playerID, int selectNum);
		void MoveSoulDown(float dt);

		// 입력 처리 
		void processInput();
		void CheckAllReady(float dt);

	private:
		game_module::ScreenManager* mScreenManager;

		// 선택창
		std::vector<game_module::TextUI*> mSoulNameTexts;
		std::vector<game_module::TextUI*> mWeaponNameTexts;
		std::vector<game_module::TextUI*> mContentTexts;

		std::vector<game_module::SpriteAnimationUI*> mPlayerSoulIcons;

		std::vector<std::string> mSoulNames;
		std::vector<std::string> mWeapons;
		std::vector<std::string> mContents;

		// Ready 처리
		std::vector<bool> mIsReadys;
		std::vector<bool> mIsSelects;

		game_module::PrefabResource mSoulPrefab;
		game_module::PrefabResource mMagicSymbolPrefab;
		std::vector<std::shared_ptr<game_module::GameObject>> mSouls;
		std::vector<std::shared_ptr<game_module::GameObject>> mMagicSymbols;
		std::vector<int> mSelectSouls;
		std::vector<game_module::Transform*> mSelectPoints;

		// 시작 처리
		float mCurTime;
		float mChangeSceneTime;

		// Soul Move
		float mSoulMoveSpeed;

	private:
		friend void RegisterMetaData();
	};
}

