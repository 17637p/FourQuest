#pragma once

#include <cassert>
#include <functional>
#include <memory>
#include <vector>

#include "../FQReflect/FQReflect.h"
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
		virtual void Excute() abstract;

		/// <summary>
		/// 실행 취소
		/// </summary>
		virtual void Undo() abstract;
	};

	/// <summary>
	/// GameObject 생성 명령
	/// </summary>
	class AddObjectCommand : public ICommand
	{
	public:
		AddObjectCommand(fq::game_module::Scene* scene
			, std::shared_ptr<fq::game_module::GameObject> object);

		~AddObjectCommand();

		void Excute() override;

		void Undo() override;

	private:
		std::shared_ptr<fq::game_module::GameObject> mGameObject;
		std::vector< std::shared_ptr<fq::game_module::GameObject>> mChildren;
		fq::game_module::Scene* mScene;
	};

	/// <summary>
	/// GameObject 삭제 명령
	/// </summary>
	class DestroyObjectCommand : public ICommand
	{
	public:
		DestroyObjectCommand(fq::game_module::Scene* scene
			, std::shared_ptr<fq::game_module::GameObject> object);

		void Excute() override;
		void Undo() override;

	private:
		std::shared_ptr<fq::game_module::GameObject> mGameObject;
		std::shared_ptr<fq::game_module::GameObject> mParent;
		std::vector< std::shared_ptr<fq::game_module::GameObject>> mChildren;
		fq::game_module::Scene* mScene;
	};

	/// <summary>
	/// 오브젝트 선택 명령
	/// </summary>
	class SelectObjectCommand : public ICommand
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
	class SetValueCommand : public ICommand
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

	class BindFunctionCommand : public ICommand
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

	class SetMetaData : public ICommand
	{
	public:
		SetMetaData(entt::meta_data data
			, std::shared_ptr<fq::game_module::GameObject> object
			, fq::reflect::IHandle* handle
			, entt::meta_any newVal)
			: mData(data)
			, mHandle(handle)
			, mObject(object)
			, mNewValue(newVal)
		{
			mOldValue = data.get(handle->GetHandle());
		}

		void Excute() override
		{
			assert(mData.set(mHandle->GetHandle(), mNewValue));
		}

		void Undo() override
		{
			assert(mData.set(mHandle->GetHandle(), mOldValue));
		}

	private:
		entt::meta_any mNewValue;
		entt::meta_any mOldValue;
		entt::meta_data mData;
		fq::reflect::IHandle* mHandle;
		std::shared_ptr<fq::game_module::GameObject> mObject;
	};
}