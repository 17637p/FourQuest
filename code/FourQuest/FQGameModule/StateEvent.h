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
		struct InstantiatePrefab
		{
			std::string FunctionName;
			PrefabResource PrefabResourceData;

			// 생성한 부모를 따라갈지 여부
			bool bIsFollowingParentPosition = true;

			// 삭제여부
			bool bUseAutoDelete = true; // 상태변화나 시간에 따른 변화에 삭제될지 여부
			bool bUseDeleteByStateEnd = false;  // 상태변화에 따라 삭제될지 여부
			float DeleteTime = 1.f;  // 삭제될 시간

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
			bool bUseAutoDelete = false; // 상태변화나 시간에 따른 변화에 삭제될지 여부
			bool bUseDeleteByStateEnd = false; // 상태변화에 따라 삭제될지 여부
			float DeleteTime = 1.f; // 삭제될 시간

			// 반복여부
			bool bIsLoop = false;

			unsigned int Channel = 0; // 0 ~ 31 
		};

	public:
		StateEvent() = default;
		~StateEvent() = default;

		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

		const std::vector<InstantiatePrefab>& GetInstantiatePrefabs() const { return mInstantiatePrefabs; }
		const std::vector<PlaySoundInfo>& GetPlaySoundInfos() const { return mPlayerSoundInfos; }

	private:
		entt::meta_handle GetHandle() override;

	private:
		std::vector<InstantiatePrefab> mInstantiatePrefabs;
		std::vector<PlaySoundInfo> mPlayerSoundInfos;

		friend void RegisterMetaData();
	};
}