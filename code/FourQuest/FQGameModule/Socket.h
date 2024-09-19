#pragma once
#include "Component.h"
#include "../FQCommon/IFQRenderResource.h"

namespace fq::game_module
{
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
		std::shared_ptr<fq::graphics::INodeHierarchyInstance> mNodeHierarchyInstance;
		std::string mBoneName;
		unsigned int mBoneIndex;
	};
}