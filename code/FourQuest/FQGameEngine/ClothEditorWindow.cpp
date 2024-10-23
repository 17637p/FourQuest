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
		, mbIsDeleteBrush(false)
		, mBrushRadian(0.1f)
		, mModelPath()
		, mFileName()
		, mClothDataLoader()
		, mClothData(nullptr)
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
		mClothData = std::make_shared<fq::physics::Cloth::ClothData>();
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
			beginButton_Save();
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
		ImGui::InputText("###", &mFileName);

		ImGui::SameLine();

		if (ImGui::Button("Save"))
		{
			std::string clothPath = mFileName + ".cloth";
			auto path = fq::path::GetResourcePath() / "Cloth" / clothPath;

			// �����ϴ� �ε��� �� ������Ʈ �� ����
			mClothData->disableIndices.clear();
			for (auto disableIndices : mObjectDisableIndices)
			{
				mClothData->disableIndices.push_back(disableIndices);
			}

			mClothDataLoader.Save(mClothData, path.c_str());
		}
	}

	void ClothEditorWindow::beginPrintText_GameObjectName()
	{
		if (!mGameObject) return;

		std::string text = mModelPath;
		ImGui::InputText("Model Path : ", &text);

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
				mClothData->vertices.resize(mesh.Vertices.size());
				mClothData->uvs.resize(mesh.Vertices.size());
				mClothData->indices.resize(mesh.Indices.size());
				triangleMeshInfo.vertexSize = mesh.Vertices.size();
				triangleMeshInfo.indexSize = mesh.Indices.size();

				for (int i = 0; i < mesh.Vertices.size(); ++i)
				{
					mClothData->vertices[i] = DirectX::SimpleMath::Vector3::Transform(mesh.Vertices[i].Pos, transform->GetWorldMatrix());
				}
				for (int i = 0; i < mesh.Vertices.size(); ++i)
				{
					mClothData->uvs[i] = mesh.Vertices[i].Tex;
				}
				for (int i = 0; i < mesh.Indices.size(); ++i)
				{
					mClothData->indices[i] = mesh.Indices[i];
				}

				triangleMeshInfo.vertices = mClothData->vertices.data();
				triangleMeshInfo.indices = mClothData->indices.data();
			}

			mGameProcess->mPhysics->CreateStaticBody(triangleMeshInfo, fq::physics::EColliderType::COLLISION);
		}
	}

	void ClothEditorWindow::debugDrawTriangle()
	{
		if (mGameObject == nullptr)
			return;

		fq::graphics::debug::SphereInfo info;

		// ������ ���ؽ��� ����������, ������ ���ؽ����� ��������� ����� ������
		for (int i = 0; i < mClothData->vertices.size(); i++)
		{
			DirectX::SimpleMath::Vector3 vertex = mClothData->vertices[i];

			info.Sphere.Center = vertex;
			info.Sphere.Radius = 0.005f;
			info.bUseDepthTest = true;
			
			if (mObjectDisableIndices.find(i) != mObjectDisableIndices.end())
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

			fq::physics::RayCastOutput raycastOutput = mGameProcess->mPhysics->RayCast(info, true, true);

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
					float brushRadiusSquared = mBrushRadian * mBrushRadian;

					if (distanceSquared <= brushRadiusSquared)
					{
						if (mbIsDeleteBrush)
						{
							auto it = mObjectDisableIndices.find(j);
							if (it != mObjectDisableIndices.end())
							{
								mObjectDisableIndices.erase(it);
							}
						}
						else
						{
							mObjectDisableIndices.insert(j);
						}
					}
				}
			}
		}
	}
}
