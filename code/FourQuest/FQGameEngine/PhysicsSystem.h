#pragma once

#include <directxtk/SimpleMath.h>

#include "../FQGameModule/EventHandler.h"
#include "../FQGameModule/Event.h"
#include "../FQGameModule/CollisionMatrix.h"

namespace fq::game_engine
{
	class GameProcess;

	class PhysicsSystem
	{
		using EventHandler = fq::game_module::EventHandler;
	public:
		PhysicsSystem();
		~PhysicsSystem();

		void Initialize(GameProcess* game);

		/// <summary>
		/// ���� �ε��Ҷ� �������� ���õ� ���ҽ��� �ε��մϴ�
		/// </summary>
		void OnLoadScene(const fq::event::OnLoadScene event);

		/// <summary>
		/// ���� ��ε��Ҷ� �������� ���õ� ���ҽ��� ��ε��մϴ�.
		/// </summary>
		void OnUnLoadScene();

		/// <summary>
		/// ���ӿ�����Ʈ �߰� �̺�Ʈ
		/// </summary>
		void OnAddGameObject(const fq::event::AddGameObject& event);

		/// <summary>
		/// ���ӿ�����Ʈ ���� �̺�Ʈ
		/// </summary>
		void OnDestroyedGameObject(const fq::event::OnDestoryedGameObject& event);

	private:
		GameProcess* mGameProcess;
		
		DirectX::SimpleMath::Vector3 mGravity;
		fq::physics::CollisionMatrix mCollisionMatrix;

		EventHandler mOnLoadSceneHandler;
		EventHandler mOnUnloadSceneHandler;
		EventHandler mOnAddGameObjectHandler;
		EventHandler mDestroyedGameObjectHandler;

		bool mbIsGameLoaded;
	};


}