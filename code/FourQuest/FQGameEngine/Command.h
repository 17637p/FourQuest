#pragma once

#include <functional>
#include <memory>

#include "../FQGameModule/GameModule.h"

namespace fq::game_engine
{
	/// <summary>
	/// Ŀ�ǵ� �������̽�
	/// </summary>
	class Command
	{
	public:
		virtual ~Command() {}

		/// <summary>
		/// ���� 
		/// </summary>
		virtual void Excute() abstract;

		/// <summary>
		/// ���� ���
		/// </summary>
		virtual void Undo() abstract;
	};

	/// <summary>
	/// GameObject ���� ���
	/// </summary>
	class CreateObjectCommand : public Command
	{
	public:
		CreateObjectCommand(fq::game_module::Scene* scene
			, std::shared_ptr<fq::game_module::GameObject> object);

		~CreateObjectCommand();

		void Excute() override;

		void Undo() override;

	private:
		std::shared_ptr<fq::game_module::GameObject> mGameObject;
		fq::game_module::Scene* mScene;
	};

	/// <summary>
	/// GameObject ���� ���
	/// </summary>
	class DestroyObjectCommand : public Command
	{
	public:
		DestroyObjectCommand(fq::game_module::Scene* scene
			, std::shared_ptr<fq::game_module::GameObject> object);

		void Excute() override;
		void Undo() override;


	private:
		std::shared_ptr<fq::game_module::GameObject> mGameObject;
		fq::game_module::Scene* mScene;
	};

	/// <summary>
	/// ������Ʈ ���� ���
	/// </summary>
	class SelectObjectCommand : public Command
	{
	public:
		SelectObjectCommand(fq::game_module::EventManager* eventMgr
			, std::shared_ptr<fq::game_module::GameObject> current
			, std::shared_ptr<fq::game_module::GameObject> prev);

		void Excute() override;

		void Undo() override;

	private:
		std::shared_ptr<fq::game_module::GameObject> mPrevSelect;
		std::shared_ptr<fq::game_module::GameObject> mCurrentSelect;
		fq::game_module::EventManager* mEventManager;
	};

	template <typename T>
	class SetValueCommand : public Command
	{
	public:
		SetValueCommand(std::function<void(T)> setter, T oldVal, T newVal)
			:mSetter(setter)
			, mOldValue(oldVal)
			, mNewValue(newVal)
		{}

		void Excute() override
		{
			mSetter(mNewValue);
		}

		void Undo() override
		{
			mSetter(mOldValue);
		}

	private:
		std::function<void(T)> mSetter;
		T mOldValue;
		T mNewValue;
	};

	class BindFunctionCommand : public Command
	{
	public:
		BindFunctionCommand(std::function<void()> excute, std::function<void()> undo)
			:mExcute(excute)
			, mUndo(undo)
		{}

		void Excute() override
		{
			mExcute();
		}

		void Undo() override
		{
			mUndo();
		}

	private:
		std::function<void()> mExcute;
		std::function<void()> mUndo;
	};


}