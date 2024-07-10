#include "ArticulationData.h"

#include "LinkData.h"

namespace fq::game_module
{
	ArticulationData::ArticulationData()
		: mArticulationInfo()
		, mRootLinkData(std::make_shared<LinkData>())
	{
		mRootLinkData->SetBoneName("root");
	}

	ArticulationData::~ArticulationData()
	{
	}

	void ArticulationData::Update()
	{
		mRootLinkData->Update(mArticulationInfo.worldTransform);
	}
}
