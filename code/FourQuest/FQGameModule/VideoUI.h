#pragma once
#include "Component.h"
#include "../FQCommon/FQCommonGraphics.h"

namespace fq::graphics
{
	class IVideoObject;
}

namespace fq::game_module
{
	class VideoUI : public Component
	{
	public:
		VideoUI();
		~VideoUI();

		virtual void OnStart() override;
		virtual void OnUpdate(float dt) override;

		// Text Option 바꾸기
		fq::graphics::VideoInfo GetVideoInfo() const { return mVIdeoInfo; }
		void SetVideoInfo(fq::graphics::VideoInfo val); // 에디터 용

		void SetIsRender(bool isRender);

		void SetTextPosition(float startX, float startY);

		fq::graphics::IVideoObject* GetVideoObject() const { return mVideoObject; }
		void SetVideoObject(fq::graphics::IVideoObject* val) { mVideoObject = val; }

	private:
		entt::meta_handle GetHandle() override { return *this; }
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

	private:
		fq::graphics::IVideoObject* mVideoObject;
		fq::graphics::VideoInfo mVIdeoInfo;

		friend void RegisterMetaData();
	};
}
