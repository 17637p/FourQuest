#pragma once

#include "IFQRenderObject.h"

namespace fq::graphics
{
	class StaticMeshObject;

	class MeshEffectObject : public IMeshEffectObject
	{
		friend class D3D11ObjectManager;

	public:
		struct Node
		{
			std::unique_ptr<StaticMeshObject> StaticMeshObject;
			std::vector<fq::common::Keyframe> Keyframes;
			std::vector<fq::common::UVKeyframe> UVKeyframes;
		};

	public:
		MeshEffectObject() = default;
		~MeshEffectObject() = default;

		virtual void SetTransform(const DirectX::SimpleMath::Matrix& transform) override { mTransform = transform; }
		virtual const DirectX::SimpleMath::Matrix& GetTransform() const override { return mTransform; }

		// Info
		virtual void SetMeshObjectInfo(const MeshObjectInfo& info) override;
		virtual const MeshObjectInfo& GetMeshObjectInfo() const override { return mInfo; }

		// TimePos
		virtual  void SetTimePos(float timePos) override;
		virtual float GetTimePos() const override { return mTimePos; }

		const std::vector<Node>& GetNodes() const { return mNodes; }

	private:
		void addNode(Node&& node)
		{
			mNodes.push_back(std::move(node));
		}
		void setAnimInfo(unsigned int frameCount, float framePerSecond, float duration)
		{
			mFrameCount = frameCount;
			mFramePerSecond = framePerSecond;
			mDuration = duration;
		}
		void setUVAnimInfo(unsigned int frameCount, float framePerSecond, float duration)
		{
			mUVFrameCount = frameCount;
			mUVFramePerSecond = framePerSecond;
			mUVDuration = duration;
		}

	public:
		DirectX::SimpleMath::Matrix mTransform;
		float mTimePos;
		MeshObjectInfo mInfo;
		std::vector<Node> mNodes;

		unsigned int mFrameCount;
		float mFramePerSecond;
		float mDuration;

		unsigned int mUVFrameCount;
		float mUVFramePerSecond;
		float mUVDuration;
	};
}