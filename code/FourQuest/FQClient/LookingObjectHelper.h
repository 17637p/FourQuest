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

			if (cloned == nullptr) // 새로 생성해서 복사본을 준다
			{
				cloned = game_module::ObjectPool::GetInstance()->Assign<LookingObjectHelper>(*this);
			}
			else // clone에 데이터를 복사한다.
			{
				// 기본 대입 연산자 호출한다.
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

