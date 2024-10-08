#include "LookingObjectHelper.h"
#include "../FQGameModule/Transform.h"

namespace fq::client
{
	void LookingObjectHelper::OnStart()
	{
		// 오브젝트 찾기
		mLookingObjectOrNull = GetScene()->GetObjectByName(mLookingObjectName);
	}

	void LookingObjectHelper::OnUpdate(float dt)
	{
		if (mLookingObjectOrNull == nullptr)
		{
			return;
		}

		auto* transformComp = GetTransform();
		const auto* lookingTransformComp = mLookingObjectOrNull->GetTransform();

		DirectX::SimpleMath::Vector3 virtualUpVector = { 0, 1, 0 };

		auto lookVector = lookingTransformComp->GetWorldPosition() - transformComp->GetWorldPosition();
		lookVector.Normalize();

		auto rightVector = virtualUpVector.Cross(lookVector);
		rightVector.Normalize();

		auto upVector = lookVector.Cross(rightVector);
		upVector.Normalize();

		DirectX::SimpleMath::Matrix rotationMatrix
		{
			{ rightVector.x, rightVector.y, rightVector.z, 0},
			{ upVector.x, upVector.y, upVector.z, 0},
			{ lookVector.x, lookVector.y, lookVector.z, 0},
			{ 0, 0, 0,1 },
		};

		transformComp->SetLocalRotationToMatrix(rotationMatrix);
	}
}