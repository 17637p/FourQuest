#pragma once

#include "IEditorWindow.h"

namespace fq::game_engine
{
	/// <summary>
	/// ����� �α׿� ���� ������ ǥ��
	/// </summary>
	class LogWindow : public IEditorWindow
	{
	public:
		LogWindow();
		~LogWindow();

		void Render() override;

	private:

	};


}