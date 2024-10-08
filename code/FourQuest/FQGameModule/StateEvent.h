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

			// ������ �θ� ������ ����
			bool bIsFollowingParentPosition = true;

			// ��������
			bool bUseAutoDelete = true; // ���º�ȭ�� �ð��� ���� ��ȭ�� �������� ����
			float DeleteTime = 1.f;  // ������ �ð�
			bool bUseDeleteByStateEnd = false;  // ���º�ȭ�� ���� �������� ����

			// ������
			DirectX::SimpleMath::Vector3 Scale = { 1, 1, 1 };
			DirectX::SimpleMath::Vector3 Translate;
			DirectX::SimpleMath::Vector3 RandomRange;

			float PlayBackSpeed = 1.f;
			bool bIsPlaybackSppedHandleChildHierarchy = false;

			// ������ ������Ʈ �����ͷ� �̺�Ʈ ������ ����
			bool bIsGenerateEvent = false;
		};

		struct PlaySoundInfo
		{
			std::string FunctionName;
			std::string SoundPath;

			// ��������
			bool bUseAutoDelete = false; // ���º�ȭ�� �ð��� ���� ��ȭ�� �������� ����
			bool bUseDeleteByStateEnd = false; // ���º�ȭ�� ���� �������� ����
			float DeleteTime = 1.f; // ������ �ð�

			// �ݺ�����
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