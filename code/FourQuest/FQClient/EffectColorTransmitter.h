#pragma once

#include "Player.h"
#include "../FQGameModule/GameModule.h"
#include "../FQGameModule/Component.h"

namespace fq::client
{
	class EffectColorTransmitter : public fq::game_module::Component
	{
	public:
		EffectColorTransmitter() = default;
		EffectColorTransmitter(const EffectColorTransmitter& other) = default;
		EffectColorTransmitter& operator=(const EffectColorTransmitter& other) = default;
		~EffectColorTransmitter() = default;

		ESoulType GetSoulType() const { return mSoulType; }
		void SetSoulType(ESoulType soulType) { mSoulType = soulType; }

	private:
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */) const override
		{
			auto cloned = std::dynamic_pointer_cast<EffectColorTransmitter>(clone);

			if (cloned == nullptr) // ���� �����ؼ� ���纻�� �ش�
			{
				cloned = game_module::ObjectPool::GetInstance()->Assign<EffectColorTransmitter>(*this);
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
		ESoulType mSoulType;

		friend void RegisterMetaData();
	};
}

