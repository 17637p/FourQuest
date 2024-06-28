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

		/// <summary>
		/// ���纻�� ��ȯ�մϴ� 
		/// </summary>
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;
	
		std::vector<fq::graphics::IImageObject*>& GetImageObjects() { return mImageObjects; }
		std::vector<fq::graphics::UIInfo> GetUIInfomations() const { return mUIInfomations; }
		
		void SetUIInfomations(std::vector<fq::graphics::UIInfo> infomations);
		void SetUIInfomation(size_t index, const fq::graphics::UIInfo& infomation);

	private:
		entt::meta_handle GetHandle() override { return *this; }

		// Editor ����
		void setUIInfomations(std::vector<fq::graphics::UIInfo> infomations);

	private:
		std::vector<graphics::UIInfo> mUIInfomations;
		std::vector<graphics::IImageObject*> mImageObjects;

		friend void RegisterMetaData();
	};
}