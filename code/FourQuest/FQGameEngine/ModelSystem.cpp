#include "ModelSystem.h"

#include <memory>

#include "../FQCommon/FQPath.h"
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

	// StaticMesh ����
	for (const auto& [node, mesh] : model.Meshes)
	{
		// ���ӿ�����Ʈ ����
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

		// Ʈ������ ����
		auto nodeObjectT = nodeObject->GetComponent<fq::game_module::Transform>();
		nodeObjectT->SetLocalMatrix(node.ToParentMatrix);

		// �θ� �����ϴ� ���
		if (fq::common::Node::INVALID_INDEX != node.ParentIndex)
		{
			auto& parent = modelObjects[node.ParentIndex];
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
		}
		else // SkinnedMesh ����
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

const fq::common::Mesh& fq::game_engine::ModelSystem::GetMesh(const fq::common::Model& model, const std::string& meshName)
{
	for (const auto& [node, mesh] : model.Meshes)
	{
		if (mesh.Name == meshName)
		{
			return mesh;
		}
	}

	return fq::common::Mesh{};
}

void fq::game_engine::ModelSystem::ConvertAllModel()
{
	auto resPath = fq::path::GetResourcePath();

	auto fileList=  fq::path::GetFileListRecursive(resPath);

	for (auto& file : fileList)
	{
		if (file.extension() == ".fbx")
		{
			std::wstring fileName = file.filename();
			fileName = fileName.substr(0, fileName.size() - 4);

			auto directory = file.parent_path() / fileName;
			mGameProcess->mGraphics->ConvertModel(file.string(), directory.string());
		}
	}
}
