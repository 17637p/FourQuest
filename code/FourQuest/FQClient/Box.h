#pragma once

#include "../FQGameModule/Component.h"
#include "../FQGameModule/PrefabResource.h"

namespace fq::client
{
	class Box : public fq::game_module::Component
	{
	public:
		Box();
		~Box();

		void SetmBrokenBoxPrefebPath(const std::string& path) { mBrokenBoxPrefebPath = path; }
		std::string GetBrokenBoxPrefebPath() const { return mBrokenBoxPrefebPath; }
		void SetDeadTime(const float& deadTime) { mDeadTime = deadTime; }
		float GetDeadTime() const { return mDeadTime; }

	private:
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone = nullptr)const override;
		entt::meta_handle GetHandle() override;

		virtual void OnStart() override;
		virtual void OnTriggerEnter(const fq::game_module::Collision& collision) override;
		virtual void OnFixedUpdate(float dt) override;


	private:
		std::string mBrokenBoxPrefebPath;
		std::shared_ptr<fq::game_module::GameObject> mObject;

		DirectX::SimpleMath::Vector3 mRotation;
		bool bIsBlock;
		float mDeadTime;
		float mDurationTime;
	};
}

