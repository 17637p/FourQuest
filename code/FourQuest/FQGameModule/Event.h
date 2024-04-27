#pragma once

#include <string>

#include "../FQReflect/entt.hpp"

namespace fq::game_module
{
	class GameObject;
	class Component;
	class Camera;
	class Light;
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
	/// ����Ʈ ���� ���� 
	/// </summary>
	struct SetLightInfo
	{
		fq::game_module::Light* light;
		bool bIsChangedType;
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
}
