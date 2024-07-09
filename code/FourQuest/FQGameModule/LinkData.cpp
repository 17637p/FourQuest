#include "LinkData.h"

namespace fq::game_module
{
	LinkData::LinkData()
		: mbIsDead(false)
		, mChildrenLinkData()
		, mID(0)
		, mSphereRadius(1.f)
		, mCapsuleHalfHeight(1.f)
		, mCapsuleRadius(1.f)
		, mExtent{1.f, 1.f, 1.f}
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
				// ��Ҹ� �����ϰ� �ݺ��ڸ� �����մϴ�.
				it = mChildrenLinkData.erase(it);
			}
			else
			{
				// ��Ҹ� ������Ʈ�ϰ� ���� ��ҷ� �̵��մϴ�.
				it->second->Update(mWorldTransform);
				++it;
			}
		}
	}
}
