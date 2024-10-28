#pragma once

#include <vector>
#include <string>
#include <map>
#include <memory>

#include "../FQGameModule/PrefabResource.h"
#include "IEditorWindow.h"

namespace fq::game_module
{
	class GameObject;
}

namespace fq::game_engine
{
	class GameProcess;
	class EditorProcess;

	/// <summary>
	/// 씬 오브젝트 변경 윈도우 
	/// </summary>
	class ChangeWindow : public IEditorWindow
	{
	public:
		void Render()override;
		void Initialize(GameProcess* game, EditorProcess* editor);
		bool& IsWindowOpen() { return mbIsOpen; }

	private:
		void beginPrefabWindow();
		void beginSearchWindow();
		void beginChangeWindow();

		std::vector<std::shared_ptr<game_module::GameObject>> loadClonePrefabResource();
		void change();

	private:
		GameProcess* mGameProcess;
		EditorProcess* mEditorProcess;
		bool mbIsOpen = false;
		bool mbUsePrevObjectScale = false;

		game_module::PrefabResource mPrefabPath;

		std::string mSearchName;
		std::vector<std::shared_ptr<game_module::GameObject>> mSearchList;
		std::map<unsigned int ,std::shared_ptr<game_module::GameObject>> mChangeList;;
	};
}