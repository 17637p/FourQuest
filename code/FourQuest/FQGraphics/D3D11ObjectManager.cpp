#include "D3D11ObjectManager.h"
#include "D3D11ModelManager.h"
#include "D3D11Device.h"
#include "RenderObject.h"
#include "ParticleObject.h"
#include "DecalObject.h"
#include "TrailObject.h"
#include "MeshEffectObject.h"
#include "Mesh.h"
#include "Material.h"
#include "NodeHierarchy.h"

#include "../FQLoader/ModelLoader.h"
#include "../FQLoader/ModelConverter.h"

namespace fq::graphics
{
	D3D11ObjectManager::~D3D11ObjectManager()
	{
		for (auto* element : mStaticMeshObjects) { deleteObject<StaticMeshObject>(element); } mStaticMeshObjects.clear();
		for (auto* element : mSkinnedMeshObjects) { deleteObject<SkinnedMeshObject>(element); } mSkinnedMeshObjects.clear();
		for (auto* element : mTerrainMeshObjects) { deleteObject<TerrainMeshObject>(element); } mTerrainMeshObjects.clear();
		for (auto* element : mParticleObjects) { deleteObject<ParticleObject>(element); } mParticleObjects.clear();
		for (auto* element : mDecalObjects) { deleteObject<DecalObject>(element); } mDecalObjects.clear();
		for (auto* element : mTrailObjects) { deleteObject<TrailObject>(element); } mTrailObjects.clear();
		for (auto* element : mMeshEffectObjects) { deleteObject<MeshEffectObject>(element); } mMeshEffectObjects.clear();
		for (auto* element : mProbeObjects) { deleteObject<ProbeObject>(element); } mProbeObjects.clear();
		ClearDeleteQueue();
	}

	void D3D11ObjectManager::ClearDeleteQueue()
	{
		while (!mStaticMeshObjectDeleteQueue.empty()) { deleteObject<StaticMeshObject>(mStaticMeshObjectDeleteQueue.front()); mStaticMeshObjectDeleteQueue.pop(); }
		while (!mSkinnedMeshObjectDeleteQueue.empty()) { deleteObject<SkinnedMeshObject>(mSkinnedMeshObjectDeleteQueue.front()); mSkinnedMeshObjectDeleteQueue.pop(); }
		while (!mTerrainMeshObjectDeleteQueue.empty()) { deleteObject<TerrainMeshObject>(mTerrainMeshObjectDeleteQueue.front()); mTerrainMeshObjectDeleteQueue.pop(); }
		while (!mParticleObjectDeleteQueue.empty()) { deleteObject<ParticleObject>(mParticleObjectDeleteQueue.front()); mParticleObjectDeleteQueue.pop(); }
		while (!mDecalObjectDeleteQueue.empty()) { deleteObject<DecalObject>(mDecalObjectDeleteQueue.front()); mDecalObjectDeleteQueue.pop(); }
		while (!mTrailObjectDeleteQueue.empty()) { deleteObject<TrailObject>(mTrailObjectDeleteQueue.front()); mTrailObjectDeleteQueue.pop(); }
		while (!mMeshEffectObjectDeleteQueue.empty()) { deleteObject<MeshEffectObject>(mMeshEffectObjectDeleteQueue.front()); mMeshEffectObjectDeleteQueue.pop(); }
		while (!mProbeObjectDeleteQueue.empty()) { deleteObject<ProbeObject>(mProbeObjectDeleteQueue.front()); mProbeObjectDeleteQueue.pop(); }
	}

