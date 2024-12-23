#include "RenderingSystem.h"

#include <filesystem>
#include <spdlog/spdlog.h>
#include <ostream>
#include <fstream>
#include "../FQGraphics/IFQGraphics.h"
#include "../FQGameModule/GameModule.h"
#include "../FQGameModule/Transform.h"
#include "../FQGameModule/SkinnedMeshRenderer.h"
#include "../FQGameModule/StaticMeshRenderer.h"
#include "../FQGameModule/Sequence.h"
#include "../FQGameModule/Terrain.h"
#include "../FQGameModule/UVAnimator.h"
#include "../FQGameModule/MaterialAnimator.h"
#include "../FQGameModule/BlendUVAnimator.h"
#include "../FQGameModule/Decal.h"
#include "../FQGameModule/PostProcessing.h"
#include "../FQGameModule/ImageUI.h"
#include "../FQGameModule/TextUI.h"
#include "../FQGameModule/SpriteAnimationUI.h"
#include "../FQGameModule/ClothCollider.h"
#include "../FQCommon/FQCommonGraphics.h"
#include "../FQCommon/FQPath.h"
#include "../FQCommon/IFQRenderObject.h"

#include "GameProcess.h"
#include "AnimationSystem.h"
#include "PhysicsSystem.h"
#include "ResourceSystem.h"
fq::game_engine::RenderingSystem::RenderingSystem()
	:mGameProcess(nullptr)
	, mOnAddGameObjectHandler{}
	, mOnLoadSceneHandler{}
	, mOnUnloadSceneHandler{}
	, mDestroyedGameObjectHandler{}
	, mRemoveComponentHandler{}
	, mAddComponentHandler{}
	, mbIsGameLoaded(false)
	, mResourceSystem(nullptr)
{}

fq::game_engine::RenderingSystem::~RenderingSystem()
{}

