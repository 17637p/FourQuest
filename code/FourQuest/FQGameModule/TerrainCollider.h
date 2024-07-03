#pragma once
#include "Component.h"
#include "ICollider.h"

#include "../FQCommon/FQCommonPhysics.h"

namespace fq::game_module
{
	/// <summary>
	/// 터레인 콜라이더 
	/// </summary>
	class TerrainCollider : public Component, public ICollider
	{
	public:
		TerrainCollider();
		~TerrainCollider();


		DirectX::SimpleMath::Vector3 GetOffset() const override { return mOffset; }
		void SetOffset(DirectX::SimpleMath::Vector3 offset) override { mOffset = offset; }
		unsigned int GetColliderID() const { return mColliderID; }
		void SetColliderID(unsigned int val) { mColliderID = val; }

	private:
		entt::meta_handle GetHandle() override { return *this; }
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;


	private:
		DirectX::SimpleMath::Vector3 mOffset;
		unsigned int mColliderID;
	};

}