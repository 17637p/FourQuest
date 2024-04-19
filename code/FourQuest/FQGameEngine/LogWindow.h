#pragma once

#include "IEditorWindow.h"

namespace fq::game_engine
{
	/// <summary>
	/// 디버그 로그에 대한 정보를 표시
	/// </summary>
	class LogWindow : public IEditorWindow
	{
	public:
		LogWindow();
		~LogWindow();

		void Initialize();

		void Render() override;

	private:


	};


}