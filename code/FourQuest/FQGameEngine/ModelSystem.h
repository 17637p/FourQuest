#pragma once

#include <filesystem>

#include "../FQCommon/FQCommonGraphics.h"

namespace fq::graphics { class IStaticMeshObject; }

namespace fq::game_engine
{
	class GameProcess;

	class ModelSystem
	{
	public:
		ModelSystem();
		~ModelSystem();

		void Initialize(GameProcess* game);

		void Render();


		/// <summary>
		/// 모델 생성하고 게임씬에 추가합니다
		/// </summary>
		void BuildModel(const std::filesystem::path& path);

	private:

		GameProcess* mGameProcess;

		std::vector<fq::graphics::IStaticMeshObject*> mStaticMeshObjects;
	};


}