void fq::game_engine::RenderingSystem::Initialize(GameProcess* gameProcess)
{
	mGameProcess = gameProcess;
	mResourceSystem = gameProcess->mResourceSystem.get();

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

	mDebugDrawLayHanlder = eventMgr->
		RegisterHandle<fq::event::DrawDebugLay>([this](fq::event::DrawDebugLay event)
			{
				mGameProcess->mGraphics->DrawRay(event.rayInfo);
			});

	mUIRenderHandler = eventMgr->
		RegisterHandle<fq::event::UIRender>(this, &RenderingSystem::OnUIRender);
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

				if (meshObject == nullptr)
				{
					return;
				}

				auto nodeHierarchyInstanceOrNull = meshObject->GetNodeHierarchyInstance();

				if (!mesh.GetUseTransform())
				{
					meshObject->SetTransform(DirectX::SimpleMath::Matrix::Identity);
				}
				else if (nodeHierarchyInstanceOrNull == nullptr)
				{
					meshObject->SetTransform(transform.GetWorldMatrix());
				}
				else
				{
					meshObject->SetTransform(nodeHierarchyInstanceOrNull->GetTransform());
				}

				fq::graphics::MaterialInstanceInfo materialInstanceInfo = mesh.GetMaterialInstanceInfo();

				// viewComponets가 모두 포함하는 오브젝트만 비용없이 가져온다면 해당 로직 수정해줘야 함
				auto* materialAnimator = object.GetComponent<MaterialAnimator>();

				if (materialAnimator != nullptr)
				{
					{
						const auto& info = materialAnimator->GetAlphaAnimatorInfo();
						materialInstanceInfo.bUseInstanceAlpha = info.bIsUsed;
						materialInstanceInfo.Alpha = info.Alpha;
					}
					{
						const auto& info = materialAnimator->GetDissolveAnimatorInfo();
						materialInstanceInfo.bUseDissolveCutoff = info.bIsUsed;
						materialInstanceInfo.DissolveCutoff = info.DissolveCutoff;
					}
				}

				auto* blendUVAnimator = object.GetComponent<BlendUVAnimator>();

				if (blendUVAnimator != nullptr)
				{
					const auto& keyframe = blendUVAnimator->GetUVKeyframe();
					materialInstanceInfo.bUseBlendUV = true;
					materialInstanceInfo.BlendUVScaleOffset = { keyframe.Scale.x, keyframe.Scale.y, keyframe.Translation.x, keyframe.Translation.y };
				}

				mesh.SetMaterialInstanceInfo(materialInstanceInfo);
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

					auto* materialAnimator = object.GetComponent<MaterialAnimator>();

					if (materialAnimator != nullptr)
					{
						fq::graphics::MaterialInstanceInfo materialInstanceInfo = mesh.GetMaterialInstanceInfo();
						{
							const auto& info = materialAnimator->GetAlphaAnimatorInfo();
							materialInstanceInfo.bUseInstanceAlpha = info.bIsUsed;
							materialInstanceInfo.Alpha = info.Alpha;
						}
						{
							const auto& info = materialAnimator->GetDissolveAnimatorInfo();
							materialInstanceInfo.bUseDissolveCutoff = info.bIsUsed;
							materialInstanceInfo.DissolveCutoff = info.DissolveCutoff;
						}

						mesh.SetMaterialInstanceInfo(materialInstanceInfo);
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
				if (object.GetName() == "PostProcessing")
				{
					auto info = postProcessing.GetPostProcessingInfo();
					mGameProcess->mGraphics->SetIsUsePostProcessing(true);
					mGameProcess->mGraphics->SetPostProcessingInfo(info);
				}
			});

	auto postProcessingView = scene->GetComponentView<game_module::PostProcessing>();

	if (postProcessingView.begin() == postProcessingView.end())
	{
		mGameProcess->mGraphics->SetIsUsePostProcessing(false);
	}

	for (auto iter = mOnImageUIRenderEventActivatedObjects.begin(); iter != mOnImageUIRenderEventActivatedObjects.end();)
	{
		// 죽을 예정이면 제거
		if (iter->first->IsDestroyed())
		{
			iter = mOnImageUIRenderEventActivatedObjects.erase(iter);
		}
		else
		{
			auto object = iter->first;
			auto imageUI = object->GetComponent<ImageUI>();

			// 이미지 UI 컴포넌트가 없다면 제거
			if (imageUI == nullptr)
			{
				iter = mOnImageUIRenderEventActivatedObjects.erase(iter);
				continue;
			}

			auto infos = imageUI->GetUIInfomations();

			for (auto imageObjectIndex : iter->second)
			{
				infos[imageObjectIndex].isRender = false;
			}

			imageUI->SetUIInfomations(infos);

			++iter;
		}
	}

	for (auto iter = mOnTextUIRenderEventActivatedObjects.begin(); iter != mOnTextUIRenderEventActivatedObjects.end();)
	{
		if ((*iter)->IsDestroyed())
		{
			iter = mOnTextUIRenderEventActivatedObjects.erase(iter);
		}
		else
		{
			auto object = *iter;
			auto textUI = object->GetComponent<TextUI>();

			if (textUI == nullptr)
			{
				iter = mOnTextUIRenderEventActivatedObjects.erase(iter);
				continue;
			}

			textUI->SetIsRender(false);

			++iter;
		}
	}

	for (auto iter = mOnSpriteUIRenderEventActivatedObjects.begin(); iter != mOnSpriteUIRenderEventActivatedObjects.end();)
	{
		if ((*iter)->IsDestroyed())
		{
			iter = mOnSpriteUIRenderEventActivatedObjects.erase(iter);
		}
		else
		{
			auto object = *iter;
			auto spriteAnimUI = object->GetComponent<SpriteAnimationUI>();

			if (spriteAnimUI == nullptr)
			{
				iter = mOnSpriteUIRenderEventActivatedObjects.erase(iter);
				continue;
			}

			auto info = spriteAnimUI->GetSpriteInfo();
			spriteAnimUI->SetIsRender(false);

			++iter;
		}
	}
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
		loadSequenceAnimation(&object);
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

	// 6. SequenceAnimation
	loadSequenceAnimation(gameObject);
}

