#pragma once

namespace fq::game_engine
{
	/// <summary>
	/// 커맨드 인터페이스
	/// </summary>
	class Command
	{
	public:
		virtual ~Command(){}

		/// <summary>
		/// 실행 
		/// </summary>
		virtual void Excute() abstract;
		
		/// <summary>
		/// 실행 취소
		/// </summary>
		virtual void Undo() abstract;
	};
}