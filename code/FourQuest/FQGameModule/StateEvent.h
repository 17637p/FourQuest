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
			PrefabResource PrefabResourceData;

			// 생성한 부모를 따라갈지 여부
			bool bIsFollowingParentPosition = true;

			// 삭제여부
			bool bUseAutoDelete = true;
			bool bUseDeleteByStateEnd = false;
			float DeleteTime = 1.f;

			// 오프셋
			DirectX::SimpleMath::Vector3 Scale = { 1, 1, 1 };
			DirectX::SimpleMath::Vector3 Translate;
			DirectX::SimpleMath::Vector3 RandomRange;
		};

		struct PlaySoundInfo
		{
			std::string FunctionName;
			std::string SoundPath;

			// 삭제여부
			bool bUseAutoDelete = true;
			bool bUseDeleteByStateEnd = false;
			float DeleteTime = 1.f;

			// 반복여부
			bool bIsLoop;

			int Channel; // 0 ~ 31 
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
		std::vector<PlaySoundInfo> mPlayerSoundInfos;

		friend void RegisterMetaData();
	};
}