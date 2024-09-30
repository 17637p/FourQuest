#pragma once

#include <vector>
#include <string>
#include <memory>

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

	private:
		GameProcess* mGameProcess;
		EditorProcess* mEditorProcess;

		std::string mPrefabPath;
		std::vector<std::shared_ptr<game_module::GameObject>> mPrefabInstance;
		std::vector<std::shared_ptr<game_module::GameObject>> mChangeObjects;
	};
}