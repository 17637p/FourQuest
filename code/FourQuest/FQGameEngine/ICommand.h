#pragma once

#include <memory>
#include "../FQGameModule/GameModule.h"

namespace fq::game_engine
{
	/// <summary>
	/// Ŀ�ǵ� �������̽�
	/// </summary>
	class ICommand
	{
	public:
		virtual ~ICommand(){}

		/// <summary>
		/// ���� 
		/// </summary>
		virtual void Excute() abstract;
		
		/// <summary>
		/// ���� ���
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