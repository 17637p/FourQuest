#pragma once

#include "IEditorWindow.h"

#include <filesystem>

#include "../FQGameModule/GameModule.h"

namespace fq::graphics
{
	class IFQGraphics;
}

namespace fq::game_engine
{
	class GameProcess;
	class EditorProcess;

	class MaterialWindow : public IEditorWindow
	{
		using EventHandler = fq::game_module::EventHandler;

	public:
		MaterialWindow();
		~MaterialWindow() = default;

		void Initialize(GameProcess* game, EditorProcess* editor);
		void Render() override;
		bool& IsWindowOpen() { return mbIsOpen; }
		void SaveMaterial();
		void OnUnloadScene();

	private:
		bool mbIsOpen;

		GameProcess* mGameProcess;
		EditorProcess* mEditorProcess;
		fq::game_module::EventManager* mEventManager;

		std::filesystem::path mSelectMaterialPaths;
		std::shared_ptr<graphics::IMaterial> mMaterialInterface;

		EventHandler mOnLoadSceneHandler;
	};
}