#pragma once

#include <filesystem>

#include "../FQCommon/FQCommonGraphics.h"
#include "../FQCommon/FQCommonLoader.h"

namespace fq::graphics { class IStaticMeshObject; }

namespace fq::game_engine
{
	class GameProcess;
	class EditorProcess;

	class ModelSystem
	{
	public:
		ModelSystem();
		~ModelSystem();

		void Initialize(GameProcess* game, EditorProcess* editor);

		/// <summary>
		/// 모델 생성하고 게임씬에 추가합니다
		/// </summary>
		void BuildModel(const std::filesystem::path& path);

		/// <summary>
		/// 모델이 소유하는 메쉬를 반환합니다 
		/// </summary>
		static const fq::common::Mesh& GetMesh(const fq::common::Model& model,const std::string& meshName);

	private:
		GameProcess* mGameProcess;
		EditorProcess* mEditorProcess;
	};


}