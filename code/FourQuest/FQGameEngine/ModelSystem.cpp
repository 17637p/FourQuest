#include "ModelSystem.h"

#include <memory>

#include "../FQGraphics/IFQGraphics.h"
#include "../FQCommon/IFQRenderObject.h"
#include "../FQGameModule/GameModule.h"

#include "EditorProcess.h"
#include "GameProcess.h"

fq::game_engine::ModelSystem::ModelSystem()
	:mGameProcess(nullptr)
	, mEditorProcess(nullptr)
{}

fq::game_engine::ModelSystem::~ModelSystem()
{}

void fq::game_engine::ModelSystem::Initialize(GameProcess* game, EditorProcess* editor)
{
	mGameProcess = game;
	mEditorProcess = editor;
}

void fq::game_engine::ModelSystem::BuildModel(const std::filesystem::path& path)
{
	std::filesystem::path texturePath = path.parent_path();
	std::filesystem::path modelPath = path;

	auto& graphics = mGameProcess->mGraphics;
	const auto& model = graphics->CreateModel(modelPath.string(), texturePath);

	std::vector<std::shared_ptr<fq::game_module::GameObject>> modelObjects;

	// StaticMesh 생성
	for (const auto& [node, mesh] : model.Meshes)
	{
		// 게임오브젝트 생성
		auto nodeObject = std::make_shared<fq::game_module::GameObject>();

		if (modelObjects.empty())
		{
			std::string modelName = modelPath.filename().string();
			modelName = modelName.substr(0, modelName.size() - 6);
			nodeObject->SetName(modelName);
		}
		else
		{
			nodeObject->SetName(node.Name);
		}
		modelObjects.push_back(nodeObject);

		// 트랜스폼 설정
		auto nodeObjectT = nodeObject->GetComponent<fq::game_module::Transform>();
		nodeObjectT->SetLocalMatrix(node.ToParentMatrix);

		// 부모가 존재하는 경우
		if (fq::common::Node::INVALID_INDEX != node.ParentIndex)
		{
			auto& parent = modelObjects[node.ParentIndex];
			auto parentT = parent->GetComponent<fq::game_module::Transform>();

			parentT->AddChild(nodeObjectT);
		}

		// 메쉬가 없는 노드
		if (mesh.Vertices.empty())
		{
			continue;
		}

		// 메쉬 생성
		fq::graphics::MeshObjectInfo meshInfo;
		meshInfo.ModelPath = modelPath.string();
		meshInfo.MeshName = mesh.Name;
		meshInfo.Transform = node.ToParentMatrix;

		for (const auto& subset : mesh.Subsets)
		{
			meshInfo.MaterialNames.push_back(subset.MaterialName);
		}

		// StaticMesh 생성
		if (mesh.BoneVertices.empty())
		{
			auto& staticMeshRenderer
				= nodeObject->AddComponent<fq::game_module::StaticMeshRenderer>();
			staticMeshRenderer.SetMeshObjectInfomation(meshInfo);
		}
		else // SkinnedMesh 생성
		{
			auto& skinnedMeshRenderer
				= nodeObject->AddComponent<fq::game_module::SkinnedMeshRenderer>();
			skinnedMeshRenderer.SetMeshObjectInfomation(meshInfo);
		}
	}

	assert(!modelObjects.empty());

	mEditorProcess->mCommandSystem->Push<AddObjectCommand>(
		mGameProcess->mSceneManager->GetCurrentScene()
		, modelObjects[0]);
}
