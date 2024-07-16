#pragma once

#include "CommonHeader.h"
#include "FQCommonGraphics.h"

#include <DirectXColors.h>
#include <DirectXCollision.h>
#include <directxtk/CommonStates.h>
#include <directxtk/PrimitiveBatch.h>
#include <directxtk/Effects.h>
#include <directXTK/VertexTypes.h>
#include <wrl/client.h>

namespace fq::graphics
{
	class D3D11CameraManager;
	class D3D11Device;

	class D3D11DebugDrawManager
	{
	public:
		D3D11DebugDrawManager() = default;
		~D3D11DebugDrawManager() = default;

		void Initialize(const std::shared_ptr<D3D11Device>& device);
		void Finalize();

		void Excute(const std::shared_ptr<D3D11Device>& device,
			const std::shared_ptr<D3D11CameraManager>& cameraManager);

		void Submit(const debug::SphereInfo& info);
		void Submit(const debug::AABBInfo& info);
		void Submit(const debug::OBBInfo& info);
		void Submit(const debug::FrustumInfo& info);
		void Submit(const debug::GridInfo& info);
		void Submit(const debug::RingInfo& info);
		void Submit(const debug::RayInfo& info);
		void Submit(const debug::PolygonInfo& info);
		void Submit(const debug::HemisphereInfo& info);
		void Submit(const debug::ConeInfo& info);
		void Submit(const debug::DountInfo& info);
		void Submit(const debug::SphereInfoEx& info);
		void Submit(const debug::RingInfoEx& info);

	private:
		void Draw(const std::shared_ptr<D3D11Device>& device, const debug::SphereInfo& info);
		void Draw(const std::shared_ptr<D3D11Device>& device, const debug::AABBInfo& info);
		void Draw(const std::shared_ptr<D3D11Device>& device, const debug::OBBInfo& info);
		void Draw(const std::shared_ptr<D3D11Device>& device, const debug::FrustumInfo& info);
		void DrawGrid(const std::shared_ptr<D3D11Device>& device, const debug::GridInfo& info);
		void DrawRing(const std::shared_ptr<D3D11Device>& device, const debug::RingInfo& info);
		void DrawRay(const std::shared_ptr<D3D11Device>& device, const debug::RayInfo& info);
		void DrawLineStrip(const std::shared_ptr<D3D11Device>& device, const debug::PolygonInfo& info);
		void Draw(const std::shared_ptr<D3D11Device>& device, const debug::HemisphereInfo& info);
		void Draw(const std::shared_ptr<D3D11Device>& device, const debug::ConeInfo& info);
		void Draw(const std::shared_ptr<D3D11Device>& device, const debug::DountInfo& info);
		void Draw(const std::shared_ptr<D3D11Device>& device, const debug::SphereInfoEx& info);
		void Draw(const std::shared_ptr<D3D11Device>& device, const debug::RingInfoEx& info);

	private:
		void drawCube(const std::shared_ptr<D3D11Device>& device,
			const DirectX::SimpleMath::Matrix& worldTransform,
			const DirectX::SimpleMath::Color& color);

	private:
		std::unique_ptr<DirectX::DX11::CommonStates> mStates;
		std::unique_ptr<DirectX::DX11::BasicEffect> mBatchEffect;
		std::unique_ptr<DirectX::PrimitiveBatch<DirectX::DX11::VertexPositionColor>> mBatch;
		Microsoft::WRL::ComPtr<ID3D11InputLayout> mBatchInputLayout;

		std::queue<debug::SphereInfo> mSphereInfos;
		std::queue<debug::AABBInfo> mAABBInfos;
		std::queue<debug::OBBInfo> mOBBInfos;
		std::queue<debug::FrustumInfo> mFrustumInfos;
		std::queue<debug::GridInfo> mGridInfos;
		std::queue<debug::RingInfo> mRingInfos;
		std::queue<debug::RayInfo> mRayInfos;
		std::queue<debug::PolygonInfo> mPolygonInfos;
		std::queue<debug::HemisphereInfo> mHemisphereInfos;
		std::queue<debug::ConeInfo> mConeInfos;
		std::queue<debug::DountInfo> mDountInfos;
		std::queue<debug::SphereInfoEx> mSphereInfoExInfos;
		std::queue<debug::RingInfoEx> mRingInfoExInfos;

		std::queue<debug::SphereInfo> mDepthOffSphereInfos;
		std::queue<debug::AABBInfo> mDepthOffAABBInfos;
		std::queue<debug::OBBInfo> mDepthOffOBBInfos;
		std::queue<debug::FrustumInfo> mDepthOffFrustumInfos;
		std::queue<debug::GridInfo> mDepthOffGridInfos;
		std::queue<debug::RingInfo> mDepthOffRingInfos;
		std::queue<debug::RayInfo> mDepthOffRayInfos;
		std::queue<debug::PolygonInfo> mDepthOffPolygonInfos;
		std::queue<debug::HemisphereInfo> mDepthOffHemisphereInfos;
		std::queue<debug::ConeInfo> mDepthOffConeInfos;
		std::queue<debug::DountInfo> mDepthOffDountInfos;
		std::queue<debug::SphereInfoEx> mDepthOffSphereInfoExInfos;
		std::queue<debug::RingInfoEx> mDepthOffRingInfoExInfos;
	};
}