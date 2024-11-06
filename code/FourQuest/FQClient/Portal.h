#pragma once

#include "../FQGameModule/Component.h"
#include "../FQGameModule/PrefabResource.h"

namespace fq::client
{
	/// <summary>
	/// ¾À ÀÌµ¿ Æ÷Å» 
	/// </summary>
	class Portal : public  fq::game_module::Component
	{
	public:
		Portal();
		~Portal();

		void ChangeScene();
		void SetNextSceneName(std::string nextSceneName) { mNextSceneName = nextSceneName; }

	private:
		void OnStart()override;
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone = nullptr)const override;
		virtual void OnTriggerEnter(const fq::game_module::Collision& collision) override;
		entt::meta_handle GetHandle() override { return *this; }

	private:
		std::string mNextSceneName;
		game_module::PrefabResource mResultUIPrefab;
		bool mbIsImmediatelyChangeScene;
		friend void RegisterMetaData();
	};
}