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
	// 폴더 없으면 폴더 만들기 경로는 resource/Export
	mDirPath = "./resource/Export/";
	if (!std::filesystem::exists(mDirPath))
	{
		std::filesystem::create_directory(mDirPath);
	}

	mOutStream = std::ofstream(mDirPath.string() + "EngineData" + ".edt"); // engineData

	if (mOutStream.is_open())
	{
		// 게임 오브젝트 순회하기
		auto view = mGameProcess->mSceneManager->GetCurrentScene()->GetObjectView();
		for (auto& object : view)
		{
			// root Object만 골라서 
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
	// 내 이름 저장
	mOutStream << object->GetName() << "\n";

	// 부모가 있는지 여부 저장
	fq::game_module::GameObject* parent = object->GetParent();
	if (parent != nullptr)
	{
		mOutStream << "true" << "\n";
		// 부모가 있다면 부모 이름 저장
		mOutStream << parent->GetName() << "\n";
	}
	else
	{
		mOutStream << "false" << "\n";
	}

	// Transform 저장 
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

	// 자식이 staticmesh가 있따면 fbx 로드 해야하니까 필요
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

	// 내가 있다면 Mesh Name 저장 
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
		// static Mesh Renderer 있으면 fbx 경로 저장
		// Resource 부터 시작하게 잘라야 함

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

	// Light 있으면 Light 저장
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

	// 자식 순회하기 
	//std::vector<fq::game_module::GameObject*> children = object->GetChildren();
	for (int i = 0; i < children.size(); i++)
	{
		PrintGameObject(children[i]);
	}
}