void fq::game_engine::RenderingSystem::loadSkinnedMeshRenderer(fq::game_module::GameObject* object)
{
	if (!object->HasComponent<fq::game_module::SkinnedMeshRenderer>())
	{
		return;
	}

	auto skinnedMeshRenderer = object->GetComponent<fq::game_module::SkinnedMeshRenderer>();
	auto modelPath = skinnedMeshRenderer->GetModelPath();
	auto key = GetModelKey(modelPath);
	auto meshName = skinnedMeshRenderer->GetMeshName();
	auto materialPaths = skinnedMeshRenderer->GetMaterialPaths();
	auto meshInfo = skinnedMeshRenderer->GetMeshObjectInfomation();
	auto transform = object->GetComponent<fq::game_module::Transform>();

	if (!std::filesystem::exists(modelPath))
	{
		//spdlog::warn("[RenderingSystem] Model Path \"{}\" does not exist", modelPath);
		return;
	}

	if (!mResourceSystem->HasModel(modelPath))
	{
		mGameProcess->mResourceSystem->LoadModelResource(modelPath);
	}

	using namespace fq::graphics;

	// SkinnedMesh 생성
	std::vector<std::shared_ptr<IMaterial>> materialInterfaces;

	materialInterfaces.reserve(materialPaths.size());

	for (const auto& materialPath : materialPaths)
	{
		std::shared_ptr<fq::graphics::IMaterial> materialInterfaceOrNull = nullptr;

		if (!std::filesystem::exists(materialPath))
		{
			spdlog::warn("{} material path is invalid", object->GetName());

			const std::string DEFAULT_MATERIAL = "./resource/Material/Default.material";
			materialInterfaceOrNull = mGameProcess->mResourceSystem->GetMaterial(DEFAULT_MATERIAL);

			if (materialInterfaceOrNull == nullptr)
			{
				mGameProcess->mResourceSystem->LoadMaterial(DEFAULT_MATERIAL);
				materialInterfaceOrNull = mGameProcess->mResourceSystem->GetMaterial(DEFAULT_MATERIAL);
			}
		}
		else
		{
			materialInterfaceOrNull = mResourceSystem->GetMaterial(materialPath);

			if (materialInterfaceOrNull == nullptr)
			{
				mResourceSystem->LoadMaterial(materialPath);
				materialInterfaceOrNull = mResourceSystem->GetMaterial(materialPath);
			}
		}

		assert(materialInterfaceOrNull != nullptr);
		materialInterfaces.push_back(materialInterfaceOrNull);
	}

	auto meshInterface = mGameProcess->mResourceSystem->GetSkinnedMesh(modelPath, meshName);

	if (meshInterface != nullptr)
	{
		ISkinnedMeshObject* iSkinnedMeshObject = mGameProcess->mGraphics->CreateSkinnedMeshObject(meshInterface, materialInterfaces, skinnedMeshRenderer->GetMeshObjectInfomation(), transform->GetLocalMatrix());
		skinnedMeshRenderer->SetSkinnedMeshObject(iSkinnedMeshObject);
		skinnedMeshRenderer->SetMaterialInterfaces(materialInterfaces);
		skinnedMeshRenderer->SetMaterialInstanceInfo(skinnedMeshRenderer->GetMaterialInstanceInfo());
	}
}

void fq::game_engine::RenderingSystem::WriteAnimation(const fq::event::WriteAnimation& event)
{
	fq::common::AnimationClip animationClilp;
	animationClilp.Name = event.animationName;
	animationClilp.FrameCount = event.animationData.begin()->second.size();
	animationClilp.FramePerSecond = (1.f / 30.f);
	animationClilp.Duration = animationClilp.FrameCount * animationClilp.FramePerSecond;

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

	mGameProcess->mGraphics->WriteAnimation("./" + event.animationName + ".animation", animationClilp);
}

