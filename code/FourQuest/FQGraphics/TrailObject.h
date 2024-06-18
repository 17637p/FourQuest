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
		TrailObject(const TrailInfo& trailInfo, std::shared_ptr<D3D11ResourceManager> resourceManager);
		virtual ~TrailObject() = default;

		void Simulate(DirectX::SimpleMath::Vector3 cameraPos);

		void SetTrailInfo(const TrailInfo& trailInfo) override { mTrailInfo = trailInfo; }
		const TrailInfo& GetTrailInfo() const override { return mTrailInfo; }

		void SetTexturePath(const std::wstring& texturePath) override;

		const std::deque<Vertex>& GetVertices() const { return mVertices; }
		std::shared_ptr<D3D11Texture> GetTexture() const { return mTexture; }

	private:
		std::shared_ptr<D3D11ResourceManager> mResourceManager;
		TrailInfo mTrailInfo;

		// instance data
		std::deque<Vertex> mVertices;
		DirectX::SimpleMath::Vector3 mLastPosition;

		std::shared_ptr<D3D11Texture> mTexture;
	};
}

