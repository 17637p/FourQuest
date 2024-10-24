#pragma once
#include "../FQGameModule/Component.h"
#include "../FQGameModule/GameModuleEnum.h"

namespace fq::client
{
	struct TagStructure
	{
		fq::game_module::ETag Tag;
	};

	class CollisionRenderChanger : public game_module::Component
	{
	public:
		CollisionRenderChanger() = default;
		~CollisionRenderChanger() = default;

	private:
		entt::meta_handle GetHandle() override { return *this; }
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;
		void OnStart() override;
		void OnUpdate(float dt) override;

		void OnTriggerEnter(const game_module::Collision& collision) override;
		void OnTriggerExit(const game_module::Collision& collision) override;

		bool checkContainTag(const fq::game_module::ETag& tag) const;

	private:
		// �����Ϳ� ������
		bool mbIsRender;
		std::vector<TagStructure> mTags;
		int mCollisionCount;

		// ���� ������
		std::vector<fq::game_module::GameObject*> mMeshGameObjects;

		friend void RegisterMetaData();
	};
}