#pragma once

#include <array>

#include "../FQCommon/FQCommonGraphics.h"
#include "../FQGameModule/GameModuleEnum.h"

namespace fq::game_module
{
	class Transform;
	class BoxCollider;
	class CapsuleCollider;
	class SphereCollider;
	class CharacterController;
	class Controller;
	class Scene;
	class Light;
}

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
		void RenderCharaterController(fq::game_module::Transform& transform, fq::game_module::Controller& cotroller);

		bool& GetOnGrid() { return mbOnGrid; }
		bool& GetOnBoxCollider() { return mbOnBoxCollider; }
		bool& GetOnCapsuleCollider() { return mbOnCapsuleCollider; }
		bool& GetOnSphereCollider() { return mbOnSphereCollider; }
		bool& GetOnConvexMeshCollider() { return mbOnConvexMeshCollider; }
		bool& GetOnCharaterController() { return mbOnCharaterController; }
		bool& GetOnNavigationMesh() { return mbOnNavigationMesh; }
		bool& GetOnTerrainCollider() { return mbOnTerrainCollider; }
		bool& GetOnUseRenderDebug() { return mbUseRenderDebug; }

		std::array<bool, static_cast<int>(game_module::ETag::End)>& GetColliderTag() { return mbUseColliderTag; }

	private:
		void renderGrid();
		void renderSphereCollider();
		void renderBoxCollider();
		void renderCapsuleCollider();
		void renderConvexMeshCollider();
		void renderTerrainCollider();
		void renderCharaterController();
		void renderNavigationMesh();

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
		bool mbOnTerrainCollider;
		bool mbOnNavigationMesh;
		bool mbUseRenderDebug;

		std::array<bool, static_cast<int>(game_module::ETag::End)> mbUseColliderTag;

		fq::graphics::debug::GridInfo mGridInfo;
	};
}