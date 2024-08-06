#include "ExportWindow.h"

#include "GameProcess.h"
#include "../FQGameModule/SceneManager.h"
#include "../FQGameModule/Scene.h"
#include "../FQGameModule/Transform.h"
#include "../FQGameModule/StaticMeshRenderer.h"
#include "../FQGameModule/Light.h"

fq::game_engine::ExportWindow::ExportWindow()
	:mGameProcess(nullptr),
	mEditorProcess(nullptr)
{
}

fq::game_engine::ExportWindow::~ExportWindow()
{

}

void fq::game_engine::ExportWindow::Initialize(GameProcess* game, EditorProcess* editor)
{
	mGameProcess = game;
	mEditorProcess = editor;
}

void fq::game_engine::ExportWindow::Render()
{
	if (!mbIsOpen) return;

	if (ImGui::Begin("ExportWindow", &mbIsOpen))
	{
		if (ImGui::Button("Export", ImVec2{ 133,25 }))
		{
			SaveGameObjectsForUnity();
		}
	}
	ImGui::End();
}

void fq::game_engine::ExportWindow::SaveGameObjectsForUnity()
{
	// ���� ������ ���� ����� ��δ� resource/Export
	mDirPath = "./resource/Export/";
	if (!std::filesystem::exists(mDirPath))
	{
		std::filesystem::create_directory(mDirPath);
	}

	mOutStream = std::ofstream(mDirPath.string() + "EngineData" + ".edt"); // engineData

	if (mOutStream.is_open())
	{
		// ���� ������Ʈ ��ȸ�ϱ�
		auto view = mGameProcess->mSceneManager->GetCurrentScene()->GetObjectView();
		for (auto& object : view)
		{
			// root Object�� ��� 
			if (!object.HasParent())
			{
				PrintGameObject(&object);
			}
		}

		mOutStream.close();
	}
}

void fq::game_engine::ExportWindow::PrintGameObject(fq::game_module::GameObject* object)
{
	// �� �̸� ����
	mOutStream << object->GetName() << "\n";

	// �θ� �ִ��� ���� ����
	fq::game_module::GameObject* parent = object->GetParent();
	if (parent != nullptr)
	{
		mOutStream << "true" << "\n";
		// �θ� �ִٸ� �θ� �̸� ����
		mOutStream << parent->GetName() << "\n";
	}
	else
	{
		mOutStream << "false" << "\n";
	}

	// Transform ���� 
	fq::game_module::Transform* transform = object->GetComponent<fq::game_module::Transform>();
	{
		// Position
		DirectX::SimpleMath::Vector3 position = transform->GetLocalPosition();
		mOutStream << position.x << " " << position.y << " " << position.z << "\n";
		// Rotation
		DirectX::SimpleMath::Quaternion rotation = transform->GetLocalRotation();
		mOutStream << rotation.x << " " << rotation.y << " " << rotation.z << " " << rotation.w << "\n";
		// Scale
		DirectX::SimpleMath::Vector3 scale = transform->GetLocalScale();
		mOutStream << scale.x << " " << scale.y << " " << scale.z << "\n";
	}

	// �ڽ��� staticmesh�� �ֵ��� fbx �ε� �ؾ��ϴϱ� �ʿ�
	fq::game_module::StaticMeshRenderer* staticMeshRenderer;

	std::vector<fq::game_module::GameObject*> children = object->GetChildren();
	std::string modelPath = "";
	for (int i = 0; i < children.size(); i++)
	{
		staticMeshRenderer = children[i]->GetComponent<fq::game_module::StaticMeshRenderer>();
		if (staticMeshRenderer != nullptr)
		{
			modelPath = staticMeshRenderer->GetModelPath();
			break;
		}
	}

	// ���� �ִٸ� Mesh Name ���� 
	staticMeshRenderer = object->GetComponent<fq::game_module::StaticMeshRenderer>();
	if (staticMeshRenderer != nullptr)
	{
		mOutStream << "meshNameTrue" << "\n";
		mOutStream << staticMeshRenderer->GetMeshName() << "\n";
	}
	else
	{
		mOutStream << "meshNameFalse" << "\n";
	}

	if (modelPath != "")
	{
		mOutStream << "Statictrue" << "\n";
		// static Mesh Renderer ������ fbx ��� ����
		// Resource ���� �����ϰ� �߶�� ��

		if (modelPath != "")
		{
			std::string keyword = "resource";

			size_t pos = modelPath.find(keyword);

			mOutStream << modelPath.substr(pos) << "\n";
		}
		else
		{
			mOutStream << "NULL" << "\n";
		}
	}
	else
	{
		mOutStream << "Staticfalse" << "\n";
	}

	// Light ������ Light ����
	fq::game_module::Light* light = object->GetComponent<fq::game_module::Light>();
	if (light != nullptr)
	{
		mOutStream << "Lighttrue" << "\n";
		if (light->GetLightType() == fq::graphics::ELightType::Directional)
		{
			mOutStream << "Directional" << "\n";
		}
		else if (light->GetLightType() == fq::graphics::ELightType::Point)
		{
			mOutStream << "Point" << "\n";
		}
		else if (light->GetLightType() == fq::graphics::ELightType::Spot)
		{
			mOutStream << "Spot" << "\n";
		}

		DirectX::SimpleMath::Color color = light->GetLightColor();
		mOutStream << color.x << " " << color.y << " " << color.z << "\n";

		mOutStream << light->GetIntensity() << "\n";

		mOutStream << light->GetRange() << "\n";

		mOutStream << light->GetSpot() << "\n";
	}
	else
	{
		mOutStream << "Lightfalse" << "\n";
	}

	mOutStream << "End" << "\n";

	// �ڽ� ��ȸ�ϱ� 
	//std::vector<fq::game_module::GameObject*> children = object->GetChildren();
	for (int i = 0; i < children.size(); i++)
	{
		PrintGameObject(children[i]);
	}
}

