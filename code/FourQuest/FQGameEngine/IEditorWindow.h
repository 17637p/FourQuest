#pragma once

namespace fq::game_engine
{
	/// <summary>
	/// ������ â �������̽� 
	/// </summary>
	class IEditorWindow
	{
	public:
		virtual ~IEditorWindow(){}

		virtual void Render() abstract;
	};
}
