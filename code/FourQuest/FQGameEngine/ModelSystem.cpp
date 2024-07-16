#include "ModelSystem.h"

#include <memory>

#include "../FQCommon/FQPath.h"
#include "../FQGraphics/IFQGraphics.h"
#include "../FQCommon/IFQRenderObject.h"
#include "../FQGameModule/GameModule.h"

#include "EditorProcess.h"
#include "GameProcess.h"
#include "RenderingSystem.h"

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
	std::filesystem::path modelPath = path;
	std::filesystem::path texturePath = fq::path::GetResourcePath() / "Texture";

	auto& graphics = mGameProcess->mGraphics;
	const auto& model = graphics->CreateModelResource(modelPath.string(), texturePath);

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

		// ���� ������ ����
		std::vector<std::string> materialNames;
		materialNames.reserve(mesh.Subsets.size());
		fq::graphics::MeshObjectInfo meshObjectInfo;

		for (const auto& subset : mesh.Subsets)
		{
			materialNames.push_back(subset.MaterialName);
		}

		// StaticMeshObject ����
		if (mesh.BoneVertices.empty())
		{
			auto& staticMeshRenderer = nodeObject->AddComponent<fq::game_module::StaticMeshRenderer>();
			//std::shared_ptr < fq::graphics::IStaticMesh> meshInterface = mGameProcess->mGraphics->GetStaticMeshByModelPathOrNull(modelPath.string(), mesh.Name);
			//fq::graphics::IStaticMeshObject* iStaticMeshObject = mGameProcess->mGraphics->CreateStaticMeshObject(meshInterface, materialInterfaces, meshObjectInfo, DirectX::SimpleMath::Matrix::Identity);
			//staticMeshRenderer.SetStaticMeshObject(iStaticMeshObject);
			staticMeshRenderer.SetModelPath(modelPath.string());
			staticMeshRenderer.SetMaterials(materialNames);
			staticMeshRenderer.SetMeshName(mesh.Name);
			staticMeshRenderer.SetTexturePath(texturePath.string());
		}
		else // SkinnedMeshObject ����
		{
			auto& skinnedMeshRenderer = nodeObject->AddComponent<fq::game_module::SkinnedMeshRenderer>();
			//std::shared_ptr<fq::graphics::ISkinnedMesh> meshInterface = mGameProcess->mGraphics->GetSkinnedMeshByModelPathOrNull(modelPath.string(), mesh.Name);
			//fq::graphics::ISkinnedMeshObject* iSkinnedMeshObject = mGameProcess->mGraphics->CreateSkinnedMeshObject(meshInterface, materialInterfaces, meshObjectInfo, DirectX::SimpleMath::Matrix::Identity);
			//iSkinnedMeshObject->SetNodeHierarchyInstance(boneHierarchyCache);
			//skinnedMeshRenderer.SetSkinnedMeshObject(iSkinnedMeshObject);
			skinnedMeshRenderer.SetModelPath(modelPath.string());
			skinnedMeshRenderer.SetMaterials(materialNames);
			skinnedMeshRenderer.SetMeshName(mesh.Name);
			skinnedMeshRenderer.SetTexturePath(texturePath.string());
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

	auto fileList = fq::path::GetFileListRecursive(resPath);

	for (auto& file : fileList)
	{
		if (file.extension() == ".fbx")
		{
			std::wstring fileName = file.filename();
			fileName = fileName.substr(0, fileName.size() - 4);

			auto directory = file.parent_path() / fileName;
			auto modelData = mGameProcess->mGraphics->ConvertModel(file.string());
			mGameProcess->mGraphics->WriteModel(directory.string(), modelData);
		}
	}
}
