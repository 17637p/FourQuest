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
		enum EPlayerID
		{
			Sword1P,
			Statff1P,
			Axe1P,
			Bow1P,
			Sword2P,
			Statff2P,
			Axe2P,
			Bow2P,
			Sword3P,
			Statff3P,
			Axe3P,
			Bow3P,
			Sword4P,
			Statff4P,
			Axe4P,
			Bow4P,
		};

	public:
		

	private:
		entt::meta_handle GetHandle() override { return *this; }

		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;
		void OnStart() override;
		void OnUpdate(float dt) override;

	private:
		EPlayerID mImageIndex;

		float mbIsVisible;
		
		float mWorldOffset;
		DirectX::SimpleMath::Vector2 mOffset;
		game_module::Transform* mTransform;
		game_module::ImageUI* mImageUI;
		game_module::Camera* mMainCamera;

		friend void RegisterMetaData();
	};


}