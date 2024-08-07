#pragma once
#include "Component.h"
#include "../FQCommon/FQCommonGraphics.h"

namespace fq::graphics
{
	class ITextObject;
}

namespace fq::game_module
{
	class TextUI : public Component
	{
	public:
		TextUI();
		~TextUI();

		virtual void OnUpdate(float dt) override;
		
		fq::graphics::TextInfo GetTextInfo() const { return mTextInfo; }
		void SetTextInfo(fq::graphics::TextInfo val);

		void SetTextPosition(float startX, float startY);

		fq::graphics::ITextObject* GetTextObject() const { return mTextObject; }
		void SetTextObject(fq::graphics::ITextObject* val) { mTextObject = val; }
	private:
		entt::meta_handle GetHandle() override { return *this; }
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

	private:
		fq::graphics::ITextObject* mTextObject;
		fq::graphics::TextInfo mTextInfo;
	};
}

