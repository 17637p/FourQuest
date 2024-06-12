#pragma once

#include "GameModule.h"

namespace fq::game_module
{
	class AddNavigationMeshObject : public fq::game_module::Component
	{
	public:
		AddNavigationMeshObject();
		virtual ~AddNavigationMeshObject() = default;

		bool GetIsUsed() const { return mIsUsed; }
		void SetIsUsed(bool val) { mIsUsed = val; }

		/// <summary>
		/// 복사본을 반환합니다 
		/// </summary>
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

	private:
		entt::meta_handle GetHandle() override { return *this; }

	private:
		bool mIsUsed;
	};
}

