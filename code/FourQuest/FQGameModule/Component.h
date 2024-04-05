#pragma once

namespace fq::game_module
{
	class GameObject;

	class Component
	{
	public:
		Component();

		Component(const Component& other) = default;

		Component(Component&& other) = default;

		virtual ~Component();

		virtual void Initialize() {};
		virtual void Start() {};
		virtual void FixedUpdate(float dt) {};
		virtual void Update(float dt) {};
		virtual void LateUpdate(float dt) {};
		virtual void Finalize() {};

	private:
		GameObject* mGameObject;
	};


}