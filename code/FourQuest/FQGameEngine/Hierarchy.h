#pragma once
#include "IEditorWindow.h"

namespace fq::game_engine
{
	/// <summary>
	/// Scene 계층구조 정보를 표시하는 창
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