#pragma once
#include "IEditorWindow.h"

namespace fq::game_engine
{
	/// <summary>
	/// ���ҽ��� ���� ������ ǥ���Ѵ�.
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