#pragma once

#include "GameObjectIterator.h"
#include "GameObjectView.h"

namespace fq::game_module::internal
{
	class GameObjectView
	{
	public:
		GameObjectView(const GameObjectIterator& beginIter, const GameObjectIterator& endIter);

		const GameObjectIterator& begin() const { return mBeginIterator; }

		const GameObjectIterator& end() const { return mEndIterator; }

	private:
		GameObjectIterator mBeginIterator;
		GameObjectIterator mEndIterator;
	};
}