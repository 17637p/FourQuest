#pragma once

#include "../FQGameModule/GameModule.h"
#include "../FQGameModule/Component.h"

namespace fq::client
{
	class MaterialManager : public fq::game_module::Component
	{
	public:
		MaterialManager() = default;
		MaterialManager(const MaterialManager& other);
		MaterialManager& operator=(const MaterialManager& other);
		~MaterialManager() = default;

		void SetManagerMaterialOrNull(std::shared_ptr<fq::graphics::IMaterial> managerMaterialOrNull);

	private:
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */) const override;
		entt::meta_handle GetHandle() override { return *this; }

		void OnUpdate(float dt) override;
		void OnStart() override;

	private:
		game_module::EventHandler mRegisterPlayerHandler;
		
		class GameManager* mGameManager;

		std::vector<std::shared_ptr<game_module::GameObject>> mStaticMeshObjects;

		std::map<game_module::GameObject*, std::vector<std::shared_ptr<fq::graphics::IMaterial>>> mMaterialCache;

		std::string mManagerMaterialPath;
		std::shared_ptr<fq::graphics::IMaterial> mManagerMaterialOrNull;

		DirectX::SimpleMath::Vector3 mWorldPointOffset;
		DirectX::SimpleMath::Vector3 mLocalPointOffset;

		bool mbUseDebugDraw;

		friend void RegisterMetaData();
	};
}

