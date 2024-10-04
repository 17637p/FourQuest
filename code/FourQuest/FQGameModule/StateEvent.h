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
			float DeleteTime = 1.f;  // 삭제될 시간
			bool bUseDeleteByStateEnd = false;  // 상태변화에 따라 삭제될지 여부

			// 오프셋
			DirectX::SimpleMath::Vector3 Scale = { 1, 1, 1 };
			DirectX::SimpleMath::Vector3 Translate;
			DirectX::SimpleMath::Vector3 RandomRange;

			float PlayBackSpeed = 1.f;
			bool bIsPlaybackSppedHandleChildHierarchy = false;

			// 생성된 오브젝트 포인터로 이벤트 던질지 여부
			bool bIsGenerateEvent = false;
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

		struct AttackIntendInfo
		{
			std::string FunctionName;
			PrefabResource PrefabResourceData;
		};

	public:
		StateEvent() = default;
		~StateEvent() = default;

		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

		void UpdateMapData();

		const std::vector<InstantiatePrefab>& GetInstantiatePrefabs() const { return mInstantiatePrefabs; }
		const std::vector<PlaySoundInfo>& GetPlaySoundInfos() const { return mPlaySoundInfos; }

		const std::map<std::string, fq::game_module::StateEvent::InstantiatePrefab> GetInstantiatePrefabMap() const { return mInstantiatePrefabMap; }
		const std::map<std::string, fq::game_module::StateEvent::PlaySoundInfo> GetPlaySoundInfoMap() const { return mPlaySoundInfoMap; }

	private:
		entt::meta_handle GetHandle() override;

	private:
		std::vector<InstantiatePrefab> mInstantiatePrefabs;
		std::vector<PlaySoundInfo> mPlaySoundInfos;

		std::map<std::string, fq::game_module::StateEvent::InstantiatePrefab> mInstantiatePrefabMap;
		std::map<std::string, fq::game_module::StateEvent::PlaySoundInfo> mPlaySoundInfoMap;

		friend void RegisterMetaData();
	};
}