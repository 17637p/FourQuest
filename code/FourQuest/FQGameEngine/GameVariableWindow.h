#pragma once

#include "IEditorWindow.h"
#include "../FQReflect/FQReflect.h"

namespace fq::game_engine
{
	/// <summary>
	/// 게임 전역 변수 설정 창 
	/// </summary>
	class GameVariableWindow : public IEditorWindow
	{
	public:
		GameVariableWindow();
		~GameVariableWindow();

		void Initialize();
		void Render() override;
		bool& IsWindowOpen() { return mbIsOpen; }

	private:
		void beginVariable(entt::meta_type type);

	private:
		bool mbIsOpen;

		std::vector<entt::meta_type> mVariableTypes;

	};


}