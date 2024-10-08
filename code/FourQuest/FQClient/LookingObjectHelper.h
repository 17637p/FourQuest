#pragma once

#include "Player.h"
#include "../FQGameModule/GameModule.h"
#include "../FQGameModule/Component.h"

namespace fq::client
{
	class LookingObjectHelper : public fq::game_module::Component
	{
	public:
		LookingObjectHelper() = default;
		LookingObjectHelper(const LookingObjectHelper& other) = default;
		LookingObjectHelper& operator=(const LookingObjectHelper& other) = default;
		~LookingObjectHelper() = default;

		void OnStart() override;
		void OnUpdate(float dt) override;

	private:
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */) const override
		{
			auto cloned = std::dynamic_pointer_cast<LookingObjectHelper>(clone);

			if (cloned == nullptr) // ���� �����ؼ� ���纻�� �ش�
			{
				cloned = game_module::ObjectPool::GetInstance()->Assign<LookingObjectHelper>(*this);
			}
			else // clone�� �����͸� �����Ѵ�.
			{
				// �⺻ ���� ������ ȣ���Ѵ�.
				*cloned = *this;
			}

			return cloned;
		}
		entt::meta_handle GetHandle() override { return *this; }

	private:
		std::string mLookingObjectName;
		std::shared_ptr<fq::game_module::GameObject> mLookingObjectOrNull;

		friend void RegisterMetaData();
	};
}

