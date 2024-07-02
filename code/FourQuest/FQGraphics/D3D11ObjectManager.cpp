#include "D3D11ObjectManager.h"
#include "D3D11ModelManager.h"
#include "D3D11Device.h"
#include "RenderObject.h"
#include "ParticleObject.h"
#include "DecalObject.h"
#include "TrailObject.h"
#include "Mesh.h"
#include "Material.h"
#include "NodeHierarchy.h"

#include "../FQLoader/ModelLoader.h"
#include "../FQLoader/ModelConverter.h"

namespace fq::graphics
{
	std::shared_ptr<IStaticMeshObject> D3D11ObjectManager::CreateStaticMeshObject(std::shared_ptr<IStaticMesh> staticMesh, std::vector<std::shared_ptr<IMaterial>> materials, const MeshObjectInfo& meshObjectInfo, const DirectX::SimpleMath::Matrix& transform)
	{
		std::shared_ptr<IStaticMeshObject> staticMeshObject = std::make_shared<StaticMeshObject>(staticMesh, materials, meshObjectInfo, transform);
		mStaticMeshObjects.insert(staticMeshObject);

		return staticMeshObject;
	}
	std::shared_ptr<ISkinnedMeshObject> D3D11ObjectManager::CreateSkinnedMeshObject(std::shared_ptr<ISkinnedMesh> skinnedMesh, std::vector<std::shared_ptr<IMaterial>> materials, const MeshObjectInfo& meshObjectInfo, const DirectX::SimpleMath::Matrix& transform)
	{
		std::shared_ptr<ISkinnedMeshObject> skinnedMeshObject = std::make_shared<SkinnedMeshObject>(skinnedMesh, materials, meshObjectInfo, transform);
		mSkinnedMeshObjects.insert(skinnedMeshObject);

		return skinnedMeshObject;
	}
	std::shared_ptr<ITerrainMeshObject> D3D11ObjectManager::CreateTerrainMeshObject(const std::shared_ptr<D3D11Device>& device, std::shared_ptr<IStaticMesh> staticMesh, const DirectX::SimpleMath::Matrix& transform)
	{
		std::shared_ptr<ITerrainMeshObject> terrainMeshObject = std::make_shared<TerrainMeshObject>(device, std::static_pointer_cast<StaticMesh>(staticMesh), transform);
		mTerrainMeshObjects.insert(terrainMeshObject);

		return terrainMeshObject;
	}
	std::shared_ptr<IParticleObject> D3D11ObjectManager::CreateParticleObject(std::shared_ptr<D3D11Device> device, std::shared_ptr<IParticleMaterial> iParticleMaterial, const ParticleInfo& particleInfo, const DirectX::SimpleMath::Matrix& transform)
	{
		std::shared_ptr<IParticleObject> particleObject = std::make_shared<ParticleObject>(device, iParticleMaterial, particleInfo, transform);
		mParticleObjects.insert(particleObject);

		return particleObject;
	}
	std::shared_ptr<IDecalObject> D3D11ObjectManager::CreateDecalObject(std::shared_ptr<IDecalMaterial> iDecalMaterial, const DecalInfo& decalInfo, const DirectX::SimpleMath::Matrix& transform)
	{
		std::shared_ptr<IDecalObject> decalObject = std::make_shared<DecalObject>(iDecalMaterial, decalInfo, transform);
		mDecalObjects.insert(decalObject);

		return decalObject;
	}
	std::shared_ptr<ITrailObject> D3D11ObjectManager::CreateTrailObject(std::shared_ptr<IParticleMaterial> iParticleMaterial, const TrailInfo& trailInfo, const DirectX::SimpleMath::Matrix& transform)
	{
		std::shared_ptr<ITrailObject> trailObject = std::make_shared<TrailObject>(iParticleMaterial, trailInfo, transform);
		mTrailObjects.insert(trailObject);

		return trailObject;
	}

	void D3D11ObjectManager::DeleteStaticMeshObject(std::shared_ptr<IStaticMeshObject> staticMeshObject)
	{
		mStaticMeshObjects.erase(staticMeshObject);
	}
	void D3D11ObjectManager::DeleteSkinnedMeshObject(std::shared_ptr<ISkinnedMeshObject> skinnedMeshObject)
	{
		mSkinnedMeshObjects.erase(skinnedMeshObject);
	}
	void D3D11ObjectManager::DeleteTerrainMeshObject(std::shared_ptr<ITerrainMeshObject> terrainMeshObjectInterface)
	{
		mTerrainMeshObjects.erase(terrainMeshObjectInterface);
	}
	void D3D11ObjectManager::DeleteParticleObject(std::shared_ptr<IParticleObject> particleObject)
	{
		mParticleObjects.erase(particleObject);
	}
	void D3D11ObjectManager::DeleteDecalObject(std::shared_ptr<IDecalObject> decalObject)
	{
		mDecalObjects.erase(decalObject);
	}
	void D3D11ObjectManager::DeleteTrailObject(std::shared_ptr<ITrailObject> trailObject)
	{
		mTrailObjects.erase(trailObject);
	}

	void D3D11ObjectManager::SetTerrainMeshObject(const std::shared_ptr<D3D11Device>& device, std::shared_ptr<ITerrainMeshObject> iTerrainMeshObject, const TerrainMaterialInfo& material)
	{
		std::shared_ptr<TerrainMeshObject> terrainMeshObject = std::static_pointer_cast<TerrainMeshObject>(iTerrainMeshObject);
		terrainMeshObject->SetTerrainMaterial(device, material);
	}
}