#pragma once

#include "../FQGameModule/GameModule.h"
#include "../FQGameModule/Component.h"

namespace fq::client
{
	class EffectColorManager : public fq::game_module::Component
	{
		using EventHandler = fq::game_module::EventHandler;

	public:
		EffectColorManager() = default;
		EffectColorManager(const EffectColorManager& other);
		EffectColorManager& operator=(const EffectColorManager& other);
		~EffectColorManager() = default;

		void OnAwake() override;
		
		void OnGeneratePrefabInstance(const fq::event::OnGeneratePrefabInstance& data);

		void SetColor(fq::game_module::GameObject* ownerObject, fq::game_module::GameObject* effectObject);

	private:
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */) const override;
		entt::meta_handle GetHandle() override { return *this; }

	private:
		// 지정된 색상 4가지
		// 프리팹 생성에 대한 핸들러
		EventHandler mOnGeneratePrefabInstance;

		DirectX::SimpleMath::Color mKnightBaseColor;
		DirectX::SimpleMath::Color mKnightEmissiveColor;
		DirectX::SimpleMath::Color mBerserkerBaseColor;
		DirectX::SimpleMath::Color mBerserkerEmissiveColor;
		DirectX::SimpleMath::Color mArcherBaseColor;
		DirectX::SimpleMath::Color mArcherEmissiveColor;
		DirectX::SimpleMath::Color mMagicBaseColor;
		DirectX::SimpleMath::Color mMagicEmissiveColor;

		friend void RegisterMetaData();
	};
}

