#pragma once
#include "../FQGameModule/Component.h"

namespace fq::game_module
{
	class Transform;
	class ImageUI;
	class Camera;
}

namespace fq::client
{
	class PlayerCheckUI : public fq::game_module::Component
	{
	public:

	private:
		entt::meta_handle GetHandle() override { return *this; }

		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;
		void OnStart() override;
		void OnUpdate(float dt) override;

	private:
		float mbIsVisible;
		
		float mWorldOffset;
		DirectX::SimpleMath::Vector2 mOffset;
		game_module::Transform* mTransform;
		game_module::ImageUI* mImageUI;
		game_module::Camera* mMainCamera;



		friend void RegisterMetaData();
	};


}