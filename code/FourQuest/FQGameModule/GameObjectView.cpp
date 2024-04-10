#include "GameObjectView.h"

fq::game_module::internal::GameObjectView::GameObjectView(
	const GameObjectIterator& beginIter
	, const GameObjectIterator& endIter)
	:mBeginIterator(beginIter)
	, mEndIterator(endIter)
{}
