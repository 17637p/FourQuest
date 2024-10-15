#pragma once

#include "../FQGameModule/GameModule.h"
#include "../FQGameModule/Component.h"

namespace fq::client
{
	class MaterialManagedGroup : public fq::game_module::Component
	{
	public:
		MaterialManagedGroup();
		MaterialManagedGroup(const MaterialManagedGroup& other);
		MaterialManagedGroup& operator=(const MaterialManagedGroup& other);
		~MaterialManagedGroup() = default;

		const std::vector<game_module::GameObject*>& GetStaticMeshObjects() const { return mStaticMeshObjects; }
		const bool GetUsed() const { return mbUsed; }

	private:
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */) const override;
		entt::meta_handle GetHandle() override { return *this; }

		void OnStart() override;

	private:
		// �ڽ� ������Ʈ �߿� static�� �༮�� ��� ���� 
		std::vector<game_module::GameObject*> mStaticMeshObjects;

		bool mbUsed;

		friend void RegisterMetaData();
	};
}

