#pragma once

namespace fq::game_module
{
	class GameObject;

	class Component
	{
	public:
		Component();
		virtual ~Component();

		virtual void Initialize() {};

		virtual void Start() {};

		virtual void Update(float dt) {};

		virtual void Finalize() {};
	private:
		GameObject* mGameObject;
	};


}