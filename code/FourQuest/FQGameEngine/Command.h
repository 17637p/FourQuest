#pragma once

namespace fq::game_engine
{
	/// <summary>
	/// Ŀ�ǵ� �������̽�
	/// </summary>
	class Command
	{
	public:
		virtual ~Command(){}

		/// <summary>
		/// ���� 
		/// </summary>
		virtual void Excute() abstract;
		
		/// <summary>
		/// ���� ���
		/// </summary>
		virtual void Undo() abstract;
	};
}