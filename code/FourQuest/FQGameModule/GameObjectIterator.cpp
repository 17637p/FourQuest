#include "GameObjectIterator.h"
#include "Scene.h"
#include "GameObject.h"

fq::game_module::internal::GameObjectIterator::GameObjectIterator(
	fq::game_module::Scene* scne
	, size_t index
	, bool isEnd
	, bool isIncludeToBeDestroyed)
	:mIndex(index)
	, mScene(scne)
	, mbIsEnd(isEnd)
	, mbIsIncludeToBeDestroyed(isIncludeToBeDestroyed)
{
	if (mIndex >= scne->GetObjectSize())
	{
		mbIsEnd = true;
	}
}

fq::game_module::GameObject& fq::game_module::internal::GameObjectIterator::Get() const
{
	assert(!IsEnd() && "EndIterator cant't access");

	return *(mScene->GetObjectByIndex(mIndex));
}

bool fq::game_module::internal::GameObjectIterator::operator==(const GameObjectIterator& other) const
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

bool fq::game_module::internal::GameObjectIterator::operator!=(const GameObjectIterator& other) const
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

bool fq::game_module::internal::GameObjectIterator::IsEnd() const
{
	return mbIsEnd || mIndex >= mScene->GetObjectSize();
}

fq::game_module::internal::GameObjectIterator& fq::game_module::internal::GameObjectIterator::operator++()
{
	assert(!IsEnd());

	++mIndex;

	while (mIndex < mScene->GetObjectSize() &&
		(Get().IsDestroyed() && !mbIsIncludeToBeDestroyed))
	{
		++mIndex;
	}

	if (mIndex >= mScene->GetObjectSize())
	{
		mbIsEnd = true;
	}

	return *this;
}
