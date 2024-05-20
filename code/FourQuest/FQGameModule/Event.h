#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <directxtk\SimpleMath.h>

#include "../FQReflect/entt.hpp"

namespace fq::game_module
{
	class GameObject;
	class Component;
	class Camera;
	class Light;
	class Animator;
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

	/// <summary>
	struct OnLoadScene
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
	//							Sound Event									// 
	//////////////////////////////////////////////////////////////////////////

	struct OnPlaySound
	{
		std::string path;
		bool bIsLoop;
		unsigned int channelIndex;
	};

	struct OnStopChannel
	{
		unsigned int channelIndex;
	};

	//////////////////////////////////////////////////////////////////////////
	//							Animation Event								// 
	//////////////////////////////////////////////////////////////////////////

	struct ChangeAnimationState
	{
		std::string exitState;
		std::string enterState;
		fq::game_module::GameObject* object;
	};

	struct WriteAnimation
	{
		std::string AnimationName;
		std::unordered_map<std::string, std::vector<DirectX::SimpleMath::Matrix>> animationData;
		fq::game_module::GameObject* object;
	};

}

