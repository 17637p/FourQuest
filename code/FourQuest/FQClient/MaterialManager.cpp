#include "MaterialManager.h"
#include "ClientEvent.h"
#include "GameManager.h"
#include "../FQGameModule/StaticMeshRenderer.h"
#include "../FQGameModule/Camera.h"
#include "../FQGameModule/Transform.h"
#include "../FQGameModule/CharacterController.h"
#include "../FQCommon/FQCommonGraphics.h"

fq::client::MaterialManager::MaterialManager(const MaterialManager& other)
	: mGameManager(other.mGameManager)
	, mStaticMeshObjects(other.mStaticMeshObjects)
	, mMaterialCache(other.mMaterialCache)
	, mManagerMaterialPath(other.mManagerMaterialPath)
	, mManagerMaterialOrNull(other.mManagerMaterialOrNull)
{
}

fq::client::MaterialManager& fq::client::MaterialManager::operator=(const MaterialManager& other)
{
	if (this == &other)
	{
		return *this;
	}

	mGameManager = other.mGameManager;
	mStaticMeshObjects = other.mStaticMeshObjects;
	mMaterialCache = other.mMaterialCache;
	mManagerMaterialPath = other.mManagerMaterialPath;
	mManagerMaterialOrNull = other.mManagerMaterialOrNull;

	return *this;
}

void fq::client::MaterialManager::SetManagerMaterialOrNull(std::shared_ptr<fq::graphics::IMaterial> managerMaterialOrNull)
{
	mManagerMaterialOrNull = managerMaterialOrNull;
}

std::shared_ptr<fq::game_module::Component> fq::client::MaterialManager::Clone(std::shared_ptr<fq::game_module::Component> clone) const
{
	auto cloned = std::dynamic_pointer_cast<MaterialManager>(clone);

	if (cloned == nullptr) // ���� �����ؼ� ���纻�� �ش�
	{
		cloned = game_module::ObjectPool::GetInstance()->Assign<MaterialManager>(*this);
	}
	else // clone�� �����͸� �����Ѵ�.
	{
		// �⺻ ���� ������ ȣ���Ѵ�.
		*cloned = *this;
	}

	return cloned;
}

void fq::client::MaterialManager::OnUpdate(float dt)
{
	std::vector<fq::game_module::GameObject*> players;

	GetScene()->ViewComponents<fq::game_module::CharacterController>(
		[&players](fq::game_module::GameObject& object, fq::game_module::CharacterController& camera)
		{
			players.push_back(&object);
		}
	);

	if (players.empty())
	{
		return;
	}
	if (mManagerMaterialOrNull == nullptr)
	{
		return;
	}

	fq::game_module::Camera* mainCamera = GetComponent<fq::game_module::Camera>();

	GetScene()->ViewComponents<fq::game_module::Camera>(
		[&mainCamera](fq::game_module::GameObject& object, fq::game_module::Camera& camera)
		{
			if (camera.IsMain())
			{
				mainCamera = &camera;
				return;
			}
		}
	);

	if (mbUseDebugDraw)
	{
		for (const auto& player : players)
		{
			DirectX::SimpleMath::Vector3 playerPosW = DirectX::SimpleMath::Vector3::TransformNormal(mLocalPointOffset, player->GetTransform()->GetLocalMatrix()) + player->GetTransform()->GetWorldPosition() + mWorldPointOffset;
			DirectX::SimpleMath::Vector3 cameraPosW = mainCamera->GetTransform()->GetWorldPosition();

			DirectX::SimpleMath::Vector3 lay = playerPosW - cameraPosW;
			
			fq::graphics::debug::RayInfo rayInfo;

			rayInfo.Origin = cameraPosW;
			rayInfo.Direction = lay;
			rayInfo.Color = { 1, 1, 0, 1 };
			rayInfo.Normalize = false;
			rayInfo.bUseDepthTest = false;

			GetScene()->GetEventManager()->FireEvent<fq::event::DrawDebugLay>({ rayInfo });
		}
	}

	GetScene()->ViewComponents<fq::game_module::StaticMeshRenderer>(
		[this, &mainCamera, &players](fq::game_module::GameObject& object, fq::game_module::StaticMeshRenderer& meshRenderer)
		{
			if (!meshRenderer.GetIsStatic())
			{
				return;
			}
			if (mainCamera == nullptr)
			{
				return;
			}

			const DirectX::BoundingBox& boundingBox = meshRenderer.GetStaticMeshObject()->GetStaticMesh()->GetMeshData().RenderBoundingBox;

			for (const auto& player : players)
			{
				const auto& objectInverseTransform = object.GetTransform()->GetWorldMatrix().Invert();

				DirectX::SimpleMath::Vector3 playerPosW = DirectX::SimpleMath::Vector3::TransformNormal(mLocalPointOffset, player->GetTransform()->GetLocalMatrix()) + player->GetTransform()->GetWorldPosition() + mWorldPointOffset;
				DirectX::SimpleMath::Vector3 cameraPosW = mainCamera->GetTransform()->GetWorldPosition();

				DirectX::SimpleMath::Vector3 playerPosL = DirectX::SimpleMath::Vector3::Transform(playerPosW, objectInverseTransform);
				DirectX::SimpleMath::Vector3 cameraPosL = DirectX::SimpleMath::Vector3::Transform(cameraPosW, objectInverseTransform);

				DirectX::SimpleMath::Vector3 lay = cameraPosL - playerPosL;

				float distance = lay.Length();
				lay.Normalize();

				if (boundingBox.Intersects(playerPosL, lay, distance))
				{
					std::vector<std::shared_ptr<fq::graphics::IMaterial>> materials = meshRenderer.GetMaterialInterfaces();
					mMaterialCache.insert({ &object, materials });

					std::vector< std::shared_ptr<fq::graphics::IMaterial>> managerMaterials(materials.size(), mManagerMaterialOrNull);
					meshRenderer.SetMaterialInterfaces(managerMaterials);

					break;
				}
				else
				{
					auto find = mMaterialCache.find(&object);

					if (find != mMaterialCache.end())
					{
						meshRenderer.SetMaterialInterfaces(find->second);
						mMaterialCache.erase(find);
					}
				}
			}
		}
	);
}

void fq::client::MaterialManager::OnStart()
{
	mGameManager = GetScene()->GetObjectByName("GameManager")->GetComponent<GameManager>();

	fq::event::LoadMaterial loadMaterial;
	loadMaterial.materialPath = mManagerMaterialPath;
	loadMaterial.materialPtr = &mManagerMaterialOrNull;

	GetScene()->GetEventManager()->FireEvent<fq::event::LoadMaterial>(std::move(loadMaterial));
}
