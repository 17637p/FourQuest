#pragma once

namespace fq::game_module
{
	class GameObject;
}

namespace fq::game_module::internal
{
	template <typename... Types>
	class ComponentIterator;

	template <typename... Types>
	class ComponentView
	{
	public:
		ComponentView(const ComponentIterator<Types...>& beginIter
			, const ComponentIterator<Types...>& endIter)
			:mBeginIterator(beginIter) 
			,mEndIterator(endIter)
		{
			if (mBeginIterator.Get() == nullptr ||
				(mBeginIterator.Get()->IsToBeDestroyed()
					&& !mBeginIterator.IsIncludeToBeDestroyed())
				|| !mBeginIterator.Get()->template HasComponent<Types...>())
			{
				++mBeginIterator;
			}
		}

		const ComponentIterator<Types...> begin()const
		{
			return mBeginIterator;
		}

		const ComponentIterator<Types...> end()const
		{
			return mEndIterator;
		}

	private:
		ComponentIterator<Types...> mBeginIterator;
		ComponentIterator<Types...> mEndIterator;
	};

}
