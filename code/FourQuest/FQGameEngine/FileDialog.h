#pragma once
#include "IEditorWindow.h"


namespace fq::game_engine
{
	/// <summary>
	/// ���� �ý����� �����ϴ� â
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