#include "MeshEffectObject.h"
#include "RenderObject.h"
#include "AnimationHelper.h"

void fq::graphics::MeshEffectObject::SetMeshObjectInfo(const MeshObjectInfo& info)
{
	for (Node& node : mNodes)
	{
		assert(node.StaticMeshObject != nullptr);
		node.StaticMeshObject->SetMeshObjectInfo(info);
	}
}

void fq::graphics::MeshEffectObject::SetTimePos(float timePos)
{
	mTimePos = timePos;

	for (Node& node : mNodes)
	{
		assert(node.StaticMeshObject != nullptr);

		if (!node.Keyframes.empty())
		{
			size_t keyframeIndex = std::min<size_t>(mTimePos / mFramePerSecond, node.Keyframes.size() - 1);
			const auto transform = AnimationHelper::CreateMatrix(node.Keyframes[keyframeIndex]);
			node.StaticMeshObject->SetTransform(transform * mTransform);
		}

		if (!node.UVKeyframes.empty())
		{
			size_t uvIndex = std::min<size_t>(mTimePos / mUVFramePerSecond, node.UVKeyframes.size() - 1);
			const auto texTransform = AnimationHelper::CreateMatrix(node.UVKeyframes[uvIndex]);
			node.StaticMeshObject->SetTexTransform(texTransform);
		}
	}
}
