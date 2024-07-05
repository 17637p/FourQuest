#pragma once

#include "../FQCommon/FQCommonPhysics.h"

#include <memory>

namespace fq::game_module
{
	using LinkName = std::string;


	class LinkData
	{
	public:
		LinkData();
		~LinkData();

		void Update();

		inline const std::string& GetBoneName();
		inline const std::string& GetParentBoneName();
		inline const float& GetDensity();
		inline const DirectX::SimpleMath::Matrix GetLocalTransform();
		
		inline const DirectX::SimpleMath::Matrix& GetJointLocalTransform();
		inline const float& GetJointStiffness();
		inline const float& GetJointDamping();
		inline const float& GetJointMaxForce();

		inline const fq::physics::EArticulationMotion& GetSwing1AxisMotion();
		inline const float& GetSwing1LimitLow();
		inline const float& GetSwing1LimitHigh();
		inline const fq::physics::EArticulationMotion& GetSwing2AxisMotion();
		inline const float& GetSwing2LimitLow();
		inline const float& GetSwing2LimitHigh();
		inline const fq::physics::EArticulationMotion& GetTwistAxisMotion();
		inline const float& GetTwistLimitLow();
		inline const float& GetTwistLimitHigh();

		inline const bool& GetIsDead();
		inline const unsigned int& GetID();

		inline void SetBoneName(const std::string& name);
		inline void SetParentBoneName(const std::string& parentName);
		inline void SetDensity(const float& density);
		inline void SetLocalTransform(const DirectX::SimpleMath::Matrix localTransform);
					
		inline void SetJointLocalTransform(const DirectX::SimpleMath::Matrix& localTransform);
		inline void SetJointStiffness(const float& stiffness);
		inline void SetJointDamping(const float& damping);
		inline void SetJointMaxForce(const float& maxForce);
					
		inline void SetSwing1AxisMotion(const fq::physics::EArticulationMotion& motion);
		inline void SetSwing1LimitLow(const float& limitsLow);
		inline void SetSwing1LimitHigh(const float& limitsHigh);
		inline void SetSwing2AxisMotion(const fq::physics	::EArticulationMotion& motion);
		inline void SetSwing2LimitLow(const float& limitsLow);
		inline void SetSwing2LimitHigh(const float& limitsHigh);
		inline void SetTwistAxisMotion(const fq::physics::EArticulationMotion& motion);
		inline void SetTwistLimitLow(const float& limitsLow);
		inline void SetTwistLimitHigh(const float& limitsHigh);

		inline void SetIsDead(const bool& isDead);
		inline void SetID(const int& id);

		inline std::shared_ptr<LinkData> GetChildLinkData(LinkName name);
		inline std::unordered_map<LinkName, std::shared_ptr<LinkData>>& GetChildrenLinkData();
		inline void AddChildLinkData(LinkName name, std::shared_ptr<LinkData> linkData);
		inline void RemoveChildLinkData(LinkName name);

	private:
		fq::physics::LinkInfo mLinkInfo;
		bool mbIsDead;
		unsigned int mID;

		std::unordered_map<LinkName, std::shared_ptr<LinkData>> mChildrenLinkData;
	};

	const std::string& LinkData::GetBoneName()
	{
		return mLinkInfo.boneName;
	}
	const std::string& LinkData::GetParentBoneName()
	{
		return mLinkInfo.parentBoneName;
	}
	const float& LinkData::GetDensity()
	{
		return mLinkInfo.density;
	}
	const DirectX::SimpleMath::Matrix LinkData::GetLocalTransform()
	{
		return mLinkInfo.localTransform;
	}

	const DirectX::SimpleMath::Matrix& LinkData::GetJointLocalTransform()
	{
		return mLinkInfo.jointInfo.localTransform;
	}
	const float& LinkData::GetJointStiffness()
	{
		return mLinkInfo.jointInfo.stiffness;
	}
	const float& LinkData::GetJointDamping()
	{
		return mLinkInfo.jointInfo.damping;
	}
	const float& LinkData::GetJointMaxForce()
	{
		return mLinkInfo.jointInfo.maxForce;
	}

	const fq::physics::EArticulationMotion& LinkData::GetSwing1AxisMotion()
	{
		return mLinkInfo.jointInfo.Swing1AxisInfo.motion;
	}
	const float& LinkData::GetSwing1LimitLow()
	{
		return mLinkInfo.jointInfo.Swing1AxisInfo.limitsLow;
	}
	const float& LinkData::GetSwing1LimitHigh()
	{
		return mLinkInfo.jointInfo.Swing1AxisInfo.limitsHigh;
	}
	const fq::physics::EArticulationMotion& LinkData::GetSwing2AxisMotion()
	{
		return mLinkInfo.jointInfo.Swing2AxisInfo.motion;
	}
	const float& LinkData::GetSwing2LimitLow()
	{
		return mLinkInfo.jointInfo.Swing2AxisInfo.limitsLow;
	}
	const float& LinkData::GetSwing2LimitHigh()
	{
		return mLinkInfo.jointInfo.Swing2AxisInfo.limitsHigh;
	}
	const fq::physics::EArticulationMotion& LinkData::GetTwistAxisMotion()
	{
		return mLinkInfo.jointInfo.TwistAxisInfo.motion;
	}
	const float& LinkData::GetTwistLimitLow()
	{
		return mLinkInfo.jointInfo.TwistAxisInfo.limitsLow;
	}
	const float& LinkData::GetTwistLimitHigh()
	{
		return mLinkInfo.jointInfo.TwistAxisInfo.limitsHigh;
	}
	const bool& LinkData::GetIsDead()
	{
		return mbIsDead;
	}
	const unsigned int& LinkData::GetID()
	{
		return mID;
	}

	void LinkData::SetBoneName(const std::string& name)
	{
		mLinkInfo.boneName = name;
	}
	void LinkData::SetParentBoneName(const std::string& parentName)
	{
		mLinkInfo.parentBoneName = parentName;
	}
	void LinkData::SetDensity(const float& density)
	{
		mLinkInfo.density = density;
	}
	void LinkData::SetLocalTransform(const DirectX::SimpleMath::Matrix localTransform)
	{
		mLinkInfo.localTransform = localTransform;
	}
		 
	void LinkData::SetJointLocalTransform(const DirectX::SimpleMath::Matrix& localTransform)
	{
		mLinkInfo.jointInfo.localTransform = localTransform;
	}
	void LinkData::SetJointStiffness(const float& stiffness)
	{
		mLinkInfo.jointInfo.stiffness = stiffness;
	}
	void LinkData::SetJointDamping(const float& damping)
	{
		mLinkInfo.jointInfo.damping = damping;
	}
	void LinkData::SetJointMaxForce(const float& maxForce)
	{
		mLinkInfo.jointInfo.maxForce;
	}
		
	void LinkData::SetSwing1AxisMotion(const fq::physics::EArticulationMotion& motion)
	{
		mLinkInfo.jointInfo.Swing1AxisInfo.motion = motion;
	}
	void LinkData::SetSwing1LimitLow(const float& limitsLow)
	{
		mLinkInfo.jointInfo.Swing1AxisInfo.limitsLow = limitsLow;
	}
	void LinkData::SetSwing1LimitHigh(const float& limitsHigh)
	{
		mLinkInfo.jointInfo.Swing1AxisInfo.limitsHigh = limitsHigh;
	}
	void LinkData::SetSwing2AxisMotion(const fq::physics::EArticulationMotion& motion)
	{
		mLinkInfo.jointInfo.Swing2AxisInfo.motion = motion;
	}
	void LinkData::SetSwing2LimitLow(const float& limitsLow)
	{
		mLinkInfo.jointInfo.Swing2AxisInfo.limitsLow = limitsLow;
	}
	void LinkData::SetSwing2LimitHigh(const float& limitsHigh)
	{
		mLinkInfo.jointInfo.Swing2AxisInfo.limitsHigh = limitsHigh;
	}
	void LinkData::SetTwistAxisMotion(const fq::physics::EArticulationMotion& motion)
	{
		mLinkInfo.jointInfo.TwistAxisInfo.motion = motion;
	}
	void LinkData::SetTwistLimitLow(const float& limitsLow)
	{
		mLinkInfo.jointInfo.TwistAxisInfo.limitsLow = limitsLow;
	}
	void LinkData::SetTwistLimitHigh(const float& limitsHigh)
	{
		mLinkInfo.jointInfo.TwistAxisInfo.limitsHigh = limitsHigh;
	}
	void LinkData::SetIsDead(const bool& isDead)
	{
		mbIsDead = isDead;
	}
	void LinkData::SetID(const int& id)
	{
		mID = id;
	}

	std::shared_ptr<LinkData> LinkData::GetChildLinkData(LinkName name)
	{
		if (mChildrenLinkData.find(name) != mChildrenLinkData.end())
			return mChildrenLinkData.find(name)->second;

		return nullptr;
	}
	std::unordered_map<LinkName, std::shared_ptr<LinkData>>& LinkData::GetChildrenLinkData()
	{
		return mChildrenLinkData;
	}

	void LinkData::AddChildLinkData(LinkName name, std::shared_ptr<LinkData> linkData)
	{
		mChildrenLinkData.insert(std::make_pair(name, linkData));
	}
	void LinkData::RemoveChildLinkData(LinkName name)
	{
		if (mChildrenLinkData.find(name) != mChildrenLinkData.end())
			mChildrenLinkData.erase(mChildrenLinkData.find(name));
	}
}