#include "ClothEditorWindow.h"
#include "imgui_stdlib.h"

#include "..\FQCommon\FQPath.h"
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
		, mBrushRadian(0.1f)
		, mModelPath()
		, mFileName()
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

			beginPrintText_GameObjectName();
			ImGui::Separator();
			beginCheckBox_IsDeleteBrush();
			ImGui::Separator();
			beginInputFloat_BrushRadian();

			ImGui::EndChild();
		
			createBrush();
			dragDropGameObject();
			debugDrawTriangle();
		}
		ImGui::End();
	}

	void ClothEditorWindow::beginButton_Save()
	{
		if (ImGui::Button("Save Cloth Data"))
		{
			ImGui::InputText("###", &mFileName);

			ImGui::SameLine();

			if (ImGui::Button("Save"))
			{
				std::string articulationPath = mFileName + ".cloth";
				auto path = fq::path::GetResourcePath() / "Cloth" / articulationPath;

				// �����ϴ� �ε��� �� ������Ʈ �� ����
				mClothData->disableIndices.clear();
				for (auto disableIndices : mObjectDisableIndiecs)
				{
					mClothData->disableIndices.push_back(disableIndices);
				}

				mClothDataLoader.Save(mClothData, path.c_str());
			}
		}
	}

	void ClothEditorWindow::beginPrintText_GameObjectName()
	{
		if (!mGameObject) return;

		std::string text = "Model Path : " + mModelPath;
		ImGui::Text(text.c_str());

		text = "Object Name : " + mGameObject->GetName();
		ImGui::Text(text.c_str());
	}

	void ClothEditorWindow::beginCheckBox_IsDeleteBrush()
	{
		ImGui::Checkbox("Delete Brush", &mbIsDeleteBrush);
	}

	void ClothEditorWindow::beginInputFloat_BrushRadian()
	{
		ImGui::InputFloat("Brush Size", &mBrushRadian);
	}

	void ClothEditorWindow::dragDropGameObject()
	{
		// Drop ó��
		if (ImGui::BeginDragDropTarget())
		{
			const ImGuiPayload* PathPayLoad = ImGui::AcceptDragDropPayload("GameObject");
			if (PathPayLoad)
			{
				mGameObject = nullptr;

				auto object = static_cast<fq::game_module::GameObject*>(PathPayLoad->Data);
				mGameObject = object->shared_from_this();

				// �ش� ������Ʈ�� �ݶ��̴� ����
				createTriangle();
			}
			ImGui::EndDragDropTarget();
		}
	}

	void ClothEditorWindow::createTriangle()
	{
		bool hasStaticMesh = mGameObject->HasComponent<fq::game_module::StaticMeshRenderer>();
		bool hasSkinnedMesh = mGameObject->HasComponent<fq::game_module::SkinnedMeshRenderer>();

		// �ش� ������Ʈ�� ����ƽ �޽��� ���̷�Ż �޽��� ������ �ִ� ���
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
				// ���� ��� ã��
				auto staticMeshRenderer = mGameObject->GetComponent<fq::game_module::StaticMeshRenderer>();
				auto meshName = staticMeshRenderer->GetMeshName();
				mModelPath = staticMeshRenderer->GetModelPath();

				// �� ��� ������ ���� �޽� ã��
				bool check = mGameProcess->mResourceSystem->HasModel(mModelPath);
				assert(check);

				const auto& model = mGameProcess->mResourceSystem->GetModel(mModelPath);
				const auto& mesh = ModelSystem::GetMesh(model, meshName);

				// ���� ������ ������ �޽��� ���ؽ��� �ε��� ���� ����
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

				// ��� ������ ���ؽ� �� ����
				mClothData->vertices = vertices;
				mClothData->indices = indices;
			}

			mGameProcess->mPhysics->CreateDynamicBody(triangleMeshInfo, fq::physics::EColliderType::COLLISION, true);
		}
	}

	void ClothEditorWindow::debugDrawTriangle()
	{
		if (mGameObject == nullptr)
			return;


		fq::graphics::debug::SphereInfo info;

		for (int i = 0; i < mClothData->vertices.size(); i++)
		{
			info.Sphere.Center = DirectX::SimpleMath::Vector3::Transform(mClothData->vertices[i], mGameObject->GetTransform()->GetWorldMatrix());
			info.Sphere.Radius = 0.003f;
			info.bUseDepthTest = true;
			
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
		if (mGameProcess->mInputManager->GetKeyState(EKey::T) == EKeyState::Hold
			&& mGameProcess->mInputManager->GetKeyState(EKey::LMouse) == EKeyState::Hold)
		{
			// ���� ���� ���콺 ����Ʈ ��ǥ�� ���ؼ� ����(Ray) ���ϱ�
			fq::game_module::Camera* editorCamera = mGameProcess->mCameraSystem->GetEditorCamera();

			DirectX::SimpleMath::Matrix viewProjection = editorCamera->GetViewProjection();
			DirectX::SimpleMath::Matrix invViewProjection = viewProjection.Invert();

			float mousePosX = mGameProcess->mInputManager->GetMousePosition().x;
			float mousePosY = mGameProcess->mInputManager->GetMousePosition().y;

			float mouseX = (2.f * mousePosX) / mGameProcess->mScreenManager->GetFixScreenWidth() - 1.f;
			float mouseY = 1.f - (2.f * mousePosY) / mGameProcess->mScreenManager->GetFixScreenHeight();
			float mouseZ = 1.f; // For the far plane

			DirectX::SimpleMath::Vector3 rayOrigin(mouseX, mouseY, 0.f);
			DirectX::SimpleMath::Vector3 rayDirection(mouseX, mouseY, mouseZ);

			DirectX::SimpleMath::Vector3 rayOriginWS = DirectX::SimpleMath::Vector3::Transform(rayOrigin, invViewProjection);
			DirectX::SimpleMath::Vector3 rayDirectionWS = DirectX::SimpleMath::Vector3::Transform(rayDirection, invViewProjection);

			// Debug
			fq::graphics::debug::RayInfo rayInfo;
			rayInfo.bUseDepthTest = true;
			rayInfo.Color = DirectX::SimpleMath::Color{ 1.f, 0.f, 0.f, 1.f };
			rayInfo.Origin = rayOriginWS;
			rayInfo.Direction = rayDirectionWS;
			rayInfo.Normalize = false;
			mGameProcess->mGraphics->DrawRay(rayInfo);

			rayDirectionWS.Normalize();
			// ���� ������ RayCast �����ϱ�
			fq::physics::RayCastInput info;
			info.origin = rayOriginWS;
			info.direction = rayDirectionWS;
			info.distance = 100.f;
			info.layerNumber = 0;

			fq::physics::RayCastOutput raycastOutput = mGameProcess->mPhysics->RayCast(info, true);

			for (int i = 0; i < raycastOutput.hitSize; i++)
			{
				DirectX::SimpleMath::Vector3 radiusPos = raycastOutput.contectPoints[i];

				fq::graphics::debug::SphereInfo sphereInfo;
				sphereInfo.bUseDepthTest = false;
				sphereInfo.Color = DirectX::SimpleMath::Color{ 1.f, 0.f, 0.f, 1.f };
				sphereInfo.Sphere.Center = radiusPos;
				sphereInfo.Sphere.Radius = mBrushRadian;
				mGameProcess->mGraphics->DrawSphere(sphereInfo);

				// RayCast�� �ε��� ������ ��(Circle)�� �����Ͽ� �ش� �� �ȿ� ���ԵǴ� vertex�� ��� ã�Ƽ� �귯�� Ÿ�Կ� ����
				// ��Ȱ��ȭ ���ؽ�, Ȱ��ȭ ���ؽ� �����ϱ�
				for (int j = 0; j < mClothData->vertices.size(); j++)
				{
					float x = mClothData->vertices[j].x - radiusPos.x;
					float y = mClothData->vertices[j].y - radiusPos.y;
					float z = mClothData->vertices[j].z - radiusPos.z;

					float distanceSquared = (x * x) + (y * y) + (z * z);

					if (distanceSquared <= mBrushRadian * mBrushRadian)
					{
						if (mbIsDeleteBrush)
						{
							auto it = mObjectDisableIndiecs.find(j);
							if (it != mObjectDisableIndiecs.end())
							{
								mObjectDisableIndiecs.erase(it);
							}
						}
						else
						{
							mObjectDisableIndiecs.insert(j);
						}
					}
				}
			}
		}
	}
}
