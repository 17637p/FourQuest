#pragma once
#include "Component.h"


namespace fq::game_module
{
	class SkinnedMeshRenderer;
	class Transform;

	class Socket : public Component
	{
	public:
		Socket();
		~Socket();

		std::string GetBoneName() const { return mBoneName; }
		void SetBoneName(std::string val) { mBoneName = val; }
		void OnUpdate(float dt)override;

		void BindBone();
	private:
		void OnStart() override;
		entt::meta_handle GetHandle() override { return *this; }
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

	private:
		Transform* mTransform;
		SkinnedMeshRenderer* mSkinnedMeshRenderer;
		std::string mBoneName;
		unsigned int mBoneIndex;
	};
}