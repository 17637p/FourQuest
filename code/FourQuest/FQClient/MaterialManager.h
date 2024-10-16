#pragma once

#include "../FQGameModule/GameModule.h"
#include "../FQGameModule/Component.h"

namespace fq::client
{
	/*
	현재는 메쉬의 바운딩 볼륨과 플레이어의 반직선으로 처리중
	추가적으로 TransparentMaterialGroup 컴포넌트 하나 만들어서 처리 받을 오브젝트 조절하는 것도 좋을듯
	*/

	class MaterialManager : public fq::game_module::Component
	{
	public:
		MaterialManager() = default;
		MaterialManager(const MaterialManager& other) = default;
		MaterialManager& operator=(const MaterialManager& other) = default;
		~MaterialManager() = default;

		void SetManagerMaterialOrNull(std::shared_ptr<fq::graphics::IMaterial> managerMaterialOrNull);

	private:
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */) const override;
		entt::meta_handle GetHandle() override { return *this; }

		void OnUpdate(float dt) override;
		void OnStart() override;

	private:
		class GameManager* mGameManager;

		std::set<game_module::GameObject*> mIgnoredObjects;
		std::map<game_module::GameObject*, std::vector<std::shared_ptr<fq::graphics::IMaterial>>> mMaterialCache;

		std::string mManagerMaterialPath;
		std::shared_ptr<fq::graphics::IMaterial> mManagerMaterialOrNull;

		DirectX::SimpleMath::Vector3 mWorldPointOffset;
		DirectX::SimpleMath::Vector3 mLocalPointOffset;

		bool mbUseDebugDraw;

		bool mbUseAllStaticObjectCheck;
		bool mbUseMaterialGroup;

		friend void RegisterMetaData();
	};
}

