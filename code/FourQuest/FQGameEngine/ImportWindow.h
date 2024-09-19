#pragma once

#include "IEditorWindow.h"

#include <imgui.h>
#include <filesystem>
#include <fstream>
#include <directxtk\SimpleMath.h>

namespace fq::game_module
{
	class Scene;
	class GameObject;
}

namespace fq::game_engine
{
	class GameProcess;
	class EditorProcess;

	namespace importData
	{
		struct Transform
		{
			DirectX::SimpleMath::Vector3 Position;
			DirectX::SimpleMath::Quaternion Rotation;
			DirectX::SimpleMath::Vector3 Scale;
		};

		struct Material
		{
			std::string Name;
			std::string Path;
			std::string renderMode;
			DirectX::SimpleMath::Vector4 Albedo;
			float Metallic;
			float Smoothness;
			DirectX::SimpleMath::Vector4 Emission;
			std::string AlbedoMap;
			std::string MetallicAndSmoothnessMap;
			std::string NormalMap;
			std::string EmissionMap;
			DirectX::SimpleMath::Vector2 Tiling;
			DirectX::SimpleMath::Vector2 Offset;
		};

		struct Mesh
		{
			std::string ModelPath;
			std::string Name;
			std::vector<Material> Materials;
			DirectX::SimpleMath::Vector4 LightmapScaleOffset;
			int LightmapIndex;
		};

		struct Light
		{
			std::string Type;
			DirectX::SimpleMath::Vector4 Color;
			float Intensity;
			float Range;
			float SpotAngle;
			std::string Mode;
		};

		struct TerrainLayer
		{
			std::string BaseColor;
			std::string NormalMap;

			float Metalic;
			float Roughness;

			float TileSizeX;
			float TileSizeY;
			float TileOffsetX;
			float TileOffsetY;
		};

		struct Terrain
		{
			std::vector<TerrainLayer> Layers;
			std::string AlphaFileName; // R���� BaseColor1, G���� 2, B���� 3, A���� 4

			std::string HeightFileName; // Raw ����
			float TextureWidth;
			float TextureHeight;

			float HeightScale; // ��ü ���� (Length)
			float TerrainWidth; // ���� ũ��
			float TerrainHeight; // ���� ũ��

			DirectX::SimpleMath::Vector4 LightmapScaleOffset;
			int LightmapIndex;
		};

		struct GameObjectLoadInfo
		{
			std::string Name;
			int ID;
			int ParentID;
			Transform TransformData;
			Mesh MeshData;
			Light LightData;
			Terrain TerrainData;
			bool isStatic;
		};
	}

	class ImportWindow : public IEditorWindow
	{
	public:
		ImportWindow();
		~ImportWindow();

		void Initialize(GameProcess* game, EditorProcess* editor);
		void Render() override;

		bool& IsWindowOpen() { return mbIsOpen; }

	private:
		void createGameObject();
		void changeData();
		std::vector<importData::GameObjectLoadInfo> loadData(const std::filesystem::path& filePath);

		std::string createObjectName(const importData::GameObjectLoadInfo& info) const;

	private:
		GameProcess* mGameProcess;
		EditorProcess* mEditorProcess;
		fq::game_module::Scene* mScene;

		std::filesystem::path mImportFileName;
		std::filesystem::path mBasePath;

		bool mbIsOpen;
	};
}

