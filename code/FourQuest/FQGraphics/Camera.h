#pragma once
#include <directxtk/SimpleMath.h>
#include <FQCommonGraphics.h>

namespace fq::common { struct Transform; }

namespace fq::graphics
{
	struct CameraInfo;

	class Camera
	{
	public:
		Camera();
		~Camera();

		void Initialize(const unsigned short width, const unsigned short height, const CameraInfo& cameraInfo);
		void Update(const fq::common::Transform& transform);

		void SetCamera(const CameraInfo& cameraInfo);
		void SetViewportSize(const unsigned short width, const unsigned short height);

		DirectX::SimpleMath::Matrix GetViewMatrix() const;
		DirectX::SimpleMath::Matrix GetProjectionMatrix() const;
		// Culling 할 때 필요한 것으로 추측
		DirectX::SimpleMath::Vector3 GetPosition() const;
		DirectX::SimpleMath::Quaternion GetRotation() const;

	private:
		void makeProjectionMatrix();

		unsigned short mWidth;
		unsigned short mHeight;

		bool mIsPerspective;
		float mFieldOfView;
		float mNearPlain;
		float mFarPlain;

		DirectX::SimpleMath::Matrix mViewMatrix;
		DirectX::SimpleMath::Matrix mProjectionMatrix;

		DirectX::SimpleMath::Vector3 mPosition;
		DirectX::SimpleMath::Quaternion mRotation;
	};
}
