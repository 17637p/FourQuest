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

	struct GameObjectLoadInfo
	{
		std::string Name;
		int ID;
		int ParentID;

		DirectX::SimpleMath::Vector3 Position;
		DirectX::SimpleMath::Quaternion rotation;
		DirectX::SimpleMath::Vector3 Scale;

		struct Mesh
		{
			std::string ModelPath;
			std::string Name;
		};
	};

	class ImportWindow : public IEditorWindow
	{
	public:
		ImportWindow();
		~ImportWindow();

		void Initialize(GameProcess* game, EditorProcess* editor);
		void Render() override;

		bool& IsWindowOpen() { return mbIsOpen; }

	private:
		void loadGameObjectsForUnityByDirectory();
		std::vector<fq::game_engine::GameObjectLoadInfo> loadGameObjectInfosByJson(const std::filesystem::path& filePath);

	private:
		GameProcess* mGameProcess;
		EditorProcess* mEditorProcess;
		fq::game_module::Scene* mScene;

		std::filesystem::path mDirPath; // "./resource/Export/" °æ·Î 

		bool mbIsOpen;
	};
}

