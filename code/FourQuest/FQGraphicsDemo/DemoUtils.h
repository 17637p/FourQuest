#pragma once
#include <FQCommon.h>

namespace fq::graphics { class IFQGraphics; }

namespace fq::utils
{
	/*=============================================================================
								Camera Moving
	=============================================================================*/
#pragma region camera
	void Strafe(fq::common::Transform& cameraTransform, float distance);
	void Walk(fq::common::Transform& cameraTransform, float distance);
	void WorldUpdown(fq::common::Transform& cameraTransform, float distance);

	void Yaw(fq::common::Transform& cameraTransform, float angle);
	void Pitch(fq::common::Transform& cameraTransform, float angle);
#pragma endregion camera

	void AddDefaultCamera(fq::graphics::IFQGraphics* graphcis);
	void AddDefaultLight();
}

