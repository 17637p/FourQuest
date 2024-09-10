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

		// ImageAndText Random�� �����ϱ� ���� �ʿ��� �Լ� 
		int GetGuideSize() { return mTexts.size(); } // �ִ� �̹���, �ؽ�Ʈ ��
		void SetGuideID(int id) { mCurGuideID = id; } // ������ �̹���, �ؽ�Ʈ Number

	private:
		entt::meta_handle GetHandle() override { return *this; }
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

	private:
		float mProgressBarMaxWidth;
		game_module::ImageUI* mProgressBar;
		game_module::Transform* mProgressSoul;

		int mCurGuideID;
		std::vector<game_module::GameObject*> mImages;
		game_module::TextUI* mTitle;
		game_module::TextUI* mContent;

		game_module::TextUI* mBackgroundTitle;
		game_module::TextUI* mBackgroundContent;

		std::vector<LoadingText> mTexts;

		game_module::ScreenManager* mScreenManager;

	private:
		friend void RegisterMetaData();
	};
}

