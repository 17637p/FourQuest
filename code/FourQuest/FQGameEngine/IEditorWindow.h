#pragma once

namespace fq::game_engine
{
	/// <summary>
	/// 에디터 창 인터페이스 
	/// </summary>
	class IEditorWindow
	{
	public:
		virtual ~IEditorWindow(){}

		virtual void Render() abstract;
	};
}
