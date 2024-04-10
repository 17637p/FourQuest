#pragma once

#include <memory>
#include "../FQGameModule/GameModule.h"

namespace fq::game_engine
{
	/// <summary>
	/// 커맨드 인터페이스
	/// </summary>
	class ICommand
	{
	public:
		virtual ~ICommand(){}

		/// <summary>
		/// 실행 
		/// </summary>
		virtual void Excute() abstract;
		
		/// <summary>
		/// 실행 취소
		/// </summary>
		virtual void Undo() abstract;
	};

	class CreateObject :ICommand
	{
	public:
		void Excute() override;
		void Undo() abstract;

	private:
		std::shared_ptr<fq::game_module::GameObject> mGameObject;
	};

}