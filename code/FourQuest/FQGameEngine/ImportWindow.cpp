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
#include "../FQGameModule/Terrain.h"
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

	// ��� �ޱ�
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
			createGameObject();
		}
	}

	ImGui::End();
}

void fq::game_engine::ImportWindow::createGameObject()
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
	std::set<std::string> objectNames;

	std::vector<importData::GameObjectLoadInfo> gameObjectInfos = loadData(mImportFileName);

	std::shared_ptr<fq::game_module::GameObject> sceneRootObject = std::make_shared<fq::game_module::GameObject>();
	sceneRootObject->SetName("staticSceneRoot");

	for (const auto& gameObjectInfo : gameObjectInfos)
	{
		std::shared_ptr<fq::game_module::GameObject> gameObject = std::make_shared<fq::game_module::GameObject>();

		// ������Ʈ ����
		{
			auto find = objectNames.find(gameObjectInfo.Name);
			std::string newFileName = gameObjectInfo.Name;

			int index = 0;
			while (find != objectNames.end())
			{
				++index;
				newFileName = gameObjectInfo.Name;
				newFileName += std::to_string(index);

				find = objectNames.find(newFileName);
			}

			gameObject->SetName(newFileName);
			objectNames.insert(newFileName);

			// Ʈ������
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
					materialInfo.MetalnessSmoothnessFileName = mTextureDirectory / fq::common::StringUtil::ToWide(material.MetallicAndSmoothnessMap);
				}
				else
				{
					materialInfo.bIsUsedMetalnessSmoothness = false;
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

			if (!modelName.empty() && !meshName.empty())
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
				staticMeshRenderer.SetIsStatic(gameObjectInfo.isStatic);
				staticMeshRenderer.SetLightmapIndex(gameObjectInfo.MeshData.LightmapIndex);
				staticMeshRenderer.SetLightmapUVScaleOffset(gameObjectInfo.MeshData.LightmapScaleOffset);
				staticMeshRenderer.SetPrevApplyTransform(DirectX::SimpleMath::Matrix::CreateScale(0.01f) * DirectX::SimpleMath::Matrix::CreateRotationY(3.14f));
			}

			// ����Ʈ
			const auto& lightType = gameObjectInfo.LightData.Type;

			if (!lightType.empty())
			{
				auto& light = gameObject->AddComponent<fq::game_module::Light>();

				if (lightType == "Spot")
				{
					light.SetLightType(fq::graphics::ELightType::Spot);
				}
				else if (lightType == "Directional")
				{
					light.SetLightType(fq::graphics::ELightType::Directional);
				}
				else if (lightType == "Point")
				{
					light.SetLightType(fq::graphics::ELightType::Point);
				}

				light.SetIntensity(gameObjectInfo.LightData.Intensity);
				light.SetRange(gameObjectInfo.LightData.Range);
				light.SetLightColor(gameObjectInfo.LightData.Color);
				light.SetSpot(gameObjectInfo.LightData.SpotAngle); // ���缭 ó��

				if (lightType == "Realtime")
				{
					light.SetLightMode(fq::graphics::ELightMode::Realtime);
				}
				else if (lightType == "Mixed")
				{
					light.SetLightMode(fq::graphics::ELightMode::Mixed);
				}
				else if (lightType == "Baked")
				{
					light.SetLightMode(fq::graphics::ELightMode::Baked);
				}
			}

			// �ͷ���
			if (!gameObjectInfo.TerrainData.Layers.empty())
			{
				auto& terrain = gameObject->AddComponent<fq::game_module::Terrain>();

				std::vector<fq::graphics::TerrainLayer> terrainLayers;
				for (const auto& importLayerInfo : gameObjectInfo.TerrainData.Layers)
				{
					fq::graphics::TerrainLayer terrainLayer;
					terrainLayer.BaseColor = importLayerInfo.BaseColor;
					terrainLayer.NormalMap = importLayerInfo.NormalMap;
					terrainLayer.Metalic = importLayerInfo.Metalic;
					terrainLayer.Roughness = importLayerInfo.Roughness;
					terrainLayer.TileSizeX = importLayerInfo.TileSizeX;
					terrainLayer.TileSizeY = importLayerInfo.TileSizeY;
					terrainLayer.TileOffsetX = importLayerInfo.TileOffsetX;
					terrainLayer.TileOffsetY = importLayerInfo.TileOffsetY;

					terrainLayers.push_back(terrainLayer);
				}

				terrain.SetAlphaMap(gameObjectInfo.TerrainData.AlphaFileName);
				terrain.SetTerrainLayers(terrainLayers);
				terrain.SetWidth(gameObjectInfo.TerrainData.TerrainWidth);
				terrain.SetHeight(gameObjectInfo.TerrainData.TerrainHeight);
				terrain.SetHeightScale(gameObjectInfo.TerrainData.HeightScale * 0.5);

				// �� ������ ���� �� �о��
				terrain.SetTextureWidth(512);
				terrain.SetTextureHeight(512);
				terrain.SetHeightMap(gameObjectInfo.TerrainData.HeightFileName);

				terrain.SetLightmapUVScaleOffset(gameObjectInfo.TerrainData.LightmapScaleOffset);
				terrain.SetLightmapIndex(gameObjectInfo.TerrainData.LightmapIndex);
				terrain.SetIsStatic(gameObjectInfo.isStatic);
			}

			gameObjectsMap.insert({ gameObjectInfo.ID, gameObject });
		}

		// �θ� �ڽ� ����
		auto find = gameObjectsMap.find(gameObjectInfo.ParentID);

		if (find != gameObjectsMap.end())
		{
			auto parentTransform = find->second->GetComponent<game_module::Transform>();
			auto childTransform = gameObject->GetComponent<game_module::Transform>();

			parentTransform->AddChild(childTransform);
		}
		else
		{
			auto transform = gameObject->GetComponent<game_module::Transform>();
			auto sceneRootTransform = sceneRootObject->GetComponent<game_module::Transform>();
			sceneRootTransform->AddChild(transform);
			rootObjects.push_back(gameObject);
		}
	}

	mEditorProcess->mCommandSystem->Push<AddObjectCommand>(mScene, sceneRootObject);
}

std::vector<fq::game_engine::importData::GameObjectLoadInfo> fq::game_engine::ImportWindow::loadData(const std::filesystem::path& filePath)
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
		assert(!"���� ���� ����");

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
		info.isStatic = gameObjectJson["isStatic"].get<bool>();

		// Transform �Ľ�
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

		// Mesh �Ľ�
		info.MeshData.ModelPath = getFileName(gameObjectJson["Mesh"]["ModelPath"].get<std::string>());
		info.MeshData.Name = gameObjectJson["Mesh"]["Name"].get<std::string>();
		info.MeshData.LightmapScaleOffset = {
				gameObjectJson["Mesh"]["LightmapScaleOffset"]["x"].get<float>(),
				gameObjectJson["Mesh"]["LightmapScaleOffset"]["y"].get<float>(),
				gameObjectJson["Mesh"]["LightmapScaleOffset"]["z"].get<float>(),
				gameObjectJson["Mesh"]["LightmapScaleOffset"]["w"].get<float>()
		};
		info.MeshData.LightmapIndex = gameObjectJson["Mesh"]["LightmapIndex"].get<int>();

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

		// Light �Ľ�
		info.LightData.Type = gameObjectJson["Light"]["Type"].get<std::string>();
		info.LightData.Color = {
			gameObjectJson["Light"]["Color"]["x"].get<float>(),
			gameObjectJson["Light"]["Color"]["y"].get<float>(),
			gameObjectJson["Light"]["Color"]["z"].get<float>(),
			gameObjectJson["Light"]["Color"]["w"].get<float>()
		};
		info.LightData.Intensity = gameObjectJson["Light"]["Intensity"].get<float>();
		info.LightData.Range = gameObjectJson["Light"]["Range"].get<float>();
		info.LightData.SpotAngle = gameObjectJson["Light"]["SpotAngle"].get<float>();
		info.LightData.Mode = gameObjectJson["Light"]["Mode"].get<std::string>();

		// terrain �Ľ�
		auto find = gameObjectJson.find("Terrain");
		if (find != gameObjectJson.end())
		{
			info.TerrainData.Layers.reserve(4);
			for (auto& layerJson : gameObjectJson["Terrain"]["Layers"])
			{
				if (info.TerrainData.Layers.size() >= 4)
				{
					break;
				}

				importData::TerrainLayer terrainLayer;
				terrainLayer.BaseColor = layerJson["BaseColor"].get<std::string>();
				terrainLayer.NormalMap = layerJson["NormalMap"].get<std::string>();
				terrainLayer.Metalic = layerJson["Metalic"].get<float>();
				terrainLayer.Roughness = layerJson["Roughness"].get<float>();
				terrainLayer.TileSizeX = layerJson["TileSizeX"].get<float>();
				terrainLayer.TileSizeY = layerJson["TileSizeY"].get<float>();
				terrainLayer.TileOffsetX = layerJson["TileOffsetX"].get<float>();
				terrainLayer.TileOffsetY = layerJson["TileOffsetY"].get<float>();

				info.TerrainData.Layers.push_back(terrainLayer);
			}

			info.TerrainData.AlphaFileName = gameObjectJson["Terrain"]["AlphaFileName"].get<std::string>();
			info.TerrainData.HeightFileName = gameObjectJson["Terrain"]["HeightFileName"].get<std::string>();
			info.TerrainData.TextureWidth = gameObjectJson["Terrain"]["TextureWidth"].get<float>();
			info.TerrainData.TextureHeight = gameObjectJson["Terrain"]["TextureHeight"].get<float>();
			info.TerrainData.HeightScale = gameObjectJson["Terrain"]["HeightScale"].get<float>();
			info.TerrainData.TerrainWidth = gameObjectJson["Terrain"]["TerrainWidth"].get<float>();
			info.TerrainData.TerrainHeight = gameObjectJson["Terrain"]["TerrainHeight"].get<float>();
			info.TerrainData.LightmapScaleOffset.x = gameObjectJson["Terrain"]["LightmapScaleOffset"]["x"].get<float>();
			info.TerrainData.LightmapScaleOffset.y = gameObjectJson["Terrain"]["LightmapScaleOffset"]["y"].get<float>();
			info.TerrainData.LightmapScaleOffset.z = gameObjectJson["Terrain"]["LightmapScaleOffset"]["z"].get<float>();
			info.TerrainData.LightmapScaleOffset.w = gameObjectJson["Terrain"]["LightmapScaleOffset"]["w"].get<float>();
			info.TerrainData.LightmapIndex = gameObjectJson["Terrain"]["LightmapIndex"].get<int>();
		}

		objectInfos.push_back(info);
	}

	return objectInfos;
}

