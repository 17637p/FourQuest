#pragma once

#include <set>
#include <string>

#include "../FQGameModule/EventHandler.h"
#include "../FQGameModule/Event.h"

namespace fq::game_module
{
	class GameObject;
}

namespace fq::game_engine
{
	class GameProcess;

	/// <summary>
	/// ���������� ���ε� ó���� ����մϴ�
	/// </summary>
	class RenderingSystem
	{
		using ModelPath = std::string;
		using EventHandler = fq::game_module::EventHandler;
	public:
		RenderingSystem();
		~RenderingSystem();

		/// <summary>
		/// ������ �ý����� �ʱ�ȭ�մϴ�
		/// </summary>
		void Initialize(GameProcess* gameProcess);

		void Update(float dt);

		/// <summary>
		/// ���� �ε��Ҷ� �������� ���õ� ���ҽ��� �ε��մϴ�
		/// </summary>
		void OnLoadScene();

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
		void loadStaticMeshRenderer(fq::game_module::GameObject* object);
		void unloadStaticMeshRenderer(fq::game_module::GameObject* object);
		void loadModel(ModelPath path);
		void unloadAllModel();

	private:
		GameProcess* mGameProcess;
		EventHandler mOnLoadSceneHandler;
		EventHandler mOnUnloadSceneHandler;
		EventHandler mOnAddGameObjectHandler;
		EventHandler mDestroyedGameObjectHandler;
		
		bool mbIsGameLoaded;

		std::set<ModelPath> mLoadModels;
	};
}