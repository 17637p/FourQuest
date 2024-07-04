#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <directxtk\SimpleMath.h>

#include "../FQReflect/entt.hpp"
#include "../FQCommon/IFQRenderResource.h"

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
	/// Scene Load �Ҷ� ȣ��
	/// </summary>
	/// ���� ������Ʈ �ı��� ȣ��
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
	/// Scene Start �Ҷ� ȣ��
	/// </summary>
	struct StartScene
	{

	};

	/// <summary>
	/// Scene Unload�� �� ȣ��
	/// </summary>
	struct OnUnloadScene
	{
	};

	/// <summary>
	/// �� CleanUp �Լ� ȣ��
	/// </summary>
	struct OnCleanUp
	{
	};

	/// <summary>
	/// �� ���� ��û �̺�Ʈ
	/// </summary>
	struct RequestChangeScene
	{
		std::string sceneName;
		bool bIsInvokeStartScene; // Ŭ���̾�Ʈ������ true�� ȣ���մϴ�
	};

	/// <summary>
	/// ���� ���� ��û �̺�Ʈ 
	/// </summary>
	struct RequestExitGame
	{
	};

	/// <summary>
	/// ���� ���ο� ���ӿ�����Ʈ�� �߰��� ȣ��
	/// </summary>
	struct AddGameObject
	{
		fq::game_module::GameObject* object;
	};

	/// <summary>
	/// ����ī�޶� �����մϴ� 
	/// </summary>
	struct SetMainCamera
	{
		fq::game_module::Camera* mainCamera;
	};

	/// <summary>
	/// ����ī�޶� ��ȯ�մϴ�
	/// </summary>
	struct GetMainCamera
	{
		fq::game_module::Camera** mainCamera;
	};

	/// <summary>
	/// ����Ʈ Ÿ�� ����
	/// </summary>
	struct SetLightType
	{
		fq::game_module::Light* light;
	};

	/// <summary>
	/// ������Ʈ �߰�
	/// </summary>
	struct AddComponent
	{
		entt::id_type id;
		fq::game_module::Component* component;
	};

	/// <summary>
	/// ������Ʈ ����
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
		std::string path;
		bool bIsLoop;
		unsigned int channelIndex;
	};

	struct OnStopChannel
	{
		unsigned int channelIndex;
	};

	//////////////////////////////////////////////////////////////////////////
	//							UI Event									// 
	//////////////////////////////////////////////////////////////////////////

	struct SetUIInfomations
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
	};

}

