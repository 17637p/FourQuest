#pragma once
#include "Component.h"
#include "../FQCommon/FQCommonGraphics.h"

namespace fq::graphics
{
	class ISpriteAnimationObject;
}

namespace fq::game_module
{
	class SpriteAnimationUI : public Component
	{
	public:
		SpriteAnimationUI();
		~SpriteAnimationUI();

		virtual void OnUpdate(float dt) override;

		fq::graphics::SpriteInfo GetSpriteInfo() const { return mSpriteAnimationInfo; }
		void SetSpriteInfo(fq::graphics::SpriteInfo val);

		void SetIsRender(bool isRender);

		void SetPosition(float startX, float startY);
		void SetScale(float scaleX, float scaleY);

		fq::graphics::ISpriteAnimationObject* GetSpriteAnimationObject() const { return mSpriteAnimationObject; }
		void SetSpriteAnimationObject(fq::graphics::ISpriteAnimationObject* val) { mSpriteAnimationObject = val; }
	private:
		entt::meta_handle GetHandle() override { return *this; }
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

	private:
		fq::graphics::ISpriteAnimationObject* mSpriteAnimationObject;
		fq::graphics::SpriteInfo mSpriteAnimationInfo;

	private:
		friend void RegisterMetaData();
	};
}

