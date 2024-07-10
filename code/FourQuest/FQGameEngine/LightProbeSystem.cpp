#include "LightProbeSystem.h"

#include "GameProcess.h"
#include "../FQGraphics/IFQGraphics.h"
#include "../FQCommon/IFQRenderObject.h"

fq::game_engine::LightProbeSystem::LightProbeSystem(GameProcess* tempProcess)
{
	mGameProcess = tempProcess;
}

fq::game_engine::LightProbeSystem::~LightProbeSystem()
{
}

void fq::game_engine::LightProbeSystem::AddLightProbe(DirectX::SimpleMath::Vector3 position)
{
	LightProbe* lightProbe = new LightProbe;

	lightProbe->position = position;
	lightProbe->index = mGameProcess->mGraphics->AddLightProbe(position);

	// Probe Object ¸¸µé±â
	const std::string sphereModel = "./resource/internal/lightProbe/sphere.model";
	const fq::common::Model& modelData = mGameProcess->mGraphics->CreateModelResource(sphereModel, "./resource/");
	for (const auto& [node, mesh] : modelData.Meshes)
	{
		if (mesh.Vertices.empty())
		{
			continue;
		}

		auto meshInterface = mGameProcess->mGraphics->GetStaticMeshByModelPathOrNull(sphereModel, mesh.Name);

		DirectX::SimpleMath::Matrix transform = DirectX::SimpleMath::Matrix::CreateScale({ 0.001f, 0.001f, 0.001f }) * 
			DirectX::SimpleMath::Matrix::CreateTranslation({ position.x, position.y, position.z });
		fq::graphics::IProbeObject* iProbeObject = mGameProcess->mGraphics->CreateProbeObject(meshInterface, node.ToParentMatrix * transform, lightProbe->index);
		mProbeObjects.push_back(iProbeObject);
	}

	mLightProbes.emplace_back(lightProbe);
}

void fq::game_engine::LightProbeSystem::DeleteLightProbe(int index)
{
	mGameProcess->mGraphics->DeleteLightProbe(mLightProbes[index]->index);

	delete mLightProbes[index];
	mLightProbes.erase(mLightProbes.begin() + index);
}

