#pragma once
#include "IEditorWindow.h"


namespace fq::game_engine
{
	/// <summary>
	/// 파일 시스템을 관리하는 창
	/// </summary>
	class FileDialog : public IEditorWindow
	{
	public:
		FileDialog();
		~FileDialog();

		void Render() override;

	private:

	};


}