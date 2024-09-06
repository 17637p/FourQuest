#pragma once

#include <string>
#include <map>
#include <filesystem>

#include "Component.h"
#include "../FQGameModule/PrefabResource.h"

namespace fq::game_module
{
	class StateEvent : public Component
	{
	public:
		struct InstantiatePrefab // 현재 위치에서 프리팹을 생성해주는 이벤트를 던짐
		{
			std::string FunctionName;
			std::string PrefabResource;
			float DeleteTime = 2.f;
		};

		struct PlaySoundInfo
		{
			std::string FunctionName;
			std::string SoundPath;
		};

	public:
		StateEvent();
		~StateEvent();

		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

		const std::vector<InstantiatePrefab>& GetInstantiatePrefabs() const { return mInstantiatePrefabs; }

	private:
		entt::meta_handle GetHandle() override;

	private:
		std::vector<InstantiatePrefab> mInstantiatePrefabs;


		friend void RegisterMetaData();
	};
}