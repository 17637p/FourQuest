#pragma once

#include "../FQCommon/FQCommonPhysics.h"

#include <memory>

namespace fq::game_module
{
	class LinkData;

	class ArticulationData
	{
		using LinkName = std::string;

	public:
		ArticulationData();
		~ArticulationData();
		
		inline const unsigned int& GetID();
		inline const unsigned int& GetLayerNumber();
		inline const DirectX::SimpleMath::Matrix& GetWorldTransform();
		inline const float& GetStaticFriction();
		inline const float& GetDynamicFriction();
		inline const float& GetRestitution();
		inline const float& GetDensity();
		inline void SetID(const unsigned int& id);
		inline void SetLayerNumber(const unsigned int& layerNumber);
		inline void SetWorldTransform(const DirectX::SimpleMath::Matrix& worldTransform);
		inline void SetStaticFriction(const float& staticFriction);
		inline void SetDynamicFriction(const float& dynamicFriction);
		inline void SetRestitution(const float& restitution);
		inline void SetDensity(const float& density);

		inline std::weak_ptr<LinkData> GetRootLinkData();

	private:
		fq::physics::ArticulationInfo mArticulationInfo;

		std::shared_ptr<LinkData> mRootLinkData;
	};


	const unsigned int& ArticulationData::GetID()
	{
		return mArticulationInfo.id;
	}
	const unsigned int& ArticulationData::GetLayerNumber()
	{
		return mArticulationInfo.layerNumber;
	}
	const DirectX::SimpleMath::Matrix& ArticulationData::GetWorldTransform()
	{
		return mArticulationInfo.worldTransform;
	}
	const float& ArticulationData::GetStaticFriction()
	{
		return mArticulationInfo.staticFriction;
	}
	const float& ArticulationData::GetDynamicFriction()
	{
		return mArticulationInfo.dynamicFriction;
	}
	const float& ArticulationData::GetRestitution()
	{
		return mArticulationInfo.restitution;
	}
	const float& ArticulationData::GetDensity()
	{
		return mArticulationInfo.density;
	}
	void ArticulationData::SetID(const unsigned int& id)
	{
		mArticulationInfo.id = id;
	}
	void ArticulationData::SetLayerNumber(const unsigned int& layerNumber)
	{
		mArticulationInfo.layerNumber = layerNumber;
	}
	void ArticulationData::SetWorldTransform(const DirectX::SimpleMath::Matrix& worldTransform)
	{
		mArticulationInfo.worldTransform = worldTransform;
	}
	void ArticulationData::SetStaticFriction(const float& staticFriction)
	{
		mArticulationInfo.staticFriction = staticFriction;
	}
	void ArticulationData::SetDynamicFriction(const float& dynamicFriction)
	{
		mArticulationInfo.dynamicFriction = dynamicFriction;
	}
	void ArticulationData::SetRestitution(const float& restitution)
	{
		mArticulationInfo.restitution = restitution;
	}
	void ArticulationData::SetDensity(const float& density)
	{
		mArticulationInfo.density = density;
	}

	std::weak_ptr<LinkData> ArticulationData::GetRootLinkData()
	{
		return mRootLinkData;
	}
}