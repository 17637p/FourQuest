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
#include "ResourceSystem.h"

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
	if (!std::filesystem::exists(path))
	{
		//spdlog::warn("[RenderingSystem] Model Path \"{}\" does not exist", path);
		return;
	}

	if (!mGameProcess->mResourceSystem->HasModel(path.string()))
	{
		mGameProcess->mResourceSystem->LoadModelResource(path.string());
	}

	const fq::common::Model& model = mGameProcess->mResourceSystem->GetModel(path.string());

	std::vector<std::shared_ptr<fq::game_module::GameObject>> modelObjects;

	// StaticMesh 생성
	for (const auto& [node, mesh] : model.Meshes)
	{
		// 게임오브젝트 생성
		auto nodeObject = std::make_shared<fq::game_module::GameObject>();

		if (modelObjects.empty())
		{
			std::string modelName = path.filename().string();
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
		std::filesystem::path parentDirectory = path.parent_path();
		std::filesystem::path directory = path;
		directory.replace_extension("");

		// 같은 디렉토리나 모델 이름으로 생성된 폴더가 있으면 확인 후 경로 지정
		for (const auto& subset : mesh.Subsets)
		{
			const std::filesystem::path materialPath0 = (parentDirectory / subset.MaterialName).string() + ".material";
			const std::filesystem::path materialPath1 = (directory / subset.MaterialName).string() + ".material";

			if (std::filesystem::exists(materialPath0))
			{
				materialPaths.push_back(materialPath0.string());
			}
			else if (std::filesystem::exists(materialPath1))
			{
				materialPaths.push_back(materialPath1.string());
			}
			else
			{
				const std::string DEFAULT_MATERIAL = "./resource/Material/Default.material";
				materialPaths.push_back(DEFAULT_MATERIAL);
			}
		}

		// StaticMeshObject 생성
		if (mesh.BoneVertices.empty())
		{
			auto& staticMeshRenderer = nodeObject->AddComponent<fq::game_module::StaticMeshRenderer>();
			staticMeshRenderer.SetModelPath(path.string());
			staticMeshRenderer.SetMaterialPaths(materialPaths);
			staticMeshRenderer.SetMeshName(mesh.Name);
		}
		else // SkinnedMeshObject 생성
		{
			auto& skinnedMeshRenderer = nodeObject->AddComponent<fq::game_module::SkinnedMeshRenderer>();
			skinnedMeshRenderer.SetModelPath(path.string());
			skinnedMeshRenderer.SetMaterialPaths(materialPaths);
			skinnedMeshRenderer.SetMeshName(mesh.Name);
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
