#pragma once

#include <memory>


namespace fq::game_module
{
	class Scene;
	class GameObject;
}

namespace fq::game_module::internal
{
	template <typename... Types>
	class ComponentIterator
	{
	public:
		ComponentIterator(Scene* scene
			, size_t index
			, bool bIsEnd
			, bool bIsIncludeToBeDestroyed);

		size_t GetIndex()const { return mIndex; }

		bool IsEnd() const
		{
			return mIndex >= mScene->GetObjectSize() || mbIsEnd;
		}

		std::shared_ptr<GameObject> Get() const
		{
			if (IsEnd())
			{
				return nullptr;
			}

			return mScene->GetObjectByIndex(mIndex);
		}

		bool IsIncludeToBeDestroyed()const { return mIsIncludeToBeDestroyed; }

		std::shared_ptr<GameObject> operator*() const { return Get(); }

		bool operator==(const ComponentIterator<Types...>& other)const
		{
			if (mScene != other.mScene)
			{
				return false;
			}

			if (IsEnd())
			{
				return other.IsEnd();
			}

			return mIndex == other.mIndex;
		}

		bool operator!=(const ComponentIterator<Types...>& other)const
		{
			if (mScene != other.mScene)
			{
				return true;
			}

			if (IsEnd())
			{
				return !other.IsEnd();
			}

			return mIndex != other.mIndex;
		}

		ComponentIterator<Types...>& operator++()
		{
			bool isEnd = false;

			do
			{
				++mIndex;

				std::shared_ptr<GameObject> object = Get();

				isEnd = (mIndex < mScene->GetObjectSize()) &&
					(object == nullptr || !object->template HasComponent<Types...>()
						|| (object->IsDestroyed() && IsIncludeToBeDestroyed()));

			} while (isEnd);

			if (mIndex >= mScene->GetObjectSize())
			{
				mbIsEnd = true;
			}

			return *this;
		}

	private:
		size_t mIndex;
		class fq::game_module::Scene* mScene;
		bool mbIsEnd;
		bool mIsIncludeToBeDestroyed;
	};

	template<typename ...Types>
	inline internal::ComponentIterator<Types...>::ComponentIterator(Scene* scene, size_t index, bool bIsEnd, bool bIsIncludeToBeDestroyed)
		:mScene(scene)
		, mIndex(index)
		, mbIsEnd(bIsEnd)
		, mIsIncludeToBeDestroyed(bIsIncludeToBeDestroyed)
	{
		if (mIndex >= mScene->GetObjectSize())
		{
			mbIsEnd = true;
		}
	}

}