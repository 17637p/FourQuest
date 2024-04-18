#pragma once
#include <directxtk/SimpleMath.h>
#include <FQCommonGraphics.h>

namespace fq { struct Transform; }

namespace fq_graphics
{
	struct CameraInfo;

	class Camera
	{
	public:
		Camera();
		~Camera();

		void Initialize(const unsigned short width, const unsigned short height, const CameraInfo& cameraInfo);
		void Update(const fq::Transform& transform);

		void SetCamera(const CameraInfo& cameraInfo);
		void SetViewportSize(const unsigned short width, const unsigned short height);

		DirectX::SimpleMath::Matrix GetViewMatrix() const;
		DirectX::SimpleMath::Matrix GetProjectionMatrix() const;
		// Culling �� �� �ʿ��� ������ ����
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

