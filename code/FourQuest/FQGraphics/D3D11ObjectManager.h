#pragma once

#include "CommonHeader.h"

#include "../FQCommon/FQCommonLoader.h"
#include "../FQCommon/FQCommonGraphics.h"
#include "../FQCommon/IFQRenderObject.h"

namespace fq::graphics
{
	class D3D11Device;

	class D3D11ModelManager;
	class IStaticMeshObject;
	class ISkinnedMeshObject;
	class ITerrainMeshObject;
	class IParticleObject;
	class IDecalObject;
	class ITrailObject;
	class IStaticMesh;
	class ISkinnedMesh;
	class IMaterial;
	class IParticleMaterial;
	class IDecalMaterial;

	class D3D11ObjectManager
	{
	public:
		D3D11ObjectManager() = default;
		~D3D11ObjectManager() = default;

		std::shared_ptr<IStaticMeshObject> CreateStaticMeshObject(std::shared_ptr<IStaticMesh> staticMesh, std::vector<std::shared_ptr<IMaterial>> materials, const MeshObjectInfo& meshObjectInfo, const DirectX::SimpleMath::Matrix& transform);
		std::shared_ptr<ISkinnedMeshObject> CreateSkinnedMeshObject(std::shared_ptr<ISkinnedMesh> skinnedMesh, std::vector<std::shared_ptr<IMaterial>> materials, const MeshObjectInfo& meshObjectInfo, const DirectX::SimpleMath::Matrix& transform);
		std::shared_ptr<ITerrainMeshObject> CreateTerrainMeshObject(const std::shared_ptr<D3D11Device>& device, std::shared_ptr<IStaticMesh> staticMesh, const DirectX::SimpleMath::Matrix& transform);
		std::shared_ptr<IParticleObject> CreateParticleObject(std::shared_ptr<D3D11Device> device, std::shared_ptr<IParticleMaterial> iParticleMaterial, const ParticleInfo& particleInfo, const DirectX::SimpleMath::Matrix& transform);
		std::shared_ptr<IDecalObject> CreateDecalObject(std::shared_ptr<IDecalMaterial> iDecalMaterial, const DecalInfo& decalInfo, const DirectX::SimpleMath::Matrix& transform);
		std::shared_ptr<ITrailObject> CreateTrailObject(std::shared_ptr<IParticleMaterial> iParticleMaterial, const TrailInfo& trailInfo, const DirectX::SimpleMath::Matrix& transform);

		void DeleteStaticMeshObject(std::shared_ptr<IStaticMeshObject> staticMeshObject);
		void DeleteSkinnedMeshObject(std::shared_ptr<ISkinnedMeshObject> skinnedMeshObject);
		void DeleteTerrainMeshObject(std::shared_ptr<ITerrainMeshObject> terrainMeshObjectInterface);
		void DeleteParticleObject(std::shared_ptr<IParticleObject> particleObject);
		void DeleteDecalObject(std::shared_ptr<IDecalObject> decalObject);
		void DeleteTrailObject(std::shared_ptr<ITrailObject> trailObject);

		void SetTerrainMeshObject(const std::shared_ptr<D3D11Device>& device, std::shared_ptr<ITerrainMeshObject> iTerrainMeshObject, const TerrainMaterialInfo& material);

		const std::set<std::shared_ptr<IStaticMeshObject>>& GetStaticMeshObjects() const { return mStaticMeshObjects; }
		const std::set<std::shared_ptr<ISkinnedMeshObject>>& GetSkinnedMeshObjects() const { return mSkinnedMeshObjects; }
		const std::set<std::shared_ptr<ITerrainMeshObject>>& GetTerrainMeshObjects() const { return mTerrainMeshObjects; }
		const std::set<std::shared_ptr<IParticleObject>>& GetParticleObjects() const { return mParticleObjects; }
		const std::set<std::shared_ptr<IDecalObject>>& GetDecalObjects() const { return mDecalObjects; }
		const std::set<std::shared_ptr<ITrailObject>>& GetTrailObjects() const { return mTrailObjects; }

	private:
		std::set<std::shared_ptr<IStaticMeshObject>> mStaticMeshObjects;
		std::set<std::shared_ptr<ISkinnedMeshObject>> mSkinnedMeshObjects;
		std::set<std::shared_ptr<ITerrainMeshObject>> mTerrainMeshObjects;
		std::set<std::shared_ptr<IParticleObject>> mParticleObjects;
		std::set<std::shared_ptr<IDecalObject>> mDecalObjects;
		std::set<std::shared_ptr<ITrailObject>> mTrailObjects;
	};
}
