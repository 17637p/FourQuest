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
		virtual ~ICommand() {}

		/// <summary>
		/// 실행 
		/// </summary>
		virtual void Excute() {};

		/// <summary>
		/// 실행 취소
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