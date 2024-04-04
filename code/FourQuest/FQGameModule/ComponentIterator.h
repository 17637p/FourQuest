#pragma once


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
			return mIndex >= mScene->GetObjectSize() || mIsEnd;
		}

		bool IsIncludeToBeDestroyed()const { return mIsIncludeToBeDestroyed; }

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
			++mIndex;

		}


	private:
		size_t mIndex;
		class fq::game_module::Scene* mScene;
		bool mIsEnd;
		bool mIsIncludeToBeDestroyed;
	};

	template<typename ...Types>
	inline internal::ComponentIterator<Types...>::ComponentIterator(Scene* scene, size_t index, bool bIsEnd, bool bIsIncludeToBeDestroyed)
	{
	}
}