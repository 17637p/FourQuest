#pragma once

#include "../FQGameModule/GameModule.h"
#include "../FQGameModule/Component.h"

namespace fq::client
{
	/*
	����� �޽��� �ٿ�� ������ �÷��̾��� ���������� ó����
	�߰������� TransparentMaterialGroup ������Ʈ �ϳ� ���� ó�� ���� ������Ʈ �����ϴ� �͵� ������
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

