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
		physx::PxArticulationMotion motion;				// ��� ����
		float limitsLow;								// ���� ( Limit�� �� ȸ�� �Ʒ� ���� : 0.0 ~ 1.0 ) 
		float limitsHigh;								// ���� ( Limit�� �� ȸ�� �� ���� : 0.0 ~ 1.0 )
	};

	struct JointResourceData
	{
		std::string parentLinkName;
		std::string ownerLinkName;
		JointAxisResourceData jointSwing1AxisInfo;		// Swing1( X���� �߽����� �� ȸ�� )
		JointAxisResourceData jointSwing2AxisInfo;		// Swing2( Y���� �߽����� �� ȸ�� )
		JointAxisResourceData jointTwistAxisInfo;		// Twist( Z���� �߽����� �� ȸ�� )
		physx::PxTransform localTransform;				// ����Ʈ�� ���� ��ǥ
		float stiffness;								// ���� : ������ ��ǥ ��ġ�� �̵��Ϸ��� ���� ũ��
		float damping;									// ���� ��� : ��� ���� ���׷� ( ������ �����ϰ� �ε巴�� �����̵� �� �� �ְ� )
		float maxForce;									// �ִ� �� : ���� ����̺갡 ������ �� �ִ� �ִ� ��
	};

	struct LinkResourceData
	{
		std::string boneName;							// �ش� ��(��ũ)�� �̸�
		std::string parentBoneName;						// �θ� ��(��ũ)�� �̸�
		physx::PxGeometry* geometry;					// ������Ʈ��
		float density;									// �е�
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
