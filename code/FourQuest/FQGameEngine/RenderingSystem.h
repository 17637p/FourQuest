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
		using Path = std::string;
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

		/// <summary>
		/// AddComponent �̺�Ʈ
		/// </summary>
		void AddComponent(const fq::event::AddComponent& event);

		/// <summary>
		/// RemoveCompnoent �̺�Ʈ
		/// </summary>
		void RemoveComponent(const fq::event::RemoveComponent& event);

		/// <summary>
		/// ���� �ε���� Ȯ���մϴ�
		/// </summary>
		bool IsLoadedModel(unsigned int key);

		/// <summary>
		/// ���� �ε��մϴ� 
		/// </summary>
		void LoadModel(const Path& path, const std::string& texturePath = {});

		/// <summary>
		/// �ִϸ��̼��� �۵��մϴ�.
		/// </summary>
		void WriteAnimation(const fq::event::WriteAnimation& event);

		unsigned int GetModelKey(const Path& modelPath, const Path& texturePath)const;

	private:
		void loadStaticMeshRenderer(fq::game_module::GameObject* object);
		void unloadStaticMeshRenderer(fq::game_module::GameObject* object);

		void loadSkinnedMeshRenderer(fq::game_module::GameObject* object);
		void unloadSkinnedMeshRenderer(fq::game_module::GameObject* object);

		void loadTerrain(fq::game_module::GameObject* object);
		void unloadTerrain(fq::game_module::GameObject* object);

		void loadAnimation(fq::game_module::GameObject * object);

		void unloadAllModel();



	private:
		GameProcess* mGameProcess;
		EventHandler mOnLoadSceneHandler;
		EventHandler mOnUnloadSceneHandler;
		EventHandler mOnAddGameObjectHandler;
		EventHandler mDestroyedGameObjectHandler;
		EventHandler mAddComponentHandler;
		EventHandler mRemoveComponentHandler;
		EventHandler mWriteAnimationHandler;
		EventHandler mSetViewportSizeHandler;

		bool mbIsGameLoaded;
		DirectX::SimpleMath::Matrix mPlaneMatrix;

		std::set<unsigned int> mLoadModels;
	};
}