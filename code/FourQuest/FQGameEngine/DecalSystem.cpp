#include "DecalSystem.h"

#include "GameProcess.h"
#include "../FQGraphics/IFQGraphics.h"
#include "../FQGameModule/GameModule.h"
#include "../FQGameModule/Decal.h"
#include "../FQGameModule/Transform.h"
#include "../FQGameModule/DecalUVAnimator.h"

fq::game_engine::DecalSystem::DecalSystem()
	:mGameProcess(nullptr)
	, mbIsGameLoaded(false)
{}

fq::game_engine::DecalSystem::~DecalSystem()
{

}

void fq::game_engine::DecalSystem::Initialize(GameProcess* gameProcess)
{
	mGameProcess = gameProcess;

	// EventHandle µî·Ï
	auto& eventMgr = mGameProcess->mEventManager;

	mOnLoadSceneHandler = eventMgr->
		RegisterHandle<fq::event::OnLoadScene>(this, &DecalSystem::OnLoadScene);

	mOnUnloadSceneHandler = eventMgr->
		RegisterHandle<fq::event::OnUnloadScene>(this, &DecalSystem::OnUnLoadScene);

	mOnAddGameObjectHandler = mGameProcess->mEventManager->
		RegisterHandle<fq::event::AddGameObject>(this, &DecalSystem::OnAddGameObject);

	mDestroyedGameObjectHandler = mGameProcess->mEventManager->
		RegisterHandle<fq::event::OnDestoryedGameObject>(this, &DecalSystem::OnDestroyedGameObject);

	mAddComponentHandler = eventMgr->
		RegisterHandle<fq::event::AddComponent>(this, &DecalSystem::AddComponent);

	mRemoveComponentHandler = eventMgr->
		RegisterHandle<fq::event::RemoveComponent>(this, &DecalSystem::RemoveComponent);
}

void fq::game_engine::DecalSystem::Update(float dt)
{
	using namespace fq::game_module;
	auto scene = mGameProcess->mSceneManager->GetCurrentScene();

	scene->ViewComponents<Transform, Decal>
		([dt](GameObject& object, Transform& transform, Decal& decal)
			{
				graphics::IDecalObject* decalObjectInterface = decal.GetDecalObjectInterface();

				if (decalObjectInterface != nullptr)
				{
					const auto& decalInfo = decal.GetDecalInfo();

					DirectX::SimpleMath::Matrix objectTransform;

					if (!decalInfo.bIsIgnoreParentRotation)
					{
						objectTransform = transform.GetWorldMatrix();
					}
					else
					{
						objectTransform =
							DirectX::SimpleMath::Matrix::CreateScale(transform.GetWorldScale())
							* DirectX::SimpleMath::Matrix::CreateFromQuaternion(transform.GetLocalRotation())
							* DirectX::SimpleMath::Matrix::CreateTranslation(transform.GetWorldPosition());
					}

					auto decalUVAnimator = object.GetComponent<DecalUVAnimator>();

					if (decalUVAnimator != nullptr)
					{
						const auto additiveTransform = decalUVAnimator->GetAdditiveTransform();
						objectTransform = additiveTransform * objectTransform;
					}

					decalObjectInterface->SetTransform(objectTransform);
				}
			});
}

void fq::game_engine::DecalSystem::OnUnLoadScene()
{
	mbIsGameLoaded = false;
}

void fq::game_engine::DecalSystem::OnAddGameObject(const fq::event::AddGameObject& event)
{
	if (!mbIsGameLoaded) return;

	loadDecal(event.object);
}

void fq::game_engine::DecalSystem::OnDestroyedGameObject(const fq::event::OnDestoryedGameObject& event)
{
	unloadDecal(event.object);
}

void fq::game_engine::DecalSystem::AddComponent(const fq::event::AddComponent& event)
{
	if (event.id == entt::resolve<fq::game_module::Decal>().id())
	{
		loadDecal(event.component->GetGameObject());
	}
}

void fq::game_engine::DecalSystem::RemoveComponent(const fq::event::RemoveComponent& event)
{
	if (event.id == entt::resolve<fq::game_module::Decal>().id())
	{
		unloadDecal(event.component->GetGameObject());
	}
}

void fq::game_engine::DecalSystem::OnLoadScene()
{
	auto scene = mGameProcess->mSceneManager->GetCurrentScene();

	for (auto& object : scene->GetObjectView(true))
	{
		loadDecal(&object);
	}

	mbIsGameLoaded = true;
}

void fq::game_engine::DecalSystem::loadDecal(fq::game_module::GameObject* object)
{
	if (!object->HasComponent<fq::game_module::Decal>())
	{
		return;
	}

	auto decal = object->GetComponent<fq::game_module::Decal>();
	auto decalInfo = decal->GetDecalInfo();
	fq::graphics::DecalMaterialInfo materialInfo = decal->GetDecalMaterialInfo();
	auto material = mGameProcess->mGraphics->CreateDecalMaterial(materialInfo);

	DirectX::SimpleMath::Matrix additiveTransform;
	auto decalUVAnimator = object->GetComponent<fq::game_module::DecalUVAnimator>();

	if (decalUVAnimator != nullptr)
	{
		additiveTransform = decalUVAnimator->GetAdditiveTransform();
	}

	auto decalObjectInterface = mGameProcess->mGraphics->CreateDecalObject(material, decalInfo, additiveTransform * object->GetComponent<fq::game_module::Transform>()->GetWorldMatrix());
	decal->SetDecalObjectInterface(decalObjectInterface);
	decal->SetDecalMaterial(material);
	decal->SetLayer(decal->GetLayer());
}

void fq::game_engine::DecalSystem::unloadDecal(fq::game_module::GameObject* object)
{
	if (!object->HasComponent<fq::game_module::Decal>())
	{
		return;
	}

	auto decal = object->GetComponent<fq::game_module::Decal>();
	auto decalObjectInterface = decal->GetDecalObjectInterface();

	if (decalObjectInterface != nullptr)
		mGameProcess->mGraphics->DeleteDecalObject(decalObjectInterface);
}

