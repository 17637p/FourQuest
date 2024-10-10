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
		
		// Text Option 바꾸기
		fq::graphics::TextInfo GetTextInfo() const { return mTextInfo; }
		void SetTextInfo(fq::graphics::TextInfo val); // 에디터 용
		void SetTextInfoPlay(fq::graphics::TextInfo val); 

		void SetText(std::string text);
		void SetIsRender(bool isRender);

		void SetTextPosition(float startX, float startY);
		void SetTextScale(float scaleX, float scaleY);

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

