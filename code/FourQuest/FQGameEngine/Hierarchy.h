#pragma once
#include "IEditorWindow.h"

namespace fq::game_engine
{
	/// <summary>
	/// Scene �������� ������ ǥ���ϴ� â
	/// </summary>
	class Hierarchy : public IEditorWindow
	{
	public:
		Hierarchy();
		~Hierarchy();

		void Render() override;
	private:

	};


}