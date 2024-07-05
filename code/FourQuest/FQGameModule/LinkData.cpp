#include "LinkData.h"

namespace fq::game_module
{
	LinkData::LinkData()
		: mLinkInfo()
		, mbIsDead(false)
		, mChildrenLinkData()
		, mID(0)
	{
	}

	LinkData::~LinkData()
	{
	}

	void LinkData::Update()
	{
		for (auto& link : mChildrenLinkData)
		{
			if (link.second->GetIsDead())
			{
				mChildrenLinkData.erase(mChildrenLinkData.find(link.first));
			}

			link.second->Update();
		}
	}
}
