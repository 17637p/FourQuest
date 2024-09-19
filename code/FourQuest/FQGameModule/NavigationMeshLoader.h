#pragma once
#include "Component.h"

namespace fq::game_module
{
	class NavigationMeshLoader : public Component
	{
	public:
		NavigationMeshLoader();
		virtual ~NavigationMeshLoader();

		std::string GetNavigationMeshPath() const { return mNavigationMeshPath; }
		void SetNavigationMeshPath(std::string val) { mNavigationMeshPath = val; }

		/// <summary>
		/// 복사본을 반환합니다 
		/// </summary>
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

	private:
		entt::meta_handle GetHandle() override { return *this; }

	private:
		std::string mNavigationMeshPath;
	};
}
