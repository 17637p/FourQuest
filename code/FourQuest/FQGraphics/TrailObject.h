#pragma once

#include <deque>
#include <string>
#include "../FQCommon/IFQRenderObject.h"
#include "D3D11Common.h"

// 해야하는 것
// 1. 메테리얼 넣기
// 2. 

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
			DirectX::SimpleMath::Vector2 UV;
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

		const std::deque<Vertex>& GetVertices() const { return mVertices; }

	private:
		DirectX::SimpleMath::Matrix mTransform;
		TrailInfo mTrailInfo;
		std::shared_ptr<IParticleMaterial> mIParticleMaterial;
		std::deque<Vertex> mVertices;
		DirectX::SimpleMath::Vector3 mLastPosition;
	};
}

