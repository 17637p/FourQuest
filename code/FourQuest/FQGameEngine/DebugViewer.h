#pragma once

#include "IEditorWindow.h"

namespace fq::game_engine
{
	/// <summary>
	/// ����� �α׿� ���� ������ ǥ��
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