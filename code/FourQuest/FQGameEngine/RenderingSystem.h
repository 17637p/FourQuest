#pragma once

#include "../FQGameModule/EventHandler.h"

namespace fq::game_engine
{
	class GameProcess;

	/// <summary>
	/// ���������� ���ε� ó���� ����մϴ�
	/// </summary>
	class RenderingSystem
	{
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
		void LoadScene();

		/// <summary>
		/// ���� ��ε��Ҷ� �������� ���õ� ���ҽ��� ��ε��մϴ�.
		/// </summary>
		void UnLoadScene();

	private:
		GameProcess* mGameProcess;
		EventHandler mOnLoadSceneHandler;
		EventHandler mOnUnloadSceneHandler;
	};
}