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
		TrailObject(const DirectX::SimpleMath::Matrix& transform, const TrailInfo& trailInfo, std::shared_ptr<IParticleMaterial> iParticleMaterial);
		virtual ~TrailObject() = default;

		void Simulate(DirectX::SimpleMath::Vector3 cameraPos);

		// Info
		void SetTrailInfo(const TrailInfo& trailInfo) override { mTrailInfo = trailInfo; }
		const TrailInfo& GetTrailInfo() const override { return mTrailInfo; }

		// Transform
		void SetTransform(const DirectX::SimpleMath::Matrix& transform) override { mTransform = transform; }
		const DirectX::SimpleMath::Matrix& GetTransform() const override { return mTransform; }

		// Material
		void SetIParticleMaterial(std::shared_ptr<IParticleMaterial> iParticleMaterial) override { mIParticleMaterial = iParticleMaterial; }
		std::shared_ptr<IParticleMaterial> GetIParticleMaterial() const override { return mIParticleMaterial; }

		// FrameTime
		void SetFrameTime(float frameTime) override { mFrameTime = frameTime; }
		float GetFrameTIme() const override {return mFrameTime;		}

		const std::deque<Vertex>& GetVertices() const { return mVertices; }

	private:
		DirectX::SimpleMath::Vector3 catmullRom(const DirectX::SimpleMath::Vector3& P0, const DirectX::SimpleMath::Vector3& P1, const DirectX::SimpleMath::Vector3& P2, const DirectX::SimpleMath::Vector3& P3, float t);
	
	private:
		DirectX::SimpleMath::Matrix mTransform;
		float mFrameTime;
		TrailInfo mTrailInfo;
		std::shared_ptr<IParticleMaterial> mIParticleMaterial;
		std::deque<Vertex> mVertices;
		DirectX::SimpleMath::Vector3 mLastPosition;

		std::deque<Vertex> mCatmulRomTopVertics;
		std::deque<Vertex> mCatmulRomBottomVertics;
	};
}

