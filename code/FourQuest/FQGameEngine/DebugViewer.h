#pragma once

#include "IEditorWindow.h"

namespace fq::game_engine
{
	/// <summary>
	/// 디버그 로그에 대한 정보를 표시
	/// </summary>
	class DebugViewer : public IEditorWindow
	{
	public:
		DebugViewer();
		~DebugViewer();

		void Render() override;

	private:

	};


}