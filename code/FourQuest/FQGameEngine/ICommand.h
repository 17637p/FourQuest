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
		virtual ~ICommand() {}

		/// <summary>
		/// ���� 
		/// </summary>
		virtual void Excute() {};

		/// <summary>
		/// ���� ���
		/// </summary>
		virtual void Undo() {};
	};

	class CreateObject :ICommand
	{
	public:
		CreateObject(fq::game_module::Scene* scene
			, std::shared_ptr<fq::game_module::GameObject> object);

		void Excute() override;
		void Undo() override;

	private:
		std::shared_ptr<fq::game_module::GameObject> mGameObject;
		fq::game_module::Scene* mScene;
	};

}