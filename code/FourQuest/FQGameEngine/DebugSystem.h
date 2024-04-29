#pragma once

#include "../FQCommon/FQCommonGraphics.h"
#include "../FQGameModule/GameModule.h"

namespace fq::game_engine
{
	class GameProcess;

	/// <summary>
	/// 디버그 랜더링 관련 정보를 처리하는 시스템 
	/// </summary>
	class DebugSystem
	{
	public:
		DebugSystem();
		~DebugSystem();

		/// <summary>
		/// 디버그 시스템을 초기화합니다 
		/// </summary>
		void Initialize(GameProcess* game);

		/// <summary>
		/// 디버그 정보를 랜더링 합니다 
		/// </summary>
		void Render();

		/// <summary>
		/// 그리드를 설정합니다 
		/// </summary>
		void SetGrid(fq::graphics::debug::GridInfo info);

		void RenderCamera(fq::game_module::Transform& transform, fq::game_module::Camera& camera);
		void RenderPointLight(fq::game_module::Light& light);
		void RenderDirLight(fq::game_module::Light& light);
		void RenderSpotLight(fq::game_module::Light& light);

		void RenderBoxCollier(fq::game_module::Transform& transform, fq::game_module::BoxCollider& collider);

	private:
		void renderGrid();
		void renderLight();
		void renderCamera();
		void renderBoxCollider();

	private:
		GameProcess* mGameProcess;
		fq::game_module::Scene* mScene;

		bool mbOnLight;
		bool mbOnGrid;
		fq::graphics::debug::GridInfo mGridInfo;

	};
}