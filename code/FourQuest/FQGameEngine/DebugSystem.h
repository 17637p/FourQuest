#pragma once

#include "../FQCommon/FQCommonGraphics.h"
#include "../FQGameModule/GameModule.h"

namespace fq::game_engine
{
	class GameProcess;

	/// <summary>
	/// ����� ������ ���� ������ ó���ϴ� �ý��� 
	/// </summary>
	class DebugSystem
	{
	public:
		DebugSystem();
		~DebugSystem();

		/// <summary>
		/// ����� �ý����� �ʱ�ȭ�մϴ� 
		/// </summary>
		void Initialize(GameProcess* game);

		/// <summary>
		/// ����� ������ ������ �մϴ� 
		/// </summary>
		void Render();

		/// <summary>
		/// �׸��带 �����մϴ� 
		/// </summary>
		void SetGrid(fq::graphics::debug::GridInfo info);

		void RenderPointLight(fq::game_module::Light& light);
		void RenderDirLight(fq::game_module::Light& light);
		void RenderSpotLight(fq::game_module::Light& light);
		void RenderBoxCollier(fq::game_module::Transform& transform, fq::game_module::BoxCollider& collider);
		void RenderSphereCollier(fq::game_module::Transform& transform, fq::game_module::SphereCollider& collider);
		void RenderCapsuleCollier(fq::game_module::Transform& transform, fq::game_module::CapsuleCollider& collider);
		void RenderCharaterController(fq::game_module::Transform& transform, fq::game_module::CharacterController& cotroller);

		bool& GetOnGrid() { return mbOnGrid; }
		bool& GetOnBoxCollider() { return mbOnBoxCollider; }
		bool& GetOnCapsuleCollider() { return mbOnCapsuleCollider; }
		bool& GetOnSphereCollider() { return mbOnSphereCollider; }
		bool& GetOnConvexMeshCollider() { return mbOnConvexMeshCollider; }
		bool& GetOnCharaterController() { return mbOnCharaterController; }

	private:
		void renderGrid();
		void renderSphereCollider();
		void renderBoxCollider();
		void renderCapsuleCollider();
		void renderConvexMeshCollider();
		void renderCharaterController();

	private:
		GameProcess* mGameProcess;
		fq::game_module::Scene* mScene;

		bool mbOnLight;
		bool mbOnGrid;
		bool mbOnBoxCollider;
		bool mbOnCapsuleCollider;
		bool mbOnSphereCollider;
		bool mbOnConvexMeshCollider;
		bool mbOnCharaterController;

		fq::graphics::debug::GridInfo mGridInfo;
	};
}