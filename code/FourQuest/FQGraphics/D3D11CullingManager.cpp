#include "D3D11CullingManager.h"

#include "RenderObject.h"

std::set<std::shared_ptr<fq::graphics::IStaticMeshObject>> fq::graphics::D3D11CullingManager::GetInFrustumStaticObjects(const std::set<std::shared_ptr<IStaticMeshObject>>& staticObjects)
{
	std::set<std::shared_ptr<IStaticMeshObject>> inFrustumObjects;

	for (const auto& staticObject : staticObjects)
	{
		DirectX::BoundingSphere sphere = staticObject->GetStaticMesh()->GetMeshData().RenderBoundingSphere;
		sphere.Transform(sphere, staticObject->GetTransform());

		bool isIntersects = cameraFrustum.Intersects(sphere);
		if (isIntersects)
		{
			inFrustumObjects.insert(staticObject);
		}
	}

	return inFrustumObjects;
}

std::set<std::shared_ptr<fq::graphics::ISkinnedMeshObject>> fq::graphics::D3D11CullingManager::GetInFrustumSkinnedObjects(const std::set<std::shared_ptr<ISkinnedMeshObject>>& skinnedObjects)
{
	std::set<std::shared_ptr<ISkinnedMeshObject>> inFrustumObjects;

	for (const auto& skinnedObject : skinnedObjects)
	{
		DirectX::BoundingSphere sphere = skinnedObject->GetSkinnedMesh()->GetMeshData().RenderBoundingSphere;
		sphere.Transform(sphere, skinnedObject->GetTransform());

		bool isIntersects = cameraFrustum.Intersects(sphere);
		if (isIntersects)
		{
			inFrustumObjects.insert(skinnedObject);
		}
	}

	return inFrustumObjects;
}

void fq::graphics::D3D11CullingManager::UpdateCameraFrustum(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Quaternion& rotation, const DirectX::SimpleMath::Matrix& projectionMatrix)
{
	cameraFrustum = DirectX::BoundingFrustum(projectionMatrix, false);

	cameraFrustum.Origin = position;
	cameraFrustum.Orientation = rotation;
}

