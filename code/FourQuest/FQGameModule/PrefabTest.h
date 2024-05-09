#pragma once
#include "Component.h"
#include "PrefabResource.h"

namespace fq::game_module
{
	class PrefabTest :public Component
	{
	public:
		PrefabTest();
		~PrefabTest();

		/// <summary>
		/// 복사본을 반환합니다 
		/// </summary>
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

		void OnUpdate(float dt) override;

		float GetCreateTime() const { return mCreateTime; }
		void SetCreateTime(float val) { mCreateTime = val; }
		fq::game_module::PrefabResource GetFireObject() const { return mFireObject; }
		void SetFireObject(fq::game_module::PrefabResource val) { mFireObject = val; }

	private:
		entt::meta_handle GetHandle() override;

	private:
		PrefabResource mFireObject;
		float mCreateTime;
		float mElapsedTime;
	};

}