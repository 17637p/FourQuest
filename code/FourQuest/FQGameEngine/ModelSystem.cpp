#include "ModelSystem.h"

#include <memory>

#include "../FQCommon/FQPath.h"
#include "../FQGraphics/IFQGraphics.h"
#include "../FQCommon/IFQRenderObject.h"
#include "../FQGameModule/GameModule.h"
#include "../FQGameModule/StaticMeshRenderer.h"
#include "../FQGameModule/SkinnedMeshRenderer.h"
#include "../FQGameModule/Transform.h"

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
	auto key = mGameProcess->mRenderingSystem->GetModelKey(path.string(), texturePath.string());

	auto& graphics = mGameProcess->mGraphics;
	const auto& model = graphics->CreateModelResource(key, modelPath.string(), texturePath);

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

		// 재질 경로
		std::vector<std::string> materialPaths;
		materialPaths.reserve(mesh.Subsets.size());
		fq::graphics::MeshObjectInfo meshObjectInfo;
		std::filesystem::path directory = path.parent_path();

		for (const auto& subset : mesh.Subsets)
		{
			const std::filesystem::path materialPath  = (directory / subset.MaterialName).string() + ".material";
			
			if (!std::filesystem::exists(materialPath))
			{
				const std::string DEFAULT_MATERIAL = "./resource/Material/Default.material";
				materialPaths.push_back(DEFAULT_MATERIAL);
			}
			else
			{
				materialPaths.push_back(materialPath.string());
			}
		}

		// StaticMeshObject 생성
		if (mesh.BoneVertices.empty())
		{
			auto& staticMeshRenderer = nodeObject->AddComponent<fq::game_module::StaticMeshRenderer>();
			//std::shared_ptr < fq::graphics::IStaticMesh> meshInterface = mGameProcess->mGraphics->GetStaticMeshByModelPathOrNull(modelPath.string(), mesh.Name);
			//fq::graphics::IStaticMeshObject* iStaticMeshObject = mGameProcess->mGraphics->CreateStaticMeshObject(meshInterface, materialInterfaces, meshObjectInfo, DirectX::SimpleMath::Matrix::Identity);
			//staticMeshRenderer.SetStaticMeshObject(iStaticMeshObject);
			staticMeshRenderer.SetModelPath(modelPath.string());
			staticMeshRenderer.SetMaterialPaths(materialPaths);
			staticMeshRenderer.SetMeshName(mesh.Name);
			staticMeshRenderer.SetTexturePath(texturePath.string());
		}
		else // SkinnedMeshObject 생성
		{
			auto& skinnedMeshRenderer = nodeObject->AddComponent<fq::game_module::SkinnedMeshRenderer>();
			//std::shared_ptr<fq::graphics::ISkinnedMesh> meshInterface = mGameProcess->mGraphics->GetSkinnedMeshByModelPathOrNull(modelPath.string(), mesh.Name);
			//fq::graphics::ISkinnedMeshObject* iSkinnedMeshObject = mGameProcess->mGraphics->CreateSkinnedMeshObject(meshInterface, materialInterfaces, meshObjectInfo, DirectX::SimpleMath::Matrix::Identity);
			//iSkinnedMeshObject->SetNodeHierarchyInstance(boneHierarchyCache);
			//skinnedMeshRenderer.SetSkinnedMeshObject(iSkinnedMeshObject);
			skinnedMeshRenderer.SetModelPath(modelPath.string());
			skinnedMeshRenderer.SetMaterialPaths(materialPaths);
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
