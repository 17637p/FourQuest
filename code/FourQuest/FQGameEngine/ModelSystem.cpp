#include "ModelSystem.h"

#include <memory>

#include "../FQGraphics/IFQGraphics.h"
#include "../FQCommon/IFQRenderObject.h"
#include "../FQGameModule/GameModule.h"

#include "GameProcess.h"

fq::game_engine::ModelSystem::ModelSystem()
	:mGameProcess(nullptr)
{}

fq::game_engine::ModelSystem::~ModelSystem()
{}

void fq::game_engine::ModelSystem::Initialize(GameProcess* game)
{
	mGameProcess = game;

	auto& graphics =  mGameProcess->mGraphics;
}


void fq::game_engine::ModelSystem::BuildModel(const std::filesystem::path& path)
{
	std::filesystem::path texturePath = path.parent_path();
	std::filesystem::path modelPath = path / path.filename();

	auto& graphics =  mGameProcess->mGraphics;
	const auto& model =  graphics->CreateModel(modelPath.string(), texturePath);

	std::vector<std::shared_ptr<fq::game_module::GameObject>> modelObjects;

	// StaticMesh ����
	for (const auto& [node ,mesh]:model.Meshes)
	{
		// ���ӿ�����Ʈ ����
		auto nodeObject = std::make_shared<fq::game_module::GameObject>();
		
		nodeObject->SetName(node.Name);
		modelObjects.push_back(nodeObject);

		// Ʈ������ ����
		auto nodeObjectT = nodeObject->GetComponent<fq::game_module::Transform>();
		nodeObjectT->SetLocalMatrix(node.ToParentMatrix);

		// �θ� �����ϴ� ���
		if (fq::common::Node::INVALID_INDEX != node.Parentindex)
		{
		 	auto& parent = modelObjects[node.Parentindex];
			auto parentT = parent->GetComponent<fq::game_module::Transform>();

			parentT->AddChild(nodeObjectT);
		}

		// �޽��� ���� ���
		if (mesh.Vertices.empty())
		{
			continue;
		}

		// �޽� ����
		fq::graphics::MeshObjectInfo meshInfo;
		meshInfo.ModelPath = modelPath.string();
		meshInfo.MeshName = mesh.Name;
		meshInfo.Transform = node.ToParentMatrix;

		for (const auto& subset : mesh.Subsets)
		{
			meshInfo.MaterialNames.push_back(subset.MaterialName);
		}

		// StaticMesh ����
		if (mesh.BoneVertices.empty())
		{
			auto& staticMeshRenderer
				= nodeObject->AddComponent<fq::game_module::StaticMeshRenderer>();

			staticMeshRenderer.SetMeshObjectInfomation(meshInfo);

			auto meshObject = graphics->CreateStaticMeshObject(meshInfo);
			staticMeshRenderer.SetStaticMeshObject(meshObject);
			meshObject->UpdateTransform(nodeObjectT->GetLocalMatrix());
		}
		else // SkinnedMesh ����
 		{

		}
	}

	assert(!modelObjects.empty());

	mGameProcess->mSceneManager->GetCurrentScene()
		->AddGameObject(modelObjects[0]);
}
