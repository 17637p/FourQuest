#pragma once
#include "../FQGameModule/Component.h"
#include "../FQGameModule/GameModuleEnum.h"

namespace fq::client
{
	class CollisionColorChanger : public game_module::Component
	{
	public:
		CollisionColorChanger() = default;
		~CollisionColorChanger() = default;

	private:
		entt::meta_handle GetHandle() override { return *this; }
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;
		void OnStart() override;
		void OnUpdate(float dt) override;

		void OnTriggerEnter(const game_module::Collision& collision) override;
		void OnTriggerExit(const game_module::Collision& collision) override;

		bool checkContainTag(const fq::game_module::ETag& tag) const;

	private:
		// 에디터용 데이터
		std::vector<fq::game_module::ETag> mTags; 
		bool mbUseBaseColor;
		DirectX::SimpleMath::Color mBaseColor;
		bool mbUseEmissiveColor;
		DirectX::SimpleMath::Color mEmissiveColor;

		int mCollisionCount; 

		// 로직 데이터
		std::vector<fq::game_module::GameObject*> mMeshGameObjects;

		friend void RegisterMetaData();
	};
}