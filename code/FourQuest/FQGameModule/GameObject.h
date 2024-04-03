#pragma once

namespace fq::game_module
{
	class Component;

	class GameObject
	{
	public:
		GameObject();

		GameObject(const GameObject& other);

		GameObject& operator=(const GameObject& other);

		~GameObject();

		void Initialize();

		void Start();

		void Update(float dt);

		void Finalize();

	private:
		unsigned int mID;
		std::string mName;
		Tag mTag;
		std::unordered_map<entt::id_type, std::unique_ptr<Component>> mComponents;
		
		FQ_REGISTRATION_FRIEND
	};
}