void fq::game_engine::RenderingSystem::loadStaticMeshRenderer(fq::game_module::GameObject* object)
{
	if (!object->HasComponent<fq::game_module::StaticMeshRenderer>())
	{
		return;
	}

	auto staticMeshRenderer = object->GetComponent<fq::game_module::StaticMeshRenderer>();
	auto modelPath = staticMeshRenderer->GetModelPath();

	// Model 생성
	if (!std::filesystem::exists(modelPath))
	{
		//spdlog::warn("[RenderingSystem] Model Path \"{}\" does not exist", modelPath);
		return;
	}

	if (!mResourceSystem->HasModel(modelPath))
	{
		mGameProcess->mResourceSystem->LoadModelResource(modelPath);
	}

	auto meshName = staticMeshRenderer->GetMeshName();
	auto materialPaths = staticMeshRenderer->GetMaterialPaths();
	auto meshInfo = staticMeshRenderer->GetMeshObjectInfomation();
	auto transform = object->GetComponent<fq::game_module::Transform>();

	using namespace fq::graphics;
	std::vector<std::shared_ptr<IMaterial>> materialInterfaces;

	materialInterfaces.reserve(materialPaths.size());

	for (const auto& materialPath : materialPaths)
	{
		std::shared_ptr<fq::graphics::IMaterial> materialInterfaceOrNull = nullptr;

		if (!std::filesystem::exists(materialPath))
		{
			spdlog::warn("{} material path is invalid", object->GetName());

			const std::string DEFAULT_MATERIAL = "./resource/Material/Default.material";
			materialInterfaceOrNull = mResourceSystem->GetMaterial(DEFAULT_MATERIAL);

			if (materialInterfaceOrNull == nullptr)
			{
				mResourceSystem->LoadMaterial(DEFAULT_MATERIAL);
				materialInterfaceOrNull = mResourceSystem->GetMaterial(DEFAULT_MATERIAL);
			}
		}
		else
		{
			materialInterfaceOrNull = mResourceSystem->GetMaterial(materialPath);

			if (materialInterfaceOrNull == nullptr)
			{
				mResourceSystem->LoadMaterial(materialPath);
				materialInterfaceOrNull = mResourceSystem->GetMaterial(materialPath);
			}
			assert(materialInterfaceOrNull != nullptr);
		}

		materialInterfaces.push_back(materialInterfaceOrNull);
	}

	auto meshInterface = mResourceSystem->GetStaticMesh(modelPath, meshName);

	if (meshInterface != nullptr)
	{
		if (object->HasComponent<fq::game_module::ClothCollider>())
		{
			auto deepCopiedMeshInterface = mGameProcess->mGraphics->CreateStaticMesh(meshInterface->GetMeshData());
			meshInterface = deepCopiedMeshInterface;
		}

		IStaticMeshObject* iStaticMeshObject = mGameProcess->mGraphics->CreateStaticMeshObject(meshInterface, materialInterfaces, staticMeshRenderer->GetMeshObjectInfomation(), transform->GetLocalMatrix());
		staticMeshRenderer->SetStaticMeshObject(iStaticMeshObject);
		staticMeshRenderer->SetMaterialInterfaces(materialInterfaces);
		staticMeshRenderer->SetLightmapUVScaleOffset(staticMeshRenderer->GetLightmapUVScaleOffset());
		staticMeshRenderer->SetLightmapIndex(staticMeshRenderer->GetLightmapIndex());
		staticMeshRenderer->SetIsStatic(staticMeshRenderer->GetIsStatic());
		staticMeshRenderer->SetIsRender(staticMeshRenderer->GetIsRender());
		staticMeshRenderer->SetMaterialInstanceInfo(staticMeshRenderer->GetMaterialInstanceInfo());
	}
}

void fq::game_engine::RenderingSystem::unloadAnimation(fq::game_module::GameObject* object)
{
	if (!object->HasComponent<game_module::Animator>())
	{
		return;
	}

	auto animator = object->GetComponent<game_module::Animator>();

	//	animator->SetController(nullptr);
	//	animator->SetNodeHierarchyInstance(nullptr);
	//	animator->SetNodeHierarchy(nullptr);
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

	auto uvAnimationInterfaceOrNull = mResourceSystem->GetUVAnimation(uvAnimationPath);

	if (uvAnimationInterfaceOrNull == nullptr)
	{
		mResourceSystem->LoadUVAnimation(uvAnimationPath);
		uvAnimationInterfaceOrNull = mResourceSystem->GetUVAnimation(uvAnimationPath);
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
			auto staticMeshObject = staticMeshRenderer->GetStaticMeshObject();

			if (staticMeshObject != nullptr)
			{
				staticMeshObject->SetUVAnimationInstance(uvAnimationInstanceInterface);
			}
		}
		if (child->HasComponent<game_module::Decal>())
		{
			auto decal = child->GetComponent<fq::game_module::Decal>();
			auto decalObject = decal->GetDecalObjectInterface();

			if (decalObject != nullptr)
			{
				decalObject->SetUVAnimationInstance(uvAnimationInstanceInterface);
			}
		}
	}
}

void fq::game_engine::RenderingSystem::loadSequenceAnimation(fq::game_module::GameObject* object)
{
	if (!object->HasComponent<game_module::Sequence>())
	{
		return;
	}

	auto sequence = object->GetComponent<fq::game_module::Sequence>();
	auto& animationContainerMap = sequence->GetAnimationContainer();

	auto& animationInfo = sequence->GetObjectAnimationInfo();

	for (auto& info : animationInfo)
	{
		std::vector<std::shared_ptr<fq::graphics::IAnimation>> animationContainer;

		for (auto& trackKey : info.animationTrackKeys)
		{
			if (trackKey.animationPath.empty())
			{
				spdlog::warn("{} trackKey animation file is empty", trackKey.animationPath);
				continue;
			}
			if (!std::filesystem::exists(trackKey.animationPath))
			{
				spdlog::warn("{} trackKey animation file is not exist", trackKey.animationPath);
				continue;
			}

			auto animationInterfaceOrNull = mGameProcess->mResourceSystem->GetAnimation(trackKey.animationPath);

			if (animationInterfaceOrNull == nullptr)
			{
				mGameProcess->mResourceSystem->LoadAnimation(trackKey.animationPath);
				animationInterfaceOrNull = mGameProcess->mResourceSystem->GetAnimation(trackKey.animationPath);
			}
			assert(animationInterfaceOrNull != nullptr);

			animationContainer.push_back(animationInterfaceOrNull);
		}

		animationContainerMap.insert(std::make_pair(info.targetObjectName, animationContainer));
	}
}


void fq::game_engine::RenderingSystem::OnDestroyedGameObject(const fq::event::OnDestoryedGameObject& event)
{
	unloadStaticMeshRenderer(event.object);
	unloadSkinnedMeshRenderer(event.object);
	unloadTerrain(event.object);
	unloadAnimation(event.object);
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

	if (!mResourceSystem->HasModel(terrainPath))
	{
		mGameProcess->mResourceSystem->LoadModelResource(terrainPath);
	}

	const fq::common::Model& modelData = mGameProcess->mResourceSystem->GetModel(terrainPath);
	auto mesh = modelData.Meshes[1];

	mPlaneMatrix = mesh.first.ToParentMatrix;

	auto staticMeshInterface = mGameProcess->mResourceSystem->GetStaticMesh(terrainPath, mesh.second.Name);

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

	// 터레인 라이트맵 관련 매개변수 세팅
	iTerrainMeshObject->SetIsStatic(terrain->GetIsStatic());
	iTerrainMeshObject->SetLightmapUVScaleOffset(terrain->GetLightmapUVScaleOffset());
	iTerrainMeshObject->SetLightmapIndex(terrain->GetLightmapIndex());
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

void fq::game_engine::RenderingSystem::OnUIRender(const fq::event::UIRender& event)
{
	using namespace fq::game_module;
	auto scene = mGameProcess->mSceneManager->GetCurrentScene();

	if (!event.bIsRenderingUI)
	{
		// 활성화 UI를 컨테이너에 저장하고 랜더링 여부를 Off시킴

		scene->ViewComponents<ImageUI>(
			[this](GameObject& object, ImageUI& imageUI)
			{
				if (!imageUI.GetIsApplyUIRenderEvent())
				{
					return;
				}

				auto infos = imageUI.GetUIInfomations();
				auto interfaceObjects = imageUI.GetImageObjects();
				assert(infos.size() == interfaceObjects.size());

				std::vector<size_t> imageObjectInterfaces;
				imageObjectInterfaces.reserve(infos.size());

				for (size_t i = 0; i < infos.size(); ++i)
				{
					if (infos[i].isRender)
					{
						imageObjectInterfaces.push_back(i);
						infos[i].isRender = false;
					}
				}

				imageUI.SetUIInfomations(infos);
				mOnImageUIRenderEventActivatedObjects.insert({ &object, imageObjectInterfaces });
			}
		);

		scene->ViewComponents<TextUI>(
			[this](GameObject& object, TextUI& textUI)
			{
				if (!textUI.GetIsApplyUIRenderEvent())
				{
					return;
				}

				auto info = textUI.GetTextInfo();

				if (info.IsRender)
				{
					textUI.SetIsRender(false);
					//textUI.SetTextInfoPlay(info);
					mOnTextUIRenderEventActivatedObjects.insert(&object);
				}
			}
		);

		scene->ViewComponents<SpriteAnimationUI>(
			[this](GameObject& object, SpriteAnimationUI& ui)
			{
				auto info = ui.GetSpriteInfo();

				if (info.isRender)
				{
					ui.SetIsRender(false);

					// info.isRender = false;
					// ui.SetSpriteInfo(info);

					mOnSpriteUIRenderEventActivatedObjects.insert(&object);
				}
			}
		);
	}
	else
	{
		// 캐쉬에 저장된 데이터의 랜더링 여부를 On 시킴
		for (auto eventObject : mOnImageUIRenderEventActivatedObjects)
		{
			auto object = eventObject.first;
			auto imageUI = object->GetComponent< ImageUI>();

			if (imageUI == nullptr)
			{
				continue;
			}

			auto infos = imageUI->GetUIInfomations();

			for (auto imageObjectIndex : eventObject.second)
			{
				infos[imageObjectIndex].isRender = true;
			}

			imageUI->SetUIInfomations(infos);
		}

		mOnImageUIRenderEventActivatedObjects.clear();

		for (auto eventObject : mOnTextUIRenderEventActivatedObjects)
		{
			auto object = eventObject;
			auto textUI = object->GetComponent<TextUI>();

			if (textUI == nullptr)
			{
				continue;
			}

			auto info = textUI->GetTextInfo();
			info.IsRender = true;
			textUI->SetIsRender(true);
			//textUI->SetTextInfoPlay(info);
		}

		mOnTextUIRenderEventActivatedObjects.clear();

		for (auto eventObject : mOnSpriteUIRenderEventActivatedObjects)
		{
			auto object = eventObject;
			auto spriteAnimUI = object->GetComponent<SpriteAnimationUI>();

			if (spriteAnimUI == nullptr)
			{
				continue;
			}

			auto info = spriteAnimUI->GetSpriteInfo();
			spriteAnimUI->SetIsRender(true);
			// info.isRender = true;
			// spriteAnimUI->SetSpriteInfo(info);
		}

		mOnSpriteUIRenderEventActivatedObjects.clear();
	}
}

unsigned int fq::game_engine::RenderingSystem::GetModelKey(const Path& modelPath, const Path& texturePath) const
{
	return entt::hashed_string((modelPath + texturePath).c_str()).value();
}

