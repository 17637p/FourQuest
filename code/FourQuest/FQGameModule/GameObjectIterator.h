#pragma once

namespace fq::game_module
{
	class GameObject;
	class Scene;
}

namespace fq::game_module::internal
{
	class GameObjectIterator
	{
	public:
		GameObjectIterator(fq::game_module::Scene* scne, size_t index, bool bIsEnd,
			bool bIsIncludeToBeDestroyed);

		std::shared_ptr<GameObject> Get()const;

		bool IsEnd()const;

		bool IsIncludeToBeDestroyed()const { return mbIsIncludeToBeDestroyed; }

		std::shared_ptr<GameObject> operator*() const { return Get(); }

		GameObjectIterator& operator++();

		bool operator==(const GameObjectIterator& other) const;

		bool operator!=(const GameObjectIterator& other) const;

	private:
		size_t mIndex;
		class fq::game_module::Scene* mScene;
		bool mbIsEnd;
		bool mbIsIncludeToBeDestroyed;
	};
}

