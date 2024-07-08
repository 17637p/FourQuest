#include "LinkData.h"

namespace fq::game_module
{
	LinkData::LinkData()
		: mbIsDead(false)
		, mChildrenLinkData()
		, mID(0)
	{
	}

	LinkData::~LinkData()
	{
	}

	void LinkData::Update(const DirectX::SimpleMath::Matrix& parentWorldTransform)
	{
		mWorldTransform = mLinkInfo.localTransform * parentWorldTransform;

		for (auto it = mChildrenLinkData.begin(); it != mChildrenLinkData.end(); )
		{
			if (it->second->GetIsDead())
			{
				// 요소를 삭제하고 반복자를 갱신합니다.
				it = mChildrenLinkData.erase(it);
			}
			else
			{
				// 요소를 업데이트하고 다음 요소로 이동합니다.
				it->second->Update(mWorldTransform);
				++it;
			}
		}
	}
}
