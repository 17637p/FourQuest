#pragma once
#pragma once

#include "Component.h"

#include "../FQCommon/FQCommonGraphics.h"

namespace fq::game_module
{
	class PostProcessing : public Component
	{
	public:
		PostProcessing() = default;
		~PostProcessing() = default;

		/// <summary>
		/// 복사본을 반환합니다 
		/// </summary>
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

		fq::graphics::PostProcessingInfo GetPostProcessingInfo() const { return mPostProcessinfo; }
		void SetPostProcessingInfo(fq::graphics::PostProcessingInfo postProcessingInfo) { mPostProcessinfo = postProcessingInfo; }

	private:
		entt::meta_handle GetHandle() override;

	private:
		fq::graphics::PostProcessingInfo mPostProcessinfo;
	};

}
