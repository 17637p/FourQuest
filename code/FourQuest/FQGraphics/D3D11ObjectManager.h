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
		~D3D11ObjectManager();

		void ClearDeleteQueue();

		void UpdateModifiedResources(std::shared_ptr<D3D11Device> device);

		IStaticMeshObject* CreateStaticMeshObject(std::shared_ptr<IStaticMesh> staticMesh, std::vector<std::shared_ptr<IMaterial>> materials, const MeshObjectInfo& meshObjectInfo, const DirectX::SimpleMath::Matrix& transform);
		ISkinnedMeshObject* CreateSkinnedMeshObject(std::shared_ptr<ISkinnedMesh> skinnedMesh, std::vector<std::shared_ptr<IMaterial>> materials, const MeshObjectInfo& meshObjectInfo, const DirectX::SimpleMath::Matrix& transform);
		ITerrainMeshObject* CreateTerrainMeshObject(const std::shared_ptr<D3D11Device>& device, std::shared_ptr<IStaticMesh> staticMesh, const DirectX::SimpleMath::Matrix& transform);
		IParticleObject* CreateParticleObject(std::shared_ptr<D3D11Device> device, std::shared_ptr<IParticleMaterial> iParticleMaterial, const ParticleInfo& particleInfo, const DirectX::SimpleMath::Matrix& transform);
		IDecalObject* CreateDecalObject(std::shared_ptr<IDecalMaterial> iDecalMaterial, const DecalInfo& decalInfo, const DirectX::SimpleMath::Matrix& transform);
		ITrailObject* CreateTrailObject(std::shared_ptr<IParticleMaterial> iParticleMaterial, const TrailInfo& trailInfo, const DirectX::SimpleMath::Matrix& transform);
		IProbeObject* CreateProbeObject(std::shared_ptr<IStaticMesh> staticMesh, const DirectX::SimpleMath::Matrix& transform, int index);

		void DeleteStaticMeshObject(IStaticMeshObject* staticMeshObject);
		void DeleteSkinnedMeshObject(ISkinnedMeshObject* skinnedMeshObject);
		void DeleteTerrainMeshObject(ITerrainMeshObject* terrainMeshObjectInterface);
		void DeleteParticleObject(IParticleObject* particleObject);
		void DeleteDecalObject(IDecalObject* decalObject);
		void DeleteTrailObject(ITrailObject* trailObject);
		void DeleteProbeObject(IProbeObject* probeObject);

		void SetTerrainMeshObject(const std::shared_ptr<D3D11Device>& device, ITerrainMeshObject* iTerrainMeshObject, const TerrainMaterialInfo& material);

		const std::set<IStaticMeshObject*>& GetStaticMeshObjects() const { return mStaticMeshObjects; }
		const std::set<ISkinnedMeshObject*>& GetSkinnedMeshObjects() const { return mSkinnedMeshObjects; }
		const std::set<ITerrainMeshObject*>& GetTerrainMeshObjects() const { return mTerrainMeshObjects; }
		const std::set<IParticleObject*>& GetParticleObjects() const { return mParticleObjects; }
		const std::set<IDecalObject*>& GetDecalObjects() const { return mDecalObjects; }
		const std::set<ITrailObject*>& GetTrailObjects() const { return mTrailObjects; }
		const std::set<IProbeObject*>& GetProbeObjects() const { return mProbeObjects; }

	private:
		template <typename Child, typename Parent>
		void deleteObject(Parent* parentPtr)
		{
			Child* childPtr = static_cast<Child*>(parentPtr);
			delete childPtr;
		}

	private:
		std::set<IStaticMeshObject*> mStaticMeshObjects;
		std::set<ISkinnedMeshObject*> mSkinnedMeshObjects;
		std::set<ITerrainMeshObject*> mTerrainMeshObjects;
		std::set<IParticleObject*> mParticleObjects;
		std::set<IDecalObject*> mDecalObjects;
		std::set<ITrailObject*> mTrailObjects;
		std::set<IProbeObject*> mProbeObjects;

		std::queue<IStaticMeshObject*> mStaticMeshObjectDeleteQueue;
		std::queue<ISkinnedMeshObject*> mSkinnedMeshObjectDeleteQueue;
		std::queue<ITerrainMeshObject*> mTerrainMeshObjectDeleteQueue;
		std::queue<IParticleObject*> mParticleObjectDeleteQueue;
		std::queue<IDecalObject*> mDecalObjectDeleteQueue;
		std::queue<ITrailObject*> mTrailObjectDeleteQueue;
		std::queue<IProbeObject*> mProbeObjectDeleteQueue;
	};
}
