#include "ClothEditorWindow.h"

#include "..\FQGameModule\StaticMeshRenderer.h"
#include "..\FQGameModule\SkinnedMeshRenderer.h"
#include "..\FQGameModule\TriangleCollider.h"
#include "..\FQGameModule\Transform.h"
#include "..\FQGraphics\IFQGraphics.h"
#include "..\FQphysics\IFQPhysics.h"

#include "ModelSystem.h"

#include "GameProcess.h"
#include "EditorProcess.h"
#include "RenderingSystem.h"

namespace fq::game_engine
{
	ClothEditorWindow::ClothEditorWindow()
	{
	}

	ClothEditorWindow::~ClothEditorWindow()
	{
	}

	void ClothEditorWindow::Initialize(GameProcess* game, EditorProcess* editor)
	{
		mGameProcess = game;
		mEditorProcess = editor;
	}

	void ClothEditorWindow::Finalize()
	{
	}

	void ClothEditorWindow::Render()
	{
		if (!mbIsOpen) return;

		if (ImGui::Begin("Cloth Editor", &mbIsOpen))
		{
			ImGui::BeginChild("PhysicsAnimatorTool");
			ImGui::EndChild();
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
				auto key = mGameProcess->mRenderingSystem->GetModelKey(modelPath);

				const auto& model = mGameProcess->mGraphics->GetModel(key);
				const auto& mesh = fq::game_engine::ModelSystem::GetMesh(model, meshName);

				vertices.resize(mesh.Vertices.size());
				triangleMeshInfo.vertexSize = mesh.Vertices.size();
				indices.resize(mesh.Indices.size());
				triangleMeshInfo.indexSize = mesh.Indices.size();

				for (int i = 0; i < mesh.Vertices.size(); ++i)
				{
					vertices[i] = -mesh.Vertices[i].Pos;
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
		auto verticesContainer = verticesMap[mGameObject->GetID()];

		fq::graphics::debug::SphereInfo info;

		for (auto& vertex : verticesContainer)
		{
			info.Sphere.Center = DirectX::SimpleMath::Vector3::Transform(vertex, mGameObject->GetTransform()->GetWorldMatrix());
			info.Sphere.Radius = 0.01f;
			info.bUseDepthTest = false;
			info.Color = DirectX::SimpleMath::Color(1.f, 1.f, 0.f, 1.f);

			mGameProcess->mGraphics->DrawSphere(info);
		}
	}
}
