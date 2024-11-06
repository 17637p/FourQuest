#pragma once
#include "Component.h"

#include "../FQCommon/FQCommonGraphics.h"
#include "../FQCommon/IFQRenderObject.h"

namespace fq::game_module
{
	class ImageUI : public Component
	{
	public:
		ImageUI();
		~ImageUI();

		virtual void OnUpdate(float dt) override;
		/// <summary>
		/// 복사본을 반환합니다 
		/// </summary>
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;
	
		std::vector<fq::graphics::IImageObject*>& GetImageObjects() { return mImageObjects; }

		std::vector<fq::graphics::UIInfo> GetUIInfomations() const { return mUIInfomations; }
		fq::graphics::UIInfo GetUIInfomation(int index) const { return mUIInfomations[index]; }

		void SetUIInfomations(std::vector<fq::graphics::UIInfo> infomations);
		void SetUIInfomation(size_t index, const fq::graphics::UIInfo& infomation);

		void SetUIPosition(size_t index, float StartX, float StartY);
		void SetUIScale(size_t index, float scaleX, float scaleY);
		void SetIsRender(size_t index, bool isRender);
		void SetFillDegree(size_t index, float degree);

		bool IsBindTransform() const { return mbIsBindTransform; }
		void SetBindTransform(bool isBind) { mbIsBindTransform = isBind; }

		bool GetIsApplyUIRenderEvent() const { return mbIsApplyUIRenderEvent; }

	private:
		entt::meta_handle GetHandle() override { return *this; }

		// Editor 전용
		void setUIInfomations(std::vector<fq::graphics::UIInfo> infomations);
		void setScreenRatioPosition();
		void setScaleScreen();

	private:
		std::vector<graphics::UIInfo> mUIInfomations;
		std::vector<graphics::IImageObject*> mImageObjects;

		bool mbIsBindTransform;
		bool mbUssScreenScale;
		bool mbIsApplyUIRenderEvent; // UIRender 이벤트 적용 여부, 추가_홍지환

		bool mbUseScreenRatioPosition; 
		DirectX::SimpleMath::Vector2 mScreenRatioPosition; // 0.f ~ 1.f

		friend void RegisterMetaData();
	};
}