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
	class CreateObject : public Command
	{
	public:
		CreateObject(fq::game_module::Scene* scene
			, std::shared_ptr<fq::game_module::GameObject> object);

		~CreateObject();

		void Excute() override;

		void Undo() override;

	private:
		std::shared_ptr<fq::game_module::GameObject> mGameObject;
		fq::game_module::Scene* mScene;
	};

	template <typename T>
	class SetValue : public Command
	{
	public:
		SetValue(std::function<void(T)> setter, T oldVal, T newVal)
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

	class BindFunction : public Command
	{
	public:
		BindFunction(std::function<void()> excute, std::function<void()> undo)
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