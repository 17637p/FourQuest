#include "ClothEditorWindow.h"

#include "..\FQGameModule\StaticMeshRenderer.h"
#include "..\FQGameModule\SkinnedMeshRenderer.h"
#include "..\FQGameModule\TriangleCollider.h"
#include "..\FQGameModule\Transform.h"
#include "..\FQGameModule\Camera.h"
#include "..\FQGraphics\IFQGraphics.h"
#include "..\FQphysics\IFQPhysics.h"
#include "..\FQGameModule\Scene.h"

#include "ModelSystem.h"

#include "GameProcess.h"
#include "EditorProcess.h"
#include "RenderingSystem.h"
#include "CameraSystem.h"
#include "ResourceSystem.h"

namespace fq::game_engine
{
	ClothEditorWindow::ClothEditorWindow()
		: mGameProcess()
		, mEditorProcess()
		, mEventManager()
		, mScene()
		, mGameObject()
		, mbIsOpen(false)
		, mBrushRadian(1.f)
	{
	}

	ClothEditorWindow::~ClothEditorWindow()
	{
	}

	void ClothEditorWindow::Initialize(GameProcess* game, EditorProcess* editor)
	{
		mGameProcess = game;
		mEditorProcess = editor;

		mScene = mGameProcess->mSceneManager->GetCurrentScene();
	}

	void ClothEditorWindow::Finalize()
	{
	}

	void ClothEditorWindow::Render()
	{
		if (!mbIsOpen) return;

		if (ImGui::Begin("Cloth Editor", &mbIsOpen))
		{
			ImGui::BeginChild("Cloth Editor");



			ImGui::EndChild();
		
			createBrush();
			dragDropGameObject();
			debugDrawTriangle();
		}
		ImGui::End();
	}

	void ClothEditorWindow::dragDropGameObject()
	{
		// Drop 처리
		if (ImGui::BeginDragDropTarget())
		{
			const ImGuiPayload* PathPayLoad = ImGui::AcceptDragDropPayload("GameObject");
			if (PathPayLoad)
			{
				mGameObject = nullptr;

				auto object = static_cast<fq::game_module::GameObject*>(PathPayLoad->Data);
				mGameObject = object->shared_from_this();

				// 해당 오브젝트의 콜라이더 생성
				createTriangle();
			}
			ImGui::EndDragDropTarget();
		}
	}

	void ClothEditorWindow::createTriangle()
	{
		bool hasStaticMesh = mGameObject->HasComponent<fq::game_module::StaticMeshRenderer>();
		bool hasSkinnedMesh = mGameObject->HasComponent<fq::game_module::SkinnedMeshRenderer>();

		// 5. Mesh Collider
		if ((hasStaticMesh || hasSkinnedMesh))
		{
			auto transform = mGameObject->GetComponent<fq::game_module::Transform>();
			fq::physics::TriangleMeshColliderInfo triangleMeshInfo;
			std::vector<DirectX::SimpleMath::Vector3> vertices;
			std::vector<unsigned int> indices;

			unsigned int id = mGameObject->GetID();
			triangleMeshInfo.colliderInfo.id = id;
			triangleMeshInfo.colliderInfo.layerNumber = static_cast<int>(mGameObject->GetTag());
			triangleMeshInfo.colliderInfo.collisionTransform = transform->GetTransform();

			if (hasStaticMesh)
			{
				auto staticMeshRenderer = mGameObject->GetComponent<fq::game_module::StaticMeshRenderer>();
				auto meshName = staticMeshRenderer->GetMeshName();
				auto modelPath = staticMeshRenderer->GetModelPath();

				bool check = mGameProcess->mResourceSystem->HasModel(modelPath);
				assert(check);

				const auto& model = mGameProcess->mResourceSystem->GetModel(modelPath);
				const auto& mesh = ModelSystem::GetMesh(model, meshName);

				vertices.resize(mesh.Vertices.size());
				triangleMeshInfo.vertexSize = mesh.Vertices.size();
				indices.resize(mesh.Indices.size());
				triangleMeshInfo.indexSize = mesh.Indices.size();

				for (int i = 0; i < mesh.Vertices.size(); ++i)
				{
					vertices[i] = mesh.Vertices[i].Pos;
				}
				for (int i = 0; i < mesh.Indices.size(); ++i)
				{
					indices[i] = mesh.Indices[i];
				}

				triangleMeshInfo.vertices = vertices.data();
				triangleMeshInfo.indices = indices.data();
			}

			mGameProcess->mPhysics->CreateDynamicBody(triangleMeshInfo, fq::physics::EColliderType::COLLISION, true);
		}
	}

	void ClothEditorWindow::debugDrawTriangle()
	{
		if (mGameObject == nullptr)
			return;

		auto verticesMap = mGameProcess->mPhysics->GetDebugTriangleVertices();
		mObjectModelVertices = verticesMap[mGameObject->GetID()];

		fq::graphics::debug::SphereInfo info;

		for (int i = 0; i < mObjectModelVertices.size(); i++)
		{
			info.Sphere.Center = DirectX::SimpleMath::Vector3::Transform(mObjectModelVertices[i], mGameObject->GetTransform()->GetWorldMatrix());
			info.Sphere.Radius = 0.01f;
			info.bUseDepthTest = false;
			
			if (mObjectDisableIndiecs.find(i) != mObjectDisableIndiecs.end())
			{
				info.Color = DirectX::SimpleMath::Color(1.f, 0.f, 0.f, 1.f);
			}
			else
			{
				info.Color = DirectX::SimpleMath::Color(1.f, 1.f, 0.f, 1.f);
			}

			mGameProcess->mGraphics->DrawSphere(info);
		}
	}

	void ClothEditorWindow::createBrush()
	{
		if (!(mGameProcess->mInputManager->GetKeyState(EKey::LMouse) == EKeyState::Hold)) return;

		// 월드 상의 마우스 포인트 좌표를 구해서 레이(Ray) 구하기
		fq::game_module::Camera* editorCamera = mGameProcess->mCameraSystem->GetEditorCamera();

		editorCamera->GetViewProjection();

		float mousePosX = mGameProcess->mInputManager->GetMousePosition().x;
		float mousePosY = mGameProcess->mInputManager->GetMousePosition().y;

		float mouseX = (2.f * mousePosX) / mGameProcess->mScreenManager->GetScreenWidth();
		float mouseY = (2.f * mousePosY) / mGameProcess->mScreenManager->GetScreenHeight();
		float mouseZ = 1.f; // For the far plane
		DirectX::SimpleMath::Vector3 rayOrigin = DirectX::XMVectorSet(mouseX, mouseY, 0.f, 1.f);
		DirectX::SimpleMath::Vector3 rayDirection = DirectX::XMVectorSet(mouseX, mouseY, mouseZ, 0.f);

		DirectX::SimpleMath::Matrix viewProjection = editorCamera->GetViewProjection();

		DirectX::SimpleMath::Vector3 rayOriginWS = DirectX::SimpleMath::Vector3::Transform(rayOrigin, viewProjection);
		DirectX::SimpleMath::Vector3 rayDirectionWS = DirectX::SimpleMath::Vector3::Transform(rayDirection, viewProjection);
		rayDirectionWS.Normalize();

		// 물리 엔진에 RayCast 실행하기
		fq::physics::RayCastInput info;
		info.origin = rayOriginWS;
		info.direction = rayDirectionWS;
		info.distance = 100.f;
		info.layerNumber = 0;

		fq::physics::RayCastOutput raycastOutput = mGameProcess->mPhysics->RayCast(info);

		DirectX::SimpleMath::Vector3 radiusPos = raycastOutput.blockPosition;

		// RayCast에 부딪힌 지점에 원(Circle)을 생성하여 해당 원 안에 포함되는 vertex들 모두 찾아서 브러쉬 타입에 따라
		// 비활성화 버텍스, 활성화 버텍스 지정하기
		for (int i = 0; i < mObjectModelVertices.size(); i++)
		{
			float x = mObjectModelVertices[i].x - radiusPos.x;
			float y = mObjectModelVertices[i].y - radiusPos.y;
			float z = mObjectModelVertices[i].z - radiusPos.z;

			float total = std::abs(x) + std::abs(y) + std::abs(z);

			if (total <= mBrushRadian)
			{
				if (mbIsDeleteBrush)
				{
					if (mObjectDisableIndiecs.find(i) != mObjectDisableIndiecs.end())
					{
						mObjectDisableIndiecs.erase(mObjectDisableIndiecs.find(i));
					}
				}
				else
				{
					mObjectDisableIndiecs.insert(i);
				}
			}
		}
	}
}
