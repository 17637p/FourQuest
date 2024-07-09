#include "LightProbeSystem.h"

#include "GameProcess.h"
#include "../FQGraphics/IFQGraphics.h"

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

	mLightProbes.emplace_back(lightProbe);
}

void fq::game_engine::LightProbeSystem::DeleteLightProbe(int index)
{
	mGameProcess->mGraphics->DeleteLightProbe(mLightProbes[index]->index);

	delete mLightProbes[index];
	mLightProbes.erase(mLightProbes.begin() + index);
}

