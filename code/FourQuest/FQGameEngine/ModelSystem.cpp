#include "ModelSystem.h"

#include "../FQGraphics/IFQGraphics.h"
#include "../FQCommon/IFQRenderObject.h"

#include "GameProcess.h"

fq::game_engine::ModelSystem::ModelSystem()
	:mGameProcess(nullptr)
{
}

fq::game_engine::ModelSystem::~ModelSystem()
{

}

void fq::game_engine::ModelSystem::Initialize(GameProcess* game)
{
	mGameProcess = game;

	auto& graphics =  mGameProcess->mGraphics;

	const std::string modelPath = "./resource/example/gun/gun";

	//graphics->ConvertModel("./resource/example/gun.fbx", "./resource/example/gun/gun");
	const auto& model = graphics->CreateModel(modelPath, "./resource/example");

	for (const auto& mesh : model.Meshes)
	{
		if (mesh.second.Vertices.empty())
		{
			continue;
		}

		fq::graphics::MeshObjectInfo meshInfo;
		meshInfo.ModelPath = modelPath;
		meshInfo.MeshName = mesh.second.Name;
		meshInfo.Transform = mesh.first.ToParentMatrix;

		for (auto subset : mesh.second.Subsets)
		{
			meshInfo.MaterialNames.push_back(subset.MaterialName);
		}

		if (mesh.second.BoneVertices.empty())
		{
			mStaticMeshObjects.push_back(graphics->CreateStaticMeshObject(meshInfo));
			mStaticMeshObjects.back()->UpdateTransform(mStaticMeshObjects.back()->GetTransform() * DirectX::SimpleMath::Matrix::CreateRotationY(3.14 * 1.5f));
		}
		else
		{
			graphics->CreateSkinnedMeshObject(meshInfo);
		}
	}
	graphics->DeleteModel("./resource/example/gun/gun");
}

void fq::game_engine::ModelSystem::Render()
{
	for (auto& obj : mStaticMeshObjects)
	{
		obj->UpdateTransform(obj->GetTransform() * DirectX::SimpleMath::Matrix::CreateRotationY(0.0001f));
	}
}