	IStaticMeshObject* D3D11ObjectManager::CreateStaticMeshObject(std::shared_ptr<IStaticMesh> staticMesh, std::vector<std::shared_ptr<IMaterial>> materials, const MeshObjectInfo& meshObjectInfo, const DirectX::SimpleMath::Matrix& transform)
	{
		IStaticMeshObject* staticMeshObject = new StaticMeshObject(staticMesh, materials, meshObjectInfo, transform);
		mStaticMeshObjects.insert(staticMeshObject);

		return staticMeshObject;
	}
	ISkinnedMeshObject* D3D11ObjectManager::CreateSkinnedMeshObject(std::shared_ptr<ISkinnedMesh> skinnedMesh, std::vector<std::shared_ptr<IMaterial>> materials, const MeshObjectInfo& meshObjectInfo, const DirectX::SimpleMath::Matrix& transform)
	{
		ISkinnedMeshObject* skinnedMeshObject = new SkinnedMeshObject(skinnedMesh, materials, meshObjectInfo, transform);
		mSkinnedMeshObjects.insert(skinnedMeshObject);

		return skinnedMeshObject;
	}
	ITerrainMeshObject* D3D11ObjectManager::CreateTerrainMeshObject(const std::shared_ptr<D3D11Device>& device, std::shared_ptr<IStaticMesh> staticMesh, const DirectX::SimpleMath::Matrix& transform)
	{
		ITerrainMeshObject* terrainMeshObject = new TerrainMeshObject(device, std::static_pointer_cast<StaticMesh>(staticMesh), transform);
		mTerrainMeshObjects.insert(terrainMeshObject);

		return terrainMeshObject;
	}
	IParticleObject* D3D11ObjectManager::CreateParticleObject(std::shared_ptr<D3D11Device> device, std::shared_ptr<IParticleMaterial> iParticleMaterial, const ParticleInfo& particleInfo, const DirectX::SimpleMath::Matrix& transform)
	{
		IParticleObject* particleObject = new ParticleObject(device, iParticleMaterial, particleInfo, transform);
		mParticleObjects.insert(particleObject);

		return particleObject;
	}
	IDecalObject* D3D11ObjectManager::CreateDecalObject(std::shared_ptr<IDecalMaterial> iDecalMaterial, const DecalInfo& decalInfo, const DirectX::SimpleMath::Matrix& transform)
	{
		IDecalObject* decalObject = new DecalObject(iDecalMaterial, decalInfo, transform);
		mDecalObjects.insert(decalObject);

		return decalObject;
	}
	ITrailObject* D3D11ObjectManager::CreateTrailObject(std::shared_ptr<IParticleMaterial> iParticleMaterial, const TrailInfo& trailInfo, const DirectX::SimpleMath::Matrix& transform)
	{
		ITrailObject* trailObject = new TrailObject(iParticleMaterial, trailInfo, transform);
		mTrailObjects.insert(trailObject);

		return trailObject;
	}

	IMeshEffectObject* D3D11ObjectManager::CreateMeshEffectObject(std::shared_ptr<D3D11Device> device, std::shared_ptr<D3D11ModelManager> modelManager, const std::string& modelPath, const std::string& uvAnimationPath, const std::string& transformAnimationPath, const std::string& texturebasePath, const DirectX::SimpleMath::Matrix& transform)
	{
		// 로딩만해서 전달하자 
		MeshEffectObject* meshEffectObject = new MeshEffectObject();
		meshEffectObject->SetTransform(transform);

		const auto& modelData = modelManager->CreateModelResource(device, modelPath, texturebasePath);
		const auto transformAnimationData = fq::loader::AnimationLoader::Read(transformAnimationPath);
		const auto uvAnimationData = fq::loader::UVAnimationLoader::Read(uvAnimationPath);

		meshEffectObject->setAnimInfo(transformAnimationData.FrameCount, transformAnimationData.FramePerSecond, transformAnimationData.Duration);
		meshEffectObject->setUVAnimInfo(uvAnimationData.FrameCount, uvAnimationData.FramePerSecond, uvAnimationData.Duration);

		for (const auto& [node, mesh] : modelData.Meshes)
		{
			auto meshResource = modelManager->GetStaticMeshByModelPathOrNull(modelPath, mesh.Name);

			if (meshResource == nullptr)
			{
				continue;
			}

			std::vector<std::shared_ptr<IMaterial>> materialInterfaces;
			materialInterfaces.reserve(mesh.Subsets.size());

			for (const auto& subset : mesh.Subsets)
			{
				auto materialResource = modelManager->GetMaterialByModelPathOrNull(modelPath, subset.MaterialName);

				if (materialResource != nullptr)
				{
					materialInterfaces.push_back(materialResource);
				}
			}

			MeshEffectObject::Node meshEffectNode;
			MeshObjectInfo defaultInfo;
			meshEffectNode.StaticMeshObject = std::make_unique<StaticMeshObject>(meshResource, materialInterfaces, defaultInfo, node.ToParentMatrix);

			for (const auto& transformAnimNode : transformAnimationData.NodeClips)
			{
				if (transformAnimNode.NodeName == node.Name)
				{
					meshEffectNode.Keyframes = transformAnimNode.Keyframes;
				}
			}

			auto unAnimFind = uvAnimationData.NodeClips.find(node.Name);

			if (unAnimFind != uvAnimationData.NodeClips.end())
			{
				meshEffectNode.UVKeyframes = unAnimFind->second.UVData;
			}

			meshEffectObject->addNode(std::move(meshEffectNode));
		}

		mMeshEffectObjects.insert(meshEffectObject);
		return meshEffectObject;
	}

	void D3D11ObjectManager::DeleteStaticMeshObject(IStaticMeshObject* staticMeshObject)
	{
		auto find = mStaticMeshObjects.find(staticMeshObject);

		if (find != mStaticMeshObjects.end())
		{
			mStaticMeshObjects.erase(find);
			mStaticMeshObjectDeleteQueue.push(staticMeshObject);
		}
	}
	void D3D11ObjectManager::DeleteSkinnedMeshObject(ISkinnedMeshObject* skinnedMeshObject)
	{
		auto find = mSkinnedMeshObjects.find(skinnedMeshObject);

		if (find != mSkinnedMeshObjects.end())
		{
			mSkinnedMeshObjects.erase(find);
			mSkinnedMeshObjectDeleteQueue.push(skinnedMeshObject);
		}
	}
	void D3D11ObjectManager::DeleteTerrainMeshObject(ITerrainMeshObject* terrainMeshObjectInterface)
	{
		auto find = mTerrainMeshObjects.find(terrainMeshObjectInterface);

		if (find != mTerrainMeshObjects.end())
		{
			mTerrainMeshObjects.erase(find);
			mTerrainMeshObjectDeleteQueue.push(terrainMeshObjectInterface);
		}
	}
	void D3D11ObjectManager::DeleteParticleObject(IParticleObject* particleObject)
	{
		auto find = mParticleObjects.find(particleObject);

		if (find != mParticleObjects.end())
		{
			mParticleObjects.erase(find);
			mParticleObjectDeleteQueue.push(particleObject);
		}
	}
	void D3D11ObjectManager::DeleteDecalObject(IDecalObject* decalObject)
	{
		auto find = mDecalObjects.find(decalObject);

		if (find != mDecalObjects.end())
		{
			mDecalObjects.erase(find);
			mDecalObjectDeleteQueue.push(decalObject);
		}
	}
	void D3D11ObjectManager::DeleteTrailObject(ITrailObject* trailObject)
	{
		auto find = mTrailObjects.find(trailObject);

		if (find != mTrailObjects.end())
		{
			mTrailObjects.erase(find);
			mTrailObjectDeleteQueue.push(trailObject);
		}
	}

	void D3D11ObjectManager::DeleteMeshEffectObject(IMeshEffectObject* meshEffectObject)
	{
		auto find = mMeshEffectObjects.find(meshEffectObject);

		if (find != mMeshEffectObjects.end())
		{
			mMeshEffectObjects.erase(find);
			mMeshEffectObjectDeleteQueue.push(meshEffectObject);
		}
	}

	void D3D11ObjectManager::SetTerrainMeshObject(const std::shared_ptr<D3D11Device>& device, ITerrainMeshObject* iTerrainMeshObject, const TerrainMaterialInfo& material)
	{
		TerrainMeshObject* terrainMeshObject = static_cast<TerrainMeshObject*>(iTerrainMeshObject);
		terrainMeshObject->SetTerrainMaterial(device, material);
	}

	graphics::IProbeObject* D3D11ObjectManager::CreateProbeObject(std::shared_ptr<IStaticMesh> staticMesh, const DirectX::SimpleMath::Matrix& transform, int index)
	{
		IProbeObject* probeObject = new ProbeObject(staticMesh, transform, index);
		mProbeObjects.insert(probeObject);

		return probeObject;
	}

	void D3D11ObjectManager::DeleteProbeObject(IProbeObject* probeObject)
	{
		auto find = mProbeObjects.find(probeObject);

		if (find != mProbeObjects.end())
		{
			mProbeObjects.erase(find);
			mProbeObjectDeleteQueue.push(probeObject);
		}
	}
}