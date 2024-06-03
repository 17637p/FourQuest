#pragma once

#include <unordered_map>
#include <memory>

#include <directxtk/SimpleMath.h>

namespace fq::common { struct Transform; }

namespace fq::graphics
{
	struct CameraInfo;
	class Camera;

	enum class ECameraType
	{
		Player,
		//Shadow,
	};

	class D3D11CameraManager
	{
	public:
		D3D11CameraManager();
		void Initialize(const unsigned short width, const unsigned short height);
		void OnResize(const unsigned short width, const unsigned short height);

		void Update(const ECameraType& cameraType, const fq::common::Transform& transform);

		void SetCamera(const ECameraType& cameraType, const CameraInfo& cameraInfo);
		void SetViewportSize(const ECameraType& cameraType, const unsigned short width, const unsigned short height);

		DirectX::SimpleMath::Matrix GetViewMatrix(const ECameraType& cameraType) const;
		DirectX::SimpleMath::Matrix GetProjectionMatrix(const ECameraType& cameraType) const;
		// Culling 할 때 필요한 것으로 추측
		DirectX::SimpleMath::Vector3 GetPosition(const ECameraType& cameraType) const;
		DirectX::SimpleMath::Quaternion GetRotation(const ECameraType& cameraType) const;
		// 쉐도우 투영행렬 만드는데 필요함
		float GetNearPlane(const ECameraType& cameraType) const;
		float GetFarPlane(const ECameraType& cameraType) const;

	private:
		std::unordered_map<ECameraType, std::shared_ptr<Camera>> mCameras;
	};
}

