#include "RenderingSystem.h"

#include <filesystem>
#include <spdlog/spdlog.h>
#include <ostream>
#include <fstream>
#include "../FQGraphics/IFQGraphics.h"
#include "../FQGameModule/GameModule.h"
#include "../FQCommon/FQPath.h"
#include "GameProcess.h"
#include "AnimationSystem.h"
#include "PhysicsSystem.h"

fq::game_engine::RenderingSystem::RenderingSystem()
	:mGameProcess(nullptr)
	, mOnAddGameObjectHandler{}
	, mOnLoadSceneHandler{}
	, mOnUnloadSceneHandler{}
	, mDestroyedGameObjectHandler{}
	, mRemoveComponentHandler{}
	, mAddComponentHandler{}
	, mbIsGameLoaded(false)
{}

fq::game_engine::RenderingSystem::~RenderingSystem()
{}

void fq::game_engine::RenderingSystem::Initialize(GameProcess* gameProcess)
{
	mGameProcess = gameProcess;

	// EventHandle 등록
	auto& eventMgr = mGameProcess->mEventManager;

	mOnLoadSceneHandler = eventMgr->
		RegisterHandle<fq::event::OnLoadScene>(this, &RenderingSystem::OnLoadScene);

	mOnUnloadSceneHandler = eventMgr->
		RegisterHandle<fq::event::OnUnloadScene>(this, &RenderingSystem::OnUnLoadScene);

	mOnAddGameObjectHandler = mGameProcess->mEventManager->
		RegisterHandle<fq::event::AddGameObject>(this, &RenderingSystem::OnAddGameObject);

	mDestroyedGameObjectHandler = mGameProcess->mEventManager->
		RegisterHandle<fq::event::OnDestoryedGameObject>(this, &RenderingSystem::OnDestroyedGameObject);

	mAddComponentHandler = eventMgr->
		RegisterHandle<fq::event::AddComponent>(this, &RenderingSystem::AddComponent);

	mRemoveComponentHandler = eventMgr->
		RegisterHandle<fq::event::RemoveComponent>(this, &RenderingSystem::RemoveComponent);

	mWriteAnimationHandler = eventMgr->
		RegisterHandle<fq::event::WriteAnimation>(this, &RenderingSystem::WriteAnimation);

	mSetViewportSizeHandler = eventMgr->
		RegisterHandle<fq::event::SetViewportSize>([this](fq::event::SetViewportSize event)
			{
				mGameProcess->mGraphics->SetViewportSize(event.width, event.height);
			});

	mUpdateMaterialInfosHandler = eventMgr->
		RegisterHandle<fq::event::UpdateMaterialInfo>(this, &RenderingSystem::OnUpdateMaterialInfos);
}

void fq::game_engine::RenderingSystem::Update(float dt)
{
	using namespace fq::game_module;

	auto scene = mGameProcess->mSceneManager->GetCurrentScene();

	scene->ViewComponents<Transform, Animator>(
		[dt](GameObject& object, Transform& transform, Animator& animator)
		{
			// staticMeshRenderer에서 애니메이터를 사용 시 애니메이터의 toWorld를 사용하기 위해서 추가
			auto nodeHierarchyInstance = animator.GetSharedNodeHierarchyInstance();

			if (nodeHierarchyInstance != nullptr)
			{
				nodeHierarchyInstance->SetTransform(transform.GetWorldMatrix());
			}
		});

	scene->ViewComponents<Transform, StaticMeshRenderer>
		([](GameObject& object, Transform& transform, StaticMeshRenderer& mesh)
			{
				auto meshObject = mesh.GetStaticMeshObject();

				if (meshObject)
				{
					auto nodeHierarchyInstanceOrNull = meshObject->GetNodeHierarchyInstance();

					if (nodeHierarchyInstanceOrNull == nullptr)
					{
						meshObject->SetTransform(transform.GetWorldMatrix());
					}
					else
					{
						meshObject->SetTransform(nodeHierarchyInstanceOrNull->GetTransform());
					}
				}
			});

	scene->ViewComponents<Transform, SkinnedMeshRenderer>
		([](GameObject& object, Transform& transform, SkinnedMeshRenderer& mesh)
			{
				auto meshObject = mesh.GetSkinnedMeshObject();
				if (meshObject)
				{
					GameObject* parentObject = object.GetParent();

					if (parentObject != nullptr)
					{
						meshObject->SetTransform(transform.GetWorldMatrix());
					}
					else
					{
						meshObject->SetTransform(DirectX::SimpleMath::Matrix::Identity);
					}
				}
			});

	scene->ViewComponents<Transform, Terrain>
		([this](GameObject& object, Transform& transform, Terrain& mesh)
			{
				auto meshObject = mesh.GetTerrainMeshObject();
				if (meshObject)
				{
					meshObject->SetTransform(mPlaneMatrix * transform.GetWorldMatrix());
				}
			});

	scene->ViewComponents<PostProcessing>
		([this](GameObject& object, PostProcessing& postProcessing)
			{
				auto info = postProcessing.GetPostProcessingInfo();
				mGameProcess->mGraphics->SetPostProcessingInfo(info);
			});
}

void fq::game_engine::RenderingSystem::OnLoadScene()
{
	auto scene = mGameProcess->mSceneManager->GetCurrentScene();

	// 1. Scene에 배치한 리소르를 로드 
	for (auto& object : scene->GetObjectView(true))
	{
		loadStaticMeshRenderer(&object);
		loadSkinnedMeshRenderer(&object);
		loadTerrain(&object);
	}

	// 2. 애니메이션을 연결
	for (auto& object : scene->GetObjectView(true))
	{
		loadAnimation(&object);
		loadUVAnimation(&object);
	}

	// 2. PrefabInstance를 로드

	// 3. SkyBox Load
	auto scenePath = fq::path::GetScenePath() / scene->GetSceneName();

	if (std::filesystem::exists(scenePath))
	{
		fq::game_module::SkyBox skybox;
		skybox.Load(scenePath);

		if (skybox.HasSkyBox())
			mGameProcess->mGraphics->SetSkyBox(skybox.mSkyBox);

		if (skybox.HasIBLTexture())
			mGameProcess->mGraphics->SetIBLTexture(skybox.mDiffuse, skybox.mSpecular, skybox.mBrdfLUT);
	}

	mbIsGameLoaded = true;
}

void fq::game_engine::RenderingSystem::OnUnLoadScene()
{
	// 1. Model Unload
	unloadAllModel();

	mbIsGameLoaded = false;
}

void fq::game_engine::RenderingSystem::OnAddGameObject(const fq::event::AddGameObject& event)
{
	// 씬이 로드된 시점에서만 처리합니다
	if (!mbIsGameLoaded)
	{
		return;
	}
	auto gameObject = event.object;

	// 오브젝트에 필요한 리소스를 로드합니다

	// 1. StaticMesh 
	loadStaticMeshRenderer(gameObject);

	// 2. SkinnedMesh
	loadSkinnedMeshRenderer(gameObject);

	// 3. Animation
	loadAnimation(gameObject);

	// 4. Terrain
	loadTerrain(gameObject);

	// 5. UVAnimation
	loadUVAnimation(gameObject);
}

void fq::game_engine::RenderingSystem::loadSkinnedMeshRenderer(fq::game_module::GameObject* object)
{
	if (!object->HasComponent<fq::game_module::SkinnedMeshRenderer>())
	{
		return;
	}

	auto skinnedMeshRenderer = object->GetComponent<fq::game_module::SkinnedMeshRenderer>();
	auto modelPath = skinnedMeshRenderer->GetModelPath();
	auto texturePath = skinnedMeshRenderer->GetTexturePath();
	auto key = GetModelKey(modelPath, texturePath);
	auto meshName = skinnedMeshRenderer->GetMeshName();
	auto materialPaths = skinnedMeshRenderer->GetMaterialPaths();
	auto meshInfo = skinnedMeshRenderer->GetMeshObjectInfomation();
	auto transform = object->GetComponent<fq::game_module::Transform>();

	// 지울 예정
	auto materialNames = skinnedMeshRenderer->GetMaterials();
	auto materialInfos = skinnedMeshRenderer->GetMaterialInfos();

	if (!std::filesystem::exists(modelPath))
	{
		SPDLOG_WARN("\"{}\" does not exist", modelPath);
	}
	else
	{
		LoadModel(modelPath, texturePath);
	}

	using namespace fq::graphics;

	// SkinnedMesh 생성
	std::vector<std::shared_ptr<IMaterial>> materialInterfaces;

	if (!materialPaths.empty())
	{
		materialInterfaces.reserve(materialPaths.size());

		for (const auto& materialPath : materialPaths)
		{
			if (!std::filesystem::exists(materialPath))
			{
				spdlog::warn("{} materialPath is invalid", object->GetName());
				continue;
			}

			std::shared_ptr<fq::graphics::IMaterial> materialInterfaceOrNull = mGameProcess->mGraphics->GetMaterialOrNull(materialPath);

			if (materialInterfaceOrNull == nullptr)
			{
				const graphics::MaterialInfo& materialInfo = mGameProcess->mGraphics->ReadMaterialInfo(materialPath);
				materialInterfaceOrNull = mGameProcess->mGraphics->CreateMaterial(materialPath, materialInfo);
			}
			assert(materialInterfaceOrNull != nullptr);

			materialInterfaces.push_back(materialInterfaceOrNull);
		}
	}
	else
	{
		size_t i = 0;

		for (const auto& materialName : materialNames)
		{
			auto materialInterface = mGameProcess->mGraphics->GetMaterialByModelPathOrNull(key, materialName);

			if (materialInterface != nullptr)
			{
				const auto& materialInfos = skinnedMeshRenderer->GetMaterialInfos();

				if (materialInfos.size() > i)
				{
					materialInterface->SetInfo(materialInfos[i]);
				}

				materialInterfaces.push_back(materialInterface);
			}

			++i;
		}
	}

	auto meshInterface = mGameProcess->mGraphics->GetSkinnedMeshByModelPathOrNull(key, meshName);

	if (meshInterface != nullptr)
	{
		ISkinnedMeshObject* iSkinnedMeshObject = mGameProcess->mGraphics->CreateSkinnedMeshObject(meshInterface, materialInterfaces, skinnedMeshRenderer->GetMeshObjectInfomation(), transform->GetLocalMatrix());
		skinnedMeshRenderer->SetSkinnedMeshObject(iSkinnedMeshObject);
		skinnedMeshRenderer->SetMaterialInterfaces(materialInterfaces);
		skinnedMeshRenderer->SetMaterialInfos(materialInfos);
	}
}

void fq::game_engine::RenderingSystem::WriteAnimation(const fq::event::WriteAnimation& event)
{
	static std::vector<fq::common::AnimationClip> mAnimationClips;

	fq::common::AnimationClip animationClilp;
	animationClilp.Name = event.animationName;
	animationClilp.FrameCount = event.animationData.begin()->second.size();
	animationClilp.FramePerSecond = (1.f / 30.f);
	animationClilp.Duration = (1.f / 30.f);

	for (const auto& data : event.animationData)
	{
		fq::common::NodeClip nodeClip;
		nodeClip.NodeName = data.first;

		for (int i = 0; i < data.second.size(); i++)
		{
			DirectX::SimpleMath::Matrix curMatrix = data.second[i];
			fq::common::Keyframe keyFrame;
			curMatrix.Decompose(keyFrame.Scale, keyFrame.Rotation, keyFrame.Translation);
			keyFrame.TimePos = (1.f / 30.f) * i;
			nodeClip.Keyframes.push_back(keyFrame);
		}
		animationClilp.NodeClips.push_back(nodeClip);
	}

	mAnimationClips.push_back(animationClilp);

	if (mAnimationClips.size() >= event.animationSize)
	{
		fq::common::Model model;
		model.Animations = mAnimationClips;
		mGameProcess->mGraphics->WriteModel("./" + event.animationName + ".model", model);
		mAnimationClips.clear();
	}
}

void fq::game_engine::RenderingSystem::loadStaticMeshRenderer(fq::game_module::GameObject* object)
{
	if (!object->HasComponent<fq::game_module::StaticMeshRenderer>())
	{
		return;
	}

	auto staticMeshRenderer = object->GetComponent<fq::game_module::StaticMeshRenderer>();
	auto modelPath = staticMeshRenderer->GetModelPath();
	auto texturePath = staticMeshRenderer->GetTexturePath();

	// Model 생성
	if (!std::filesystem::exists(modelPath))
	{
		SPDLOG_WARN("[RenderingSystem] Model Path \"{}\" does not exist", modelPath);
	}
	else
	{
		LoadModel(modelPath, texturePath);
	}

	auto key = GetModelKey(modelPath, texturePath);
	auto meshName = staticMeshRenderer->GetMeshName();
	auto materialPaths = staticMeshRenderer->GetMaterialPaths();
	auto meshInfo = staticMeshRenderer->GetMeshObjectInfomation();
	auto transform = object->GetComponent<fq::game_module::Transform>();

	// 지울 예정
	auto materialInfos = staticMeshRenderer->GetMaterialInfos();
	auto materialNames = staticMeshRenderer->GetMaterials();

	using namespace fq::graphics;
	std::vector<std::shared_ptr<IMaterial>> materialInterfaces;

	if (!materialPaths.empty())
	{
		materialInterfaces.reserve(materialPaths.size());

		for (const auto& materialPath : materialPaths)
		{
			if (!std::filesystem::exists(materialPath))
			{
				spdlog::warn("{} materialPath is invalid", object->GetName());
				continue;
			}

			std::shared_ptr<fq::graphics::IMaterial> materialInterfaceOrNull = mGameProcess->mGraphics->GetMaterialOrNull(materialPath);

			if (materialInterfaceOrNull == nullptr)
			{
				const graphics::MaterialInfo& materialInfo = mGameProcess->mGraphics->ReadMaterialInfo(materialPath);
				materialInterfaceOrNull = mGameProcess->mGraphics->CreateMaterial(materialPath, materialInfo);
			}
			assert(materialInterfaceOrNull != nullptr);

			materialInterfaces.push_back(materialInterfaceOrNull);
		}
	}
	else
	{
		size_t i = 0;

		for (const auto& materialName : materialNames)
		{
			auto materialInterface = mGameProcess->mGraphics->GetMaterialByModelPathOrNull(key, materialName);

			if (materialInterface != nullptr)
			{
				const auto& materialInfos = staticMeshRenderer->GetMaterialInfos();

				if (materialInfos.size() > i)
				{
					materialInterface->SetInfo(materialInfos[i]);
				}

				materialInterfaces.push_back(materialInterface);
			}

			++i;
		}
	}

	auto meshInterface = mGameProcess->mGraphics->GetStaticMeshByModelPathOrNull(key, meshName);

	if (meshInterface != nullptr)
	{
		IStaticMeshObject* iStaticMeshObject = mGameProcess->mGraphics->CreateStaticMeshObject(meshInterface, materialInterfaces, staticMeshRenderer->GetMeshObjectInfomation(), transform->GetLocalMatrix());
		staticMeshRenderer->SetStaticMeshObject(iStaticMeshObject);
		staticMeshRenderer->SetMaterialInterfaces(materialInterfaces);
		staticMeshRenderer->SetMaterialInfos(materialInfos);
	}
}

void fq::game_engine::RenderingSystem::loadAnimation(fq::game_module::GameObject* object)
{
	if (!object->HasComponent<game_module::Animator>())
	{
		return;
	}

	// AnimatorController 로드 요청
	bool check = mGameProcess->mAnimationSystem->LoadAnimatorController(object);

	if (!check) return;

	auto animator = object->GetComponent<fq::game_module::Animator>();
	auto nodeHierarchyInstance = animator->GetSharedNodeHierarchyInstance();
	auto nodeHierarchy = nodeHierarchyInstance->GetNodeHierarchy();

	std::function<void(fq::game_module::GameObject*)> setNodeHierarchyRecursive = [&nodeHierarchyInstance, &nodeHierarchy, &setNodeHierarchyRecursive](fq::game_module::GameObject* object)
		{
			for (auto& child : object->GetChildren())
			{
				if (child->HasComponent<game_module::SkinnedMeshRenderer>())
				{
					auto meshRenderer = child->GetComponent<fq::game_module::SkinnedMeshRenderer>();
					auto meshObject = meshRenderer->GetSkinnedMeshObject();

					if (meshObject != nullptr)
					{
						meshObject->SetNodeHierarchyInstance(nodeHierarchyInstance);
					}
				}
				if (child->HasComponent<game_module::StaticMeshRenderer>())
				{
					auto meshRenderer = child->GetComponent<fq::game_module::StaticMeshRenderer>();
					auto meshObject = meshRenderer->GetStaticMeshObject();

					const auto& nodeName = meshObject->GetStaticMesh()->GetMeshData().NodeName;
					unsigned int index = 0;

					/* 소켓과 같은 스태틱 매쉬는 연결되지 않도록 하기 위해
					메쉬의 노드 이름과 계층 구조에 포함된 노드 이름이 동일할 경우에만 연결됨 */
					if (meshObject != nullptr && nodeHierarchy->TryGetBoneIndex(nodeName, &index))
					{
						meshObject->SetNodeHierarchyInstance(nodeHierarchyInstance);
						meshObject->SetReferenceBoneIndex(index);
					}
				}

				setNodeHierarchyRecursive(child);
			}
		};

	// 메쉬에 계층구조 연결
	setNodeHierarchyRecursive(object);
}

void fq::game_engine::RenderingSystem::loadUVAnimation(fq::game_module::GameObject* object)
{
	if (!object->HasComponent<game_module::UVAnimator>())
	{
		return;
	}

	auto animator = object->GetComponent<fq::game_module::UVAnimator>();

	const auto uvAnimationPath = animator->GetUVAnimationPath();

	if (!std::filesystem::exists(uvAnimationPath))
	{
		spdlog::warn("{} uv animation file not exist", object->GetName());
		return;
	}

	auto uvAnimationInterfaceOrNull = mGameProcess->mGraphics->GetUVAnimationOrNull(uvAnimationPath);

	if (uvAnimationInterfaceOrNull == nullptr)
	{
		const auto& uvAnimationData = mGameProcess->mGraphics->ReadUVAnimation(uvAnimationPath);
		uvAnimationInterfaceOrNull = mGameProcess->mGraphics->CreateUVAnimation(uvAnimationPath, uvAnimationData);
	}
	assert(uvAnimationInterfaceOrNull != nullptr);

	auto uvAnimationInstanceInterface = uvAnimationInterfaceOrNull->CreateUVAnimationInstance();
	animator->SetIUVAnimation(uvAnimationInterfaceOrNull);
	animator->SetIUVAnimationInstance(uvAnimationInstanceInterface);

	// 자식 계층에 애니메이션 인스턴스 연결
	for (auto& child : object->GetChildren())
	{
		if (child->HasComponent<game_module::StaticMeshRenderer>())
		{
			auto staticMeshRenderer = child->GetComponent<fq::game_module::StaticMeshRenderer>();
			staticMeshRenderer->GetStaticMeshObject()->SetUVAnimationInstance(uvAnimationInstanceInterface);
		}
		if (child->HasComponent<game_module::Decal>())
		{
			auto decal = child->GetComponent<fq::game_module::Decal>();
			decal->GetDecalObjectInterface()->SetUVAnimationInstance(uvAnimationInstanceInterface);
		}
	}
}

void fq::game_engine::RenderingSystem::LoadModel(const Path& path, const std::string& texturePath /*= {}*/)
{
	if (!std::filesystem::exists(path))
	{
		spdlog::warn("[RenderingSystem] Load Model Failed \"{}\" ", path);
		return;
	}

	auto key = GetModelKey(path, texturePath);
	auto iter = mLoadModels.find(key);

	if (iter == mLoadModels.end())
	{
		std::filesystem::path texture = texturePath.empty() ?
			std::filesystem::path(path).remove_filename() : texturePath;

		auto key = GetModelKey(path, texturePath);
		mGameProcess->mGraphics->CreateModelResource(key, path, texture);
		mLoadModels.insert(key);
	}
}

void fq::game_engine::RenderingSystem::unloadAllModel()
{
	for (auto& modelKey : mLoadModels)
	{
		mGameProcess->mGraphics->DeleteModelResource(modelKey);
	}

	mLoadModels.clear();
}

void fq::game_engine::RenderingSystem::OnDestroyedGameObject(const fq::event::OnDestoryedGameObject& event)
{
	unloadStaticMeshRenderer(event.object);
	unloadSkinnedMeshRenderer(event.object);
	unloadTerrain(event.object);
}

void fq::game_engine::RenderingSystem::unloadStaticMeshRenderer(fq::game_module::GameObject* object)
{
	if (!object->HasComponent<fq::game_module::StaticMeshRenderer>())
	{
		return;
	}

	auto staticMeshRenderer = object->GetComponent<fq::game_module::StaticMeshRenderer>();
	auto staticMesh = staticMeshRenderer->GetStaticMeshObject();
	mGameProcess->mGraphics->DeleteStaticMeshObject(staticMesh);
	staticMeshRenderer->SetStaticMeshObject(nullptr);
}


void fq::game_engine::RenderingSystem::unloadSkinnedMeshRenderer(fq::game_module::GameObject* object)
{
	if (!object->HasComponent<fq::game_module::SkinnedMeshRenderer>())
	{
		return;
	}

	auto staticMeshRenderer = object->GetComponent<fq::game_module::SkinnedMeshRenderer>();
	auto staticMesh = staticMeshRenderer->GetSkinnedMeshObject();
	mGameProcess->mGraphics->DeleteSkinnedMeshObject(staticMesh);
	staticMeshRenderer->SetSkinnedMeshObject(nullptr);
}

void fq::game_engine::RenderingSystem::AddComponent(const fq::event::AddComponent& event)
{
	if (event.id == entt::resolve<fq::game_module::StaticMeshRenderer>().id())
	{
		loadStaticMeshRenderer(event.component->GetGameObject());
	}

	if (event.id == entt::resolve<fq::game_module::SkinnedMeshRenderer>().id())
	{
		loadSkinnedMeshRenderer(event.component->GetGameObject());
	}

	if (event.id == entt::resolve<fq::game_module::Terrain>().id())
	{
		loadTerrain(event.component->GetGameObject());
	}
}

void fq::game_engine::RenderingSystem::RemoveComponent(const fq::event::RemoveComponent& event)
{
	if (event.id == entt::resolve<fq::game_module::StaticMeshRenderer>().id())
	{
		unloadStaticMeshRenderer(event.component->GetGameObject());
	}

	if (event.id == entt::resolve<fq::game_module::SkinnedMeshRenderer>().id())
	{
		unloadSkinnedMeshRenderer(event.component->GetGameObject());
	}

	if (event.id == entt::resolve<fq::game_module::Terrain>().id())
	{
		unloadTerrain(event.component->GetGameObject());
	}
}

bool fq::game_engine::RenderingSystem::IsLoadedModel(unsigned int key)
{
	return mLoadModels.find(key) != mLoadModels.end();
}

void fq::game_engine::RenderingSystem::loadTerrain(fq::game_module::GameObject* object)
{
	if (!object->HasComponent<fq::game_module::Terrain>())
	{
		return;
	}

	auto terrain = object->GetComponent<fq::game_module::Terrain>();
	auto transform = object->GetComponent<fq::game_module::Transform>();

	std::string terrainPath = "./resource/internal/terrain/Plane.model";

	// Model 생성
	LoadModel(terrainPath);
	auto key = GetModelKey(terrainPath, {});
	const fq::common::Model& modelData = mGameProcess->mGraphics->GetModel(key);
	auto mesh = modelData.Meshes[1];

	mPlaneMatrix = mesh.first.ToParentMatrix;

	auto staticMeshInterface = mGameProcess->mGraphics->GetStaticMeshByModelPathOrNull(key, mesh.second.Name); ;

	fq::graphics::ITerrainMeshObject* iTerrainMeshObject = mGameProcess->mGraphics->CreateTerrainMeshObject(staticMeshInterface, mesh.first.ToParentMatrix * transform->GetWorldMatrix());
	terrain->SetTerrainMeshObject(iTerrainMeshObject);

	fq::graphics::TerrainMaterialInfo info;
	info.AlPhaFileName = terrain->GetAlphaMap();
	info.Layers = terrain->GetTerrainLayers();
	info.TerrainWidth = terrain->GetWidth();
	info.TerrainHeight = terrain->GetHeight();

	info.TextureWidth = terrain->GetTextureWidth();
	info.TextureHeight = terrain->GetTextureHeight();
	info.HeightScale = terrain->GetHeightScale();
	info.HeightFileName = terrain->GetHeightMap();

	mGameProcess->mGraphics->SetTerrainMeshObject(iTerrainMeshObject, info);

	// Terrain Collider 생성요청 
	mGameProcess->mPhysicsSystem->AddTerrainCollider(object);
}

void fq::game_engine::RenderingSystem::unloadTerrain(fq::game_module::GameObject* object)
{
	if (!object->HasComponent<fq::game_module::Terrain>())
	{
		return;
	}

	auto terrain = object->GetComponent<fq::game_module::Terrain>();
	auto terrainMeshObject = terrain->GetTerrainMeshObject();
	mGameProcess->mGraphics->DeleteTerrainMeshObject(terrainMeshObject);
	terrain->SetTerrainMeshObject(nullptr);
}

void fq::game_engine::RenderingSystem::OnUpdateMaterialInfos(const fq::event::UpdateMaterialInfo& event)
{
	using namespace fq::game_module;

	auto scene = mGameProcess->mSceneManager->GetCurrentScene();

	scene->ViewComponents<StaticMeshRenderer>
		([](GameObject& object, StaticMeshRenderer& mesh)
			{
				mesh.UpdateMaterialInfoByMaterialInterface();
			});

	scene->ViewComponents<SkinnedMeshRenderer>
		([](GameObject& object, SkinnedMeshRenderer& mesh)
			{
				mesh.UpdateMaterialInfoByMaterialInterface();
			});
}

unsigned int fq::game_engine::RenderingSystem::GetModelKey(const Path& modelPath, const Path& texturePath) const
{
	return entt::hashed_string((modelPath + texturePath).c_str()).value();
}
