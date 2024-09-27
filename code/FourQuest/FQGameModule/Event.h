#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <directxtk\SimpleMath.h>

#include "../FQReflect/entt.hpp"
#include "../FQCommon/IFQRenderResource.h"
#include "GameModuleEnum.h"

namespace fq::game_module
{
	class GameObject;
	class Component;
	class Camera;
	class Light;
	class Animator;
	class ArticulationData;
	class LinkData;
}

namespace fq::event
{
	/// <summary>
	/// Scene Load 할때 호출
	/// </summary>
	/// 게임 오브젝트 파괴시 호출
	/// </summary>
	struct OnDestoryedGameObject
	{
		fq::game_module::GameObject* object;
	};

	struct OnLoadScene
	{
		std::string sceneName;
	};

	struct PreOnLoadScene
	{
		std::string sceneName;
	};


	/// <summary>
	/// Scene Start 할때 호출
	/// </summary>
	struct StartScene
	{

	};

	/// <summary>
	/// Scene Unload할 때 호출
	/// </summary>
	struct OnUnloadScene
	{
	};

	/// <summary>
	/// 씬 CleanUp 함수 호출
	/// </summary>
	struct OnCleanUp
	{
	};

	/// <summary>
	/// 씬 변경 요청 이벤트
	/// </summary>
	struct RequestChangeScene
	{
		std::string sceneName;
		bool bIsInvokeStartScene; // 클라이언트에서는 true로 호출합니다
	};

	/// <summary>
	/// 게임 종료 요청 이벤트 
	/// </summary>
	struct RequestExitGame
	{
	};

	/// <summary>
	/// 씬에 새로운 게임오브젝트를 추가시 호출
	/// </summary>
	struct AddGameObject
	{
		fq::game_module::GameObject* object;
	};

	/// <summary>
	/// 메인카메라를 설정합니다 
	/// </summary>
	struct SetMainCamera
	{
		fq::game_module::Camera* mainCamera;
	};

	/// <summary>
	/// 메인카메라를 반환합니다
	/// </summary>
	struct GetMainCamera
	{
		fq::game_module::Camera** mainCamera;
	};

	/// <summary>
	/// 라이트 타입 변경
	/// </summary>
	struct SetLightType
	{
		fq::game_module::Light* light;
	};

	/// <summary>
	/// 컴포넌트 추가
	/// </summary>
	struct AddComponent
	{
		entt::id_type id;
		fq::game_module::Component* component;
	};

	/// <summary>
	/// 컴포넌트 제거
	/// </summary>
	struct RemoveComponent
	{
		entt::id_type id;
		fq::game_module::Component* component;
	};

	//////////////////////////////////////////////////////////////////////////
	//							Screen Event								// 
	//////////////////////////////////////////////////////////////////////////

	struct SetScreenSize
	{
		unsigned short width;
		unsigned short height;
	};

	struct SetViewportSize
	{
		unsigned short width;
		unsigned short height;
	};

	//////////////////////////////////////////////////////////////////////////
	//							Sound Event									// 
	//////////////////////////////////////////////////////////////////////////

	struct OnPlaySound
	{
		std::string key;
		bool bIsLoop;
		unsigned int channelIndex;
	};

	struct OnStopChannel
	{
		unsigned int channelIndex;
	};

	//////////////////////////////////////////////////////////////////////////
	//								UI Event									// 
	//////////////////////////////////////////////////////////////////////////

	struct SetUIInfomations
	{
		fq::game_module::GameObject* object;
	};

	struct SetTextInformation
	{
		fq::game_module::GameObject* object;
	};

	struct SetSpriteAnimationInformation
	{
		fq::game_module::GameObject* object;
	};

	//////////////////////////////////////////////////////////////////////////
	//							Light Event									// 
	//////////////////////////////////////////////////////////////////////////

	struct SetLightShadow
	{
		bool bUseShadow;
		unsigned int id;
	};

	//////////////////////////////////////////////////////////////////////////
	//							Animation Event								// 
	//////////////////////////////////////////////////////////////////////////

	struct ChangeAnimationState
	{
		bool bIsBlend;
		std::shared_ptr<fq::graphics::IAnimation> currentState;
		std::shared_ptr<fq::graphics::IAnimation> nextState;
		fq::game_module::Animator* animator;
	};

	struct WriteAnimation
	{
		std::string animationName;
		std::unordered_map<std::string, std::vector<DirectX::SimpleMath::Matrix>> animationData;
		fq::game_module::GameObject* object;
		unsigned int animationSize;
	};

	//////////////////////////////////////////////////////////////////////////
	//						Physics Event									// 
	//////////////////////////////////////////////////////////////////////////

	struct AddInputMove
	{
		unsigned int colliderID;
		DirectX::SimpleMath::Vector3 input;
		bool isDynamic;
	};

	/// <summary>
	/// RayCast 이벤트를 보내면 
	/// result에 결과값을 전달합니다 
	/// </summary>
	struct RayCast
	{
		struct ResultData
		{
			bool hasBlock = false;
			game_module::GameObject* blockObject;
			DirectX::SimpleMath::Vector3 blockPosition;

			unsigned int hitCount = -1;
			std::vector<game_module::GameObject*> hitObjects;
			std::vector<DirectX::SimpleMath::Vector3> hitContactPoints;
		};

		DirectX::SimpleMath::Vector3 origin = DirectX::SimpleMath::Vector3::Zero;
		DirectX::SimpleMath::Vector3 direction = DirectX::SimpleMath::Vector3::Zero;
		float distance = 0.f;
		game_module::ETag tag = game_module::ETag::Untagged;

		ResultData* result = nullptr;
		bool bUseDebugDraw = false;
	};

	//////////////////////////////////////////////////////////////////////////
	//							Articulation Event							// 
	//////////////////////////////////////////////////////////////////////////

	struct SelectLinkData
	{
		std::shared_ptr<fq::game_module::ArticulationData> articulationData;
		std::shared_ptr<fq::game_module::LinkData> linkData;
	};

	struct SetSimulationPhysics
	{
		unsigned int colliderID;
		bool bIsRagdoll;
	};


	//////////////////////////////////////////////////////////////////////////
	//							Material Event							// 
	//////////////////////////////////////////////////////////////////////////

	/// <summary>
	/// 리소스 매니저에 머터리얼 생성을 요청합니다.
	/// </summary>
	struct LoadMaterial
	{
		std::string materialPath;
		std::shared_ptr<fq::graphics::IMaterial>* materialPtr;
	};

	//////////////////////////////////////////////////////////////////////////
	//							Debug Draw Event							// 
	//////////////////////////////////////////////////////////////////////////

	struct DrawDebugLay
	{
		fq::graphics::debug::RayInfo rayInfo;
	};

	//////////////////////////////////////////////////////////////////////////
	//							OnCreateStateEvent							// 
	//////////////////////////////////////////////////////////////////////////

	struct OnCreateStateEvent
	{
		std::string RegisterKeyName;
		fq::game_module::GameObject* gameObject;
	};

	struct OnDeleteStateEvent
	{
		fq::game_module::GameObject* gameObject;
	};

	//////////////////////////////////////////////////////////////////////////
	//							Sequence Event								// 
	//////////////////////////////////////////////////////////////////////////
	struct SequenceEnterSpeech
	{
		std::string name; // 말하는 캐릭터 이름
	};

	struct SequenceExitSpeech
	{
		std::string name; // 말하는 캐릭터 이름
	};
}

