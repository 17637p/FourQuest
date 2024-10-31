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
	struct LoadingText
	{
		std::string Title = "";
		std::string Content = "";
	};

	class LoadingUI : public game_module::Component
	{
	public:
		LoadingUI();
		~LoadingUI();

		virtual void OnStart() override;
		virtual void OnUpdate(float dt) override;

		void SetProgressBar(float progress);
		void SetScaleScreen();

		// ImageAndText Random을 설정하기 위해 필요한 함수 
		int GetGuideSize() { return mTexts.size(); } // 최대 이미지, 텍스트 수
		void SetGuideID(int id) { mCurGuideID = id; } // 선택할 이미지, 텍스트 Number

	private:
		entt::meta_handle GetHandle() override { return *this; }
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

	private:
		float mProgressBarMaxWidth;
		game_module::ImageUI* mProgressBar;
		game_module::Transform* mProgressSoul;

		int mCurGuideID;
		std::vector<game_module::ImageUI*> mImages;
		std::vector<game_module::TextUI*> mTitles;
		std::vector<game_module::TextUI*> mContents;

		std::vector<LoadingText> mTexts;

		game_module::ScreenManager* mScreenManager;

	private:
		friend void RegisterMetaData();
	};
}

