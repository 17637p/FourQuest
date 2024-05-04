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
		/// �� �����ϰ� ���Ӿ��� �߰��մϴ�
		/// </summary>
		void BuildModel(const std::filesystem::path& path);

		/// <summary>
		/// ���� �����ϴ� �޽��� ��ȯ�մϴ� 
		/// </summary>
		static const fq::common::Mesh& GetMesh(const fq::common::Model& model,const std::string& meshName);

	private:
		GameProcess* mGameProcess;
		EditorProcess* mEditorProcess;
	};


}