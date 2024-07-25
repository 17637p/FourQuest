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
	private:
		entt::meta_handle GetHandle() override { return *this; }

		// Editor 전용
		void setUIInfomations(std::vector<fq::graphics::UIInfo> infomations);

	private:
		std::vector<graphics::UIInfo> mUIInfomations;
		std::vector<graphics::IImageObject*> mImageObjects;

		friend void RegisterMetaData();
	};
}