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

		struct GameObjectLoadInfo
		{
			std::string Name;
			int ID;
			int ParentID;
			Transform TransformData;
			Mesh MeshData;
			Light LightData;
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
		std::vector<importData::GameObjectLoadInfo> loadData(const std::filesystem::path& filePath);

	private:
		GameProcess* mGameProcess;
		EditorProcess* mEditorProcess;
		fq::game_module::Scene* mScene;

		std::filesystem::path mImportFileName;
		std::filesystem::path mFBXDirectory;
		std::filesystem::path mTextureDirectory;
		std::filesystem::path mMaterialDirectory;

		bool mbIsOpen;
	};
}

