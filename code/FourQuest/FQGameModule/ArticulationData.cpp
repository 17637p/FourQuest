#include "ArticulationData.h"

#include "LinkData.h"

namespace fq::game_module
{
	ArticulationData::ArticulationData()
		: mArticulationInfo()
		, mRootLinkData(std::make_shared<LinkData>())
	{
	}

	ArticulationData::~ArticulationData()
	{
	}

	void ArticulationData::Update()
	{
		mRootLinkData->Update();
	}
}
