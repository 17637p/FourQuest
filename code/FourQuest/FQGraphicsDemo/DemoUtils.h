#pragma once
#include <FQCommon.h>

namespace fq::utils
{
	/*=============================================================================
								camera
	=============================================================================*/
#pragma region camera
	void Strafe(fq::common::Transform& cameraTransform, float distance);
	void Walk(fq::common::Transform& cameraTransform, float distance);
	void WorldUpdown(fq::common::Transform& cameraTransform, float distance);

	void Yaw(fq::common::Transform& cameraTransform, float angle);
	void Pitch(fq::common::Transform& cameraTransform, float angle);
#pragma endregion camera
}

