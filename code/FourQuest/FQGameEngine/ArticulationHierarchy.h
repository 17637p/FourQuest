#pragma once

#include <memory>

#include "IEditorWindow.h"

#include "../FQReflect/FQReflect.h"
#include "../FQGameModule/GameModule.h"

namespace fq::game_engine
{
	class GameProcess;
	class EditorProcess;

	class ArticulationHierarchy : public IEditorWindow
	{
	public:
		ArticulationHierarchy();
		~ArticulationHierarchy();

		/// <summary>
		/// 계층구조 창을 초기화합니다
		/// </summary>
		/// <param name="process">게임프로세스</param>
		void Initialize(GameProcess* game, EditorProcess* editor);

		/// <summary>
		/// 
		/// </summary>
		virtual void Render() override;

	private:
		GameProcess* mGameProcess;
		EditorProcess* mEditorProcess;

		bool mbIsOpen;

		fq::game_module::EventManager* mEventManager;
		std::shared_ptr<fq::game_module::GameObject> mSelectObject;
	};
}