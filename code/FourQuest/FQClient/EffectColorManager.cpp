#include "EffectColorManager.h"
#include "ClientEvent.h"
#include "GameManager.h"
#include "Player.h"
#include "EffectColorTransmitter.h"
#include "Soul.h"

#include "../FQGameModule/StaticMeshRenderer.h"
#include "../FQGameModule/Camera.h"
#include "../FQGameModule/Transform.h"
#include "../FQGameModule/CharacterController.h"
#include "../FQGameModule/Decal.h"
#include "../FQGameModule/Particle.h"
#include "../FQCommon/FQCommonGraphics.h"

fq::client::EffectColorManager::EffectColorManager(const EffectColorManager& other)
	: mOnGeneratePrefabInstance{}
	, mKnightBaseColor{ other.mKnightBaseColor }
	, mKnightEmissiveColor{ other.mKnightEmissiveColor }
	, mBerserkerBaseColor{ other.mBerserkerBaseColor }
	, mBerserkerEmissiveColor{ other.mBerserkerEmissiveColor }
	, mArcherBaseColor{ other.mArcherBaseColor }
	, mArcherEmissiveColor{ other.mArcherEmissiveColor }
	, mMagicBaseColor{ other.mMagicBaseColor }
	, mMagicEmissiveColor{ other.mMagicEmissiveColor }
{
}

fq::client::EffectColorManager& fq::client::EffectColorManager::operator=(const EffectColorManager& other)
{
	if (this == &other)
	{
		return *this;
	}

	mKnightBaseColor = other.mKnightBaseColor;
	mKnightEmissiveColor = other.mKnightEmissiveColor;
	mBerserkerBaseColor = other.mBerserkerBaseColor;
	mBerserkerEmissiveColor = other.mBerserkerEmissiveColor;
	mArcherBaseColor = other.mArcherBaseColor;
	mArcherEmissiveColor = other.mArcherEmissiveColor;
	mMagicBaseColor = other.mMagicBaseColor;
	mMagicEmissiveColor = other.mMagicEmissiveColor;

	return *this;
}

void fq::client::EffectColorManager::OnAwake()
{
	mOnGeneratePrefabInstance = GetScene()->GetEventManager()->RegisterHandle<fq::event::OnGeneratePrefabInstance>(this, &EffectColorManager::OnGeneratePrefabInstance);
}

void fq::client::EffectColorManager::OnGeneratePrefabInstance(const fq::event::OnGeneratePrefabInstance& data)
{
	SetColor(data.EventSendObject, data.PrefabGameObject.get());
}

void fq::client::EffectColorManager::SetColor(fq::game_module::GameObject* ownerObject, fq::game_module::GameObject* effectObject)
{
	auto soul = ownerObject->GetComponent<Soul>();
	auto playerPtr = ownerObject->GetComponent<Player>();
	auto effectColorTrasmitter = ownerObject->GetComponent<EffectColorTransmitter>();

	ESoulType soulType = ESoulType::End;

	if (playerPtr != nullptr)
	{
		soulType = playerPtr->GetSoulType();
	}
	else if (soul != nullptr)
	{
		soulType = soul->GetSoulType();
	}
	else if (effectColorTrasmitter != nullptr)
	{
		soulType = effectColorTrasmitter->GetSoulType();
	}
	else
	{
		return;
	}

	DirectX::SimpleMath::Color applyBaseColor;
	DirectX::SimpleMath::Color applyEmissiveColor;

	switch (soulType)
	{
	case ESoulType::Sword:
		applyBaseColor = mKnightBaseColor;
		applyEmissiveColor = mKnightEmissiveColor;
		break;
	case ESoulType::Staff:
		applyBaseColor = mMagicBaseColor;
		applyEmissiveColor = mMagicEmissiveColor;
		break;
	case ESoulType::Axe:
		applyBaseColor = mBerserkerBaseColor;
		applyEmissiveColor = mBerserkerEmissiveColor;
		break;
	case ESoulType::Bow:
		applyBaseColor = mArcherBaseColor;
		applyEmissiveColor = mArcherEmissiveColor;
		break;
	default:
		assert(false);
		break;
	}

	// 알파값은 기존의 값을 유지함
	std::function<void(fq::game_module::GameObject*)> setColor = [&setColor, applyBaseColor, applyEmissiveColor](fq::game_module::GameObject* gameObject)
		{
			auto staticMeshRenderer = gameObject->GetComponent<fq::game_module::StaticMeshRenderer>();

			if (staticMeshRenderer != nullptr)
			{
				auto materialInstanceInfo = staticMeshRenderer->GetMaterialInstanceInfo();

				materialInstanceInfo.bUseBaseColor = true;
				materialInstanceInfo.BaseColor.x = applyBaseColor.x;
				materialInstanceInfo.BaseColor.y = applyBaseColor.y;
				materialInstanceInfo.BaseColor.z = applyBaseColor.z;
				materialInstanceInfo.bUseEmissiveColor = true;
				materialInstanceInfo.EmissiveColor.x = applyEmissiveColor.x;
				materialInstanceInfo.EmissiveColor.y = applyEmissiveColor.y;
				materialInstanceInfo.EmissiveColor.z = applyEmissiveColor.z;

				staticMeshRenderer->SetMaterialInstanceInfo(materialInstanceInfo);
			}

			auto decalOrNull = gameObject->GetComponent<fq::game_module::Decal>();

			if (decalOrNull != nullptr)
			{
				auto materialInfo = decalOrNull->GetDecalMaterialInfo();
				materialInfo.BaseColor.x = applyBaseColor.x;
				materialInfo.BaseColor.y = applyBaseColor.y;
				materialInfo.BaseColor.z = applyBaseColor.z;
				materialInfo.EmissiveColor.x = applyEmissiveColor.x;
				materialInfo.EmissiveColor.y = applyEmissiveColor.y;
				materialInfo.EmissiveColor.z = applyEmissiveColor.z;
				decalOrNull->SetDecalMaterialInfo(materialInfo);
			}

			auto particleOrNull = gameObject->GetComponent<fq::game_module::Particle>();

			if (particleOrNull)
			{
				auto info = particleOrNull->GetParticleMaterialInfo();

				info.BaseColor.x = applyBaseColor.x;
				info.BaseColor.y = applyBaseColor.y;
				info.BaseColor.z = applyBaseColor.z;
				info.EmissiveColor.x = applyEmissiveColor.x;
				info.EmissiveColor.y = applyEmissiveColor.y;
				info.EmissiveColor.z = applyEmissiveColor.z;

				particleOrNull->SetParticleMaterialInfo(info);
			}

			for (auto child : gameObject->GetChildren())
			{
				setColor(child);
			}
		};

	setColor(effectObject);
}

std::shared_ptr<fq::game_module::Component> fq::client::EffectColorManager::Clone(std::shared_ptr<fq::game_module::Component> clone) const
{
	auto cloned = std::dynamic_pointer_cast<EffectColorManager>(clone);

	if (cloned == nullptr) // 새로 생성해서 복사본을 준다
	{
		cloned = game_module::ObjectPool::GetInstance()->Assign<EffectColorManager>(*this);
	}
	else // clone에 데이터를 복사한다.
	{
		// 기본 대입 연산자 호출한다.
		*cloned = *this;
	}

	return cloned;
}
