#pragma once

#include <queue>
#include <string>
#include <array>
#include "../FQCommon/IFQRenderObject.h"
#include "D3D11Common.h"

namespace fq::graphics
{
	class D3D11ResourceManager;
	class D3D11Texture;
	class IParticleMaterial;

	class TrailObject : public ITrailObject
	{
	public:
		struct Vertex
		{
			DirectX::SimpleMath::Vector3 Position;
			DirectX::SimpleMath::Vector4 Color;
			DirectX::SimpleMath::Vector2 UV;
			DirectX::SimpleMath::Vector3 ExtendAxis;
			DirectX::SimpleMath::Vector3 Origin;
			float Age;
		};

	public:
		TrailObject(std::shared_ptr<IParticleMaterial> iParticleMaterial, const TrailInfo& trailInfo, const DirectX::SimpleMath::Matrix& transform);
		virtual ~TrailObject() = default;

		void Simulate(DirectX::SimpleMath::Vector3 cameraPos);

		// Transform
		virtual void SetTransform(const DirectX::SimpleMath::Matrix& transform) override { mTransform = transform; }
		virtual const DirectX::SimpleMath::Matrix& GetTransform() const override { return mTransform; }

		// Info
		void SetTrailInfo(const TrailInfo& trailInfo) override { mTrailInfo = trailInfo; }
		const TrailInfo& GetTrailInfo() const override { return mTrailInfo; }
		 TrailInfo& GetTrailInfo()  override { return mTrailInfo; }

		// Material
		void SetIParticleMaterial(std::shared_ptr<IParticleMaterial> iParticleMaterial) override { mIParticleMaterial = iParticleMaterial; }
		std::shared_ptr<IParticleMaterial> GetIParticleMaterial() const override { return mIParticleMaterial; }

		// FrameTime
		void SetFrameTime(float frameTime) override { mFrameTime = frameTime; }
		float GetFrameTIme() const override { return mFrameTime; }

		const std::deque<Vertex>& GetVertices() const { return mVertices; }

	private:
		DirectX::SimpleMath::Vector3 catmullRom(const DirectX::SimpleMath::Vector3& P0, const DirectX::SimpleMath::Vector3& P1, const DirectX::SimpleMath::Vector3& P2, const DirectX::SimpleMath::Vector3& P3, float t);

	private:
		std::shared_ptr<IParticleMaterial> mIParticleMaterial;
		TrailInfo mTrailInfo;
		DirectX::SimpleMath::Matrix mTransform;
		float mFrameTime;
		DirectX::SimpleMath::Vector3 mLastPosition;
		std::deque<Vertex> mVertices;
		std::deque<Vertex> mCatmulRomTopVertics;
		std::deque<Vertex> mCatmulRomBottomVertics;
	};
}

