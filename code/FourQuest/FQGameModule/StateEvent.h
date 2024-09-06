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
		struct InstantiatePrefab // ���� ��ġ���� �������� �������ִ� �̺�Ʈ�� ����
		{
			std::string FunctionName;
			PrefabResource PrefabResourceData;

			// ������ �θ� ������ ����
			bool bIsFollowingParentPosition = true;

			// ��������
			bool bUseAutoDelete = true;
			bool bUseDeleteByStateEnd = false;
			float DeleteTime = 1.f;

			// ������
			DirectX::SimpleMath::Vector3 Scale = { 1, 1, 1 };
			DirectX::SimpleMath::Vector3 Translate;
			DirectX::SimpleMath::Vector3 RandomRange;
		};

		struct PlaySoundInfo
		{
			std::string FunctionName;
			std::string SoundPath;

			// ��������
			bool bUseAutoDelete = true;
			bool bUseDeleteByStateEnd = false;
			float DeleteTime = 1.f;

			// �ݺ�����
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