#pragma once
#include "ResourceBase.h"

#include <physx/PxPhysicsAPI.h>
#include <unordered_map>
#include <string>
#include <memory>
#include <vector>

namespace fq::physics
{
	struct JointAxisResourceData
	{
		physx::PxArticulationMotion motion;				// 모션 제한
		float limitsLow;								// 범위 ( Limit일 때 회전 아랫 범위 : 0.0 ~ 1.0 ) 
		float limitsHigh;								// 범위 ( Limit일 때 회전 윗 범위 : 0.0 ~ 1.0 )
	};

	struct JointResourceData
	{
		std::string parentLinkName;
		std::string ownerLinkName;
		JointAxisResourceData jointSwing1AxisInfo;		// Swing1( X축을 중심으로 한 회전 )
		JointAxisResourceData jointSwing2AxisInfo;		// Swing2( Y축을 중심으로 한 회전 )
		JointAxisResourceData jointTwistAxisInfo;		// Twist( Z축을 중심으로 한 회전 )
		physx::PxTransform localTransform;				// 조인트의 로절 좌표
		float stiffness;								// 강성 : 관절이 목표 위치로 이동하려는 힘의 크기
		float damping;									// 감쇠 계수 : 운동에 대한 저항력 ( 진동을 방지하고 부드럽게 움직이동 할 수 있게 )
		float maxForce;									// 최대 힘 : 관절 드라이브가 적용할 수 있는 최대 힘
	};

	struct LinkResourceData
	{
		std::string boneName;							// 해당 본(링크)의 이름
		std::string parentBoneName;						// 부모 본(링크)의 이름
		physx::PxGeometry* geometry;					// 지오메트리
		float density;									// 밀도
	};

	class ArticulationResource : public ResourceBase
	{
	public:
		ArticulationResource();
		virtual ~ArticulationResource();

	private:
		std::vector<LinkResourceData> mLinkData;
		std::vector<JointResourceData> mJointData;
	};
}
