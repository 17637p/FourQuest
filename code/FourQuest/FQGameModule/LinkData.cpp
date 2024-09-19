#include "LinkData.h"

#include "../FQCommon/FQCommonGraphics.h"



namespace fq::game_module
{
	LinkData::LinkData()
		: mbIsDead(false)
		, mShapeType(EShapeType::BOX)
		, mChildrenLinkData()
		, mID(0)
		, mSphereRadius(0.1f)
		, mCapsuleHalfHeight(0.1f)
		, mCapsuleRadius(0.1f)
		, mExtent{0.1f, 0.1f, 0.1f}
	{
	}

	LinkData::~LinkData()
	{
	}

	void LinkData::Update(const DirectX::SimpleMath::Matrix& parentWorldTransform)
	{
		mParentWorldTransform = parentWorldTransform;
		mWorldTransform = mLinkInfo.localTransform * parentWorldTransform;
		mLinkInfo.worldTransform = mLinkInfo.localTransform * parentWorldTransform;

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
