#pragma once
#include "IEditorWindow.h"

namespace fq::game_engine
{
	/// <summary>
	/// 리소스에 대한 정보를 표시한다.
	/// </summary>
	class Inspector : public IEditorWindow
	{
	public:
		Inspector();
		~Inspector();

		void Render() override;

	private:

	};


}