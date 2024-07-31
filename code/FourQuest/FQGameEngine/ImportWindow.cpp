#include "ImportWindow.h"

#include <algorithm>
#include <imgui.h>
#include "imgui_stdlib.h"

#include "../FQGameModule/SceneManager.h"
#include "../FQGameModule/Scene.h"
#include "../FQGameModule/Transform.h"
#include "../FQGameModule/StaticMeshRenderer.h"
#include "../FQGameModule/Light.h"
#include "../FQGameModule/GameObject.h"
#include "../FQGraphics/IFQGraphics.h"
#include "../FQCommon/FQPath.h"
#include "../FQCommon/StringUtil.h"
#include "GameProcess.h"
#include "EditorProcess.h"
#include "ModelSystem.h"
#include "RenderingSystem.h"

fq::game_engine::ImportWindow::ImportWindow()
	:mGameProcess(nullptr),
	mEditorProcess(nullptr)
{
}

fq::game_engine::ImportWindow::~ImportWindow()
{
}

void fq::game_engine::ImportWindow::Initialize(GameProcess* game, EditorProcess* editor)
{
	mGameProcess = game;
	mEditorProcess = editor;
	mScene = mGameProcess->mSceneManager->GetCurrentScene();
}

void fq::game_engine::ImportWindow::Render()
{
	if (!mbIsOpen) return;

	// 경로 받기
	if (ImGui::Begin("ImportWindow", &mbIsOpen))
	{
		std::string importFileName = mImportFileName.string();
		ImGui::InputText("ImportFileName", &importFileName);

		if (ImGui::BeginDragDropTarget())
		{
			const ImGuiPayload* pathPayLoad = ImGui::AcceptDragDropPayload("Path");

			if (pathPayLoad)
			{
				std::filesystem::path* path
					= static_cast<std::filesystem::path*>(pathPayLoad->Data);

				if (std::filesystem::exists(*path) && path->extension() == ".json")
				{
					mImportFileName = *path;
				}
			}
		}

		std::string fbxDirectory = mFBXDirectory.string();
		ImGui::InputText("FBXDirectoryPath", &fbxDirectory);

		if (ImGui::BeginDragDropTarget())
		{
			const ImGuiPayload* pathPayLoad = ImGui::AcceptDragDropPayload("Path");

			if (pathPayLoad)
			{
				std::filesystem::path* path
					= static_cast<std::filesystem::path*>(pathPayLoad->Data);

				if (std::filesystem::exists(*path) && std::filesystem::is_directory(*path))
				{
					mFBXDirectory = *path;
				}
			}
		}

		std::string textureDirectory = mTextureDirectory.string();
		ImGui::InputText("TextureDirectoryPath", &textureDirectory);

		if (ImGui::BeginDragDropTarget())
		{
			const ImGuiPayload* pathPayLoad = ImGui::AcceptDragDropPayload("Path");

			if (pathPayLoad)
			{
				std::filesystem::path* path
					= static_cast<std::filesystem::path*>(pathPayLoad->Data);

				if (std::filesystem::exists(*path) && std::filesystem::is_directory(*path))
				{
					mTextureDirectory = *path;
				}
			}
		}

		std::string materialDirectory = mMaterialDirectory.string();
		ImGui::InputText("MaterialDirectoryPath", &materialDirectory);

		if (ImGui::BeginDragDropTarget())
		{
			const ImGuiPayload* pathPayLoad = ImGui::AcceptDragDropPayload("Path");

			if (pathPayLoad)
			{
				std::filesystem::path* path
					= static_cast<std::filesystem::path*>(pathPayLoad->Data);

				if (std::filesystem::exists(*path) && std::filesystem::is_directory(*path))
				{
					mMaterialDirectory = *path;
				}
			}
		}

		if (ImGui::Button("Import", ImVec2{ 133,25 }))
		{
			loadGameObjectsForUnityByDirectory();
		}
	}

	ImGui::End();
}

void fq::game_engine::ImportWindow::loadGameObjectsForUnityByDirectory()
{
	if (!std::filesystem::exists(mImportFileName) || mImportFileName.extension() != ".json")
	{
		return;
	}
	if (!std::filesystem::exists(mFBXDirectory) && !std::filesystem::is_directory(mFBXDirectory))
	{
		return;
	}
	if (!std::filesystem::exists(mTextureDirectory) && !std::filesystem::is_directory(mTextureDirectory))
	{
		return;
	}
	if (!std::filesystem::exists(mMaterialDirectory) && !std::filesystem::is_directory(mMaterialDirectory))
	{
		return;
	}

	std::vector<std::shared_ptr<fq::game_module::GameObject>> rootObjects;
	std::map<int, std::shared_ptr<fq::game_module::GameObject>> gameObjectsMap;

	std::vector<importData::GameObjectLoadInfo> gameObjectInfos = loadGameObjectInfosByJson(mImportFileName);
	
	std::shared_ptr<fq::game_module::GameObject> sceneRootObject = std::make_shared<fq::game_module::GameObject>();
	sceneRootObject->SetName("staticSceneRoot");

	for (const auto& gameObjectInfo : gameObjectInfos)
	{
		std::shared_ptr<fq::game_module::GameObject> gameObject = std::make_shared<fq::game_module::GameObject>();

		// 오브젝트 생성
		{
			gameObject->SetName(gameObjectInfo.Name);

			// 트랜스폼
			auto transform = gameObject->GetComponent<game_module::Transform>();
			transform->SetWorldPosition(gameObjectInfo.TransformData.Position);
			transform->SetLocalRotation(gameObjectInfo.TransformData.Rotation);
			transform->SetLocalScale(gameObjectInfo.TransformData.Scale);

			std::vector<fq::graphics::MaterialInfo> materialInfos;
			materialInfos.reserve(gameObjectInfo.MeshData.Materials.size());
			std::vector<std::string> materialPaths;
			materialPaths.reserve(gameObjectInfo.MeshData.Materials.size());

			for (const auto& material : gameObjectInfo.MeshData.Materials)
			{
				fq::graphics::MaterialInfo materialInfo;

				materialInfo.BaseColor = material.Albedo;
				materialInfo.Metalness = material.Metallic;
				materialInfo.Roughness = 1 - material.Smoothness;
				materialInfo.EmissiveColor = material.Emission;
				materialInfo.Tiling = material.Tiling;
				materialInfo.Offset = material.Offset;

				if (!material.AlbedoMap.empty())
				{
					materialInfo.BaseColorFileName = mTextureDirectory / fq::common::StringUtil::ToWide(material.AlbedoMap);
				}
				if (!material.MetallicAndSmoothnessMap.empty())
				{
				}
				if (!material.NormalMap.empty())
				{
					materialInfo.NormalFileName = mTextureDirectory / fq::common::StringUtil::ToWide(material.NormalMap);
				}
				if (!material.EmissionMap.empty())
				{
					materialInfo.EmissiveFileName = mTextureDirectory / fq::common::StringUtil::ToWide(material.EmissionMap);
				}

				std::string materialPath = (mMaterialDirectory / material.Name).string() + ".material";
				materialPaths.push_back(materialPath);

				mGameProcess->mGraphics->WriteMaterialInfo(materialPath, materialInfo);
			}

			const auto& modelName = gameObjectInfo.MeshData.ModelPath;
			const auto& meshName = gameObjectInfo.MeshData.Name;

			if (!modelName.empty() || !meshName.empty())
			{
				std::filesystem::path modelPath = (mFBXDirectory / modelName);

				if (modelPath.extension() == ".fbx")
				{
					fq::common::Model modelData = mGameProcess->mGraphics->ConvertModel(modelPath.string());
					modelPath.replace_extension(".model");
					mGameProcess->mGraphics->WriteModel(modelPath.string(), modelData);
				}

				auto key = mGameProcess->mRenderingSystem->GetModelKey(modelPath.string(), mTextureDirectory.string());
				mGameProcess->mGraphics->CreateModelResource(key, modelPath.string(), mTextureDirectory.string());

				auto& staticMeshRenderer = gameObject->AddComponent<fq::game_module::StaticMeshRenderer>();
				staticMeshRenderer.SetModelPath(modelPath.string());
				staticMeshRenderer.SetMaterialPaths(materialPaths);
				staticMeshRenderer.SetMeshName(meshName);
				staticMeshRenderer.SetTexturePath(mTextureDirectory.string());
			}

			// 라이트

			gameObjectsMap.insert({ gameObjectInfo.ID, gameObject });
		}

		// 부모 자식 연결
		auto find = gameObjectsMap.find(gameObjectInfo.ParentID);

		if (find != gameObjectsMap.end())
		{
			auto parentTransform = find->second->GetComponent<game_module::Transform>();
			auto childTransform = gameObject->GetComponent<game_module::Transform>();

			parentTransform->AddChild(childTransform);
		}

		// 루트 오브젝트 체크
		if (gameObjectInfo.ParentID == 0)
		{
			auto transform = gameObject->GetComponent<game_module::Transform>();
			transform->SetLocalMatrix(DirectX::SimpleMath::Matrix::CreateScale(0.01) * DirectX::SimpleMath::Matrix::CreateRotationY(3.14) * transform->GetLocalMatrix());

			auto sceneRootTransform = sceneRootObject->GetComponent<game_module::Transform>();
			sceneRootTransform->AddChild(transform);
		}
	}

	mEditorProcess->mCommandSystem->Push<AddObjectCommand>(mScene, sceneRootObject);
}

std::vector<fq::game_engine::importData::GameObjectLoadInfo> fq::game_engine::ImportWindow::loadGameObjectInfosByJson(const std::filesystem::path& filePath)
{
	assert(std::filesystem::exists(filePath));

	std::ifstream readData(filePath);
	nlohmann::ordered_json gameObjectsJson;

	if (readData.is_open())
	{
		readData >> gameObjectsJson;
		readData.close();
	}
	else
		assert(!"파일 열기 실패");

	using namespace importData;

	auto getFileName = [](const std::string filePath) -> std::string
		{
			if (filePath.empty())
			{
				return "";
			}

			std::filesystem::path path{ filePath };

			return path.filename().string();
		};

	std::vector<GameObjectLoadInfo> objectInfos;

	for (const auto& gameObjectJson : gameObjectsJson["GameObjects"])
	{
		GameObjectLoadInfo info;

		info.Name = gameObjectJson["Name"].get<std::string>();
		info.ID = gameObjectJson["ID"].get<int>();
		info.ParentID = gameObjectJson["ParentID"].get<int>();

		// Transform 파싱
		info.TransformData.Position = {
			gameObjectJson["Transform"]["Position"]["x"].get<float>(),
			gameObjectJson["Transform"]["Position"]["y"].get<float>(),
			gameObjectJson["Transform"]["Position"]["z"].get<float>()
		};
		info.TransformData.Rotation = {
			gameObjectJson["Transform"]["Rotation"]["x"].get<float>(),
			gameObjectJson["Transform"]["Rotation"]["y"].get<float>(),
			gameObjectJson["Transform"]["Rotation"]["z"].get<float>(),
			gameObjectJson["Transform"]["Rotation"]["w"].get<float>()
		};
		info.TransformData.Scale = {
			gameObjectJson["Transform"]["Scale"]["x"].get<float>(),
			gameObjectJson["Transform"]["Scale"]["y"].get<float>(),
			gameObjectJson["Transform"]["Scale"]["z"].get<float>()
		};

		// Mesh 파싱
		info.MeshData.ModelPath = getFileName(gameObjectJson["Mesh"]["ModelPath"].get<std::string>());
		info.MeshData.Name = gameObjectJson["Mesh"]["Name"].get<std::string>();
		for (const auto& materialJson : gameObjectJson["Mesh"]["Materials"]) {
			Material material;
			material.Name = materialJson["Name"].get<std::string>();
			material.renderMode = materialJson["renderMode"].get<std::string>();
			material.Albedo = {
				materialJson["Albedo"]["x"].get<float>(),
				materialJson["Albedo"]["y"].get<float>(),
				materialJson["Albedo"]["z"].get<float>(),
				materialJson["Albedo"]["w"].get<float>()
			};
			material.Metallic = materialJson["Metallic"].get<float>();
			material.Smoothness = materialJson["Smoothness"].get<float>();
			material.Emission = {
				materialJson["Emission"]["x"].get<float>(),
				materialJson["Emission"]["y"].get<float>(),
				materialJson["Emission"]["z"].get<float>(),
				materialJson["Emission"]["w"].get<float>()
			};
			material.AlbedoMap = getFileName(materialJson["AlbedoMap"].get<std::string>());
			material.MetallicAndSmoothnessMap = getFileName(materialJson["MetallicAndSmoothnessMap"].get<std::string>());
			material.NormalMap = getFileName(materialJson["NormalMap"].get<std::string>());
			material.EmissionMap = getFileName(materialJson["EmissionMap"].get<std::string>());
			material.Tiling = {
				materialJson["Tiling"]["x"].get<float>(),
				materialJson["Tiling"]["y"].get<float>()
			};
			material.Offset = {
				materialJson["Offset"]["x"].get<float>(),
				materialJson["Offset"]["y"].get<float>()
			};
			info.MeshData.Materials.push_back(material);
		}

		// Light 파싱
		info.LightData.Type = gameObjectJson["Light"]["Type"].get<std::string>();
		info.LightData.Color = {
			gameObjectJson["Light"]["Color"]["x"].get<float>(),
			gameObjectJson["Light"]["Color"]["y"].get<float>(),
			gameObjectJson["Light"]["Color"]["z"].get<float>(),
			gameObjectJson["Light"]["Color"]["w"].get<float>()
		};

		objectInfos.push_back(info);
	}

	return objectInfos;
}

