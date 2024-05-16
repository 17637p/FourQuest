#include "CharacterJoint.h"

#include "CharacterLink.h"

namespace fq::physics
{
	CharacterJoint::CharacterJoint()
		: mOwnerLink(nullptr)
		, mParentLink(nullptr)
		, mLocalTransform()
		, mPxJoint(nullptr)
		, mDrive()
		, mSwing1Limit()
		, mSwing2Limit()
		, mTwistLimit()
	{
	}

	CharacterJoint::~CharacterJoint()
	{
	}

	bool CharacterJoint::Initialize(CharacterLink* parentLink, CharacterLink* ownerLink, const CharacterJointInfo& info)
	{
		mParentLink = parentLink;
		mOwnerLink = ownerLink;

		return true;
	}
}
