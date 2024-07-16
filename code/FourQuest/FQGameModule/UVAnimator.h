#pragma once
#include "Component.h"
#include "../FQCommon/IFQRenderResource.h"

namespace fq::game_module
{
	/// <summary>
	/// UV 애니메이션을 관리하는 컴포넌트
	/// </summary>
	class UVAnimator : public Component
	{
		using ControllerPath = std::string;
		using ParameterID = unsigned int;

	public:
		UVAnimator();
		~UVAnimator() = default;

		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

		void UpdateTimePos(float dt);
		void SetTimePos(float dt);

		void SetUVAnimationPath(const std::string& path) { mUVAnimationPath = path; }
		const std::string GetUVAnimationPath() const { return mUVAnimationPath; }

		void SetIUVAnimation(std::shared_ptr<fq::graphics::IUVAnimation> iUVAnimation) { mIUVAnimation = iUVAnimation; }
		std::shared_ptr<fq::graphics::IUVAnimation> GetIUVAnimation() const { return mIUVAnimation; }

		void SetIUVAnimationInstance(std::shared_ptr<fq::graphics::IUVAnimationInstance> iUVAnimationInstance) { mIUVAnimationInstance = iUVAnimationInstance; }
		std::shared_ptr<fq::graphics::IUVAnimationInstance> GetIUVAnimationInstance() const { return mIUVAnimationInstance; }

	private:
		entt::meta_handle GetHandle() override { return *this; }

	private:
		std::string mUVAnimationPath;
		std::shared_ptr<fq::graphics::IUVAnimation> mIUVAnimation;
		std::shared_ptr<fq::graphics::IUVAnimationInstance> mIUVAnimationInstance;
	};
}

