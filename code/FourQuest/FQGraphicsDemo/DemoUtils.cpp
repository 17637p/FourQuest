#include "DemoUtils.h"

void fq::utils::Strafe(fq::common::Transform& cameraTransform, float distance)
{
	DirectX::SimpleMath::Matrix tempMatrix;
	tempMatrix = DirectX::SimpleMath::Matrix::CreateFromQuaternion(cameraTransform.worldRotation);

	cameraTransform.worldPosition = DirectX::SimpleMath::Vector3(
		tempMatrix._11 * distance + cameraTransform.worldPosition.x,
		tempMatrix._12 * distance + cameraTransform.worldPosition.y,
		tempMatrix._13 * distance + cameraTransform.worldPosition.z);

	cameraTransform.worldMatrix =
		DirectX::SimpleMath::Matrix::CreateScale(cameraTransform.worldScale) *
		DirectX::SimpleMath::Matrix::CreateFromQuaternion(cameraTransform.worldRotation) *
		DirectX::SimpleMath::Matrix::CreateTranslation(cameraTransform.worldPosition);
}

void fq::utils::Walk(fq::common::Transform& cameraTransform, float distance)
{
	DirectX::SimpleMath::Matrix tempMatrix;
	tempMatrix = DirectX::SimpleMath::Matrix::CreateFromQuaternion(cameraTransform.worldRotation);

	cameraTransform.worldPosition = DirectX::SimpleMath::Vector3(
		tempMatrix._31 * distance + cameraTransform.worldPosition.x,
		tempMatrix._32 * distance + cameraTransform.worldPosition.y,
		tempMatrix._33 * distance + cameraTransform.worldPosition.z);

	cameraTransform.worldMatrix =
		DirectX::SimpleMath::Matrix::CreateScale(cameraTransform.worldScale) *
		DirectX::SimpleMath::Matrix::CreateFromQuaternion(cameraTransform.worldRotation) *
		DirectX::SimpleMath::Matrix::CreateTranslation(cameraTransform.worldPosition);
}

void fq::utils::WorldUpdown(fq::common::Transform& cameraTransform, float distance)
{
	DirectX::SimpleMath::Matrix tempMatrix;
	tempMatrix = DirectX::SimpleMath::Matrix::CreateFromQuaternion(cameraTransform.worldRotation);

	cameraTransform.worldPosition = DirectX::SimpleMath::Vector3(
		tempMatrix._21 * distance + cameraTransform.worldPosition.x,
		tempMatrix._22 * distance + cameraTransform.worldPosition.y,
		tempMatrix._23 * distance + cameraTransform.worldPosition.z);

	cameraTransform.worldMatrix =
		DirectX::SimpleMath::Matrix::CreateScale(cameraTransform.worldScale) *
		DirectX::SimpleMath::Matrix::CreateFromQuaternion(cameraTransform.worldRotation) *
		DirectX::SimpleMath::Matrix::CreateTranslation(cameraTransform.worldPosition);
}

void fq::utils::Yaw(fq::common::Transform& cameraTransform, float angle)
{
	DirectX::SimpleMath::Vector3 up{ 0, 1, 0 };
	//up.Normalize();
	up = up * angle;
	DirectX::SimpleMath::Quaternion quaternion = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(up.y, up.x, up.z);

	cameraTransform.worldRotation = cameraTransform.worldRotation * quaternion;

	cameraTransform.worldMatrix =
		DirectX::SimpleMath::Matrix::CreateScale(cameraTransform.worldScale) *
		DirectX::SimpleMath::Matrix::CreateFromQuaternion(cameraTransform.worldRotation) *
		DirectX::SimpleMath::Matrix::CreateTranslation(cameraTransform.worldPosition);
}

void fq::utils::Pitch(fq::common::Transform& cameraTransform, float angle)
{
	DirectX::SimpleMath::Vector3 right{ 1, 0, 0 };
	right = right * angle;
	DirectX::SimpleMath::Quaternion quaternion = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(right.y, right.x, right.z);

	cameraTransform.worldRotation = quaternion * cameraTransform.worldRotation;

	cameraTransform.worldMatrix =
		DirectX::SimpleMath::Matrix::CreateScale(cameraTransform.worldScale) *
		DirectX::SimpleMath::Matrix::CreateFromQuaternion(cameraTransform.worldRotation) *
		DirectX::SimpleMath::Matrix::CreateTranslation(cameraTransform.worldPosition);
}
