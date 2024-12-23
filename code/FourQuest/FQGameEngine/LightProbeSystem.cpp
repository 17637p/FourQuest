#include "LightProbeSystem.h"

#include "GameProcess.h"
#include "../FQGraphics/IFQGraphics.h"
#include "../FQCommon/IFQRenderObject.h"

#include "RenderingSystem.h"

#include <fstream>

fq::game_engine::LightProbeSystem::LightProbeSystem(GameProcess* tempProcess)
	:mLightProbeDefaultScale(0.001f),
	mLightProbeScale(1)
{
	mGameProcess = tempProcess;
}

fq::game_engine::LightProbeSystem::~LightProbeSystem()
{
}

void fq::game_engine::LightProbeSystem::AddLightProbe(DirectX::SimpleMath::Vector3 position)
{
	int index = mGameProcess->mGraphics->AddLightProbe(position);

	// Probe Object 만들기
	const std::string sphereModel = "./resource/internal/lightProbe/sphere.model";
	const std::string texturePath = "./resource/";
	auto key = mGameProcess->mRenderingSystem->GetModelKey(sphereModel, texturePath);

	const fq::common::Model& modelData = mGameProcess->mGraphics->CreateModelResource( key,sphereModel, texturePath);
	for (const auto& [node, mesh] : modelData.Meshes)
	{
		if (mesh.Vertices.empty())
		{
			continue;
		}

		auto meshInterface = mGameProcess->mGraphics->GetStaticMeshByModelPathOrNull( key, mesh.Name);

		DirectX::SimpleMath::Matrix transform = DirectX::SimpleMath::Matrix::CreateScale({ 
			mLightProbeDefaultScale * mLightProbeScale, 
			mLightProbeDefaultScale * mLightProbeScale,
			mLightProbeDefaultScale * mLightProbeScale }) *
			DirectX::SimpleMath::Matrix::CreateTranslation({ position.x, position.y, position.z });
		fq::graphics::IProbeObject* iProbeObject = mGameProcess->mGraphics->CreateProbeObject(meshInterface, node.ToParentMatrix * transform, index);
		mProbeObjects.push_back(iProbeObject);
	}
}

void fq::game_engine::LightProbeSystem::DeleteLightProbe(fq::graphics::IProbeObject* probeObject)
{
	mGameProcess->mGraphics->DeleteLightProbe(probeObject->GetIndex());

	mGameProcess->mGraphics->DeleteProbeObject(probeObject);
	mProbeObjects.erase(std::remove(mProbeObjects.begin(), mProbeObjects.end(), probeObject));
}

void fq::game_engine::LightProbeSystem::BakeLightProbe(bool isAll)
{
	mGameProcess->mGraphics->BakeLightProbe(isAll);
}

void fq::game_engine::LightProbeSystem::SaveProbeTexture(bool isAll, int width, int height)
{
	mGameProcess->mGraphics->SaveCubeProbeTexture(isAll, width, height);
}

void fq::game_engine::LightProbeSystem::SaveLightProbes(std::string fileName)
{
	mGameProcess->mGraphics->SaveLightProbes(fileName);

	std::string line;
	std::ofstream lightProbeFile("./resource/LightProbe/" + fileName + ".Lpo");
	if (lightProbeFile.is_open())
	{
		// LightProbeSize
		lightProbeFile << mProbeObjects.size() << "\n";

		// Write LightProbe
		for (int i = 0; i < mProbeObjects.size(); i++)
		{
			lightProbeFile << mProbeObjects[i]->GetIndex() << "\n";

			// Matrix
			for (int j = 0; j < 4; j++)
			{
				for (int k = 0; k < 4; k++)
				{
					lightProbeFile << mProbeObjects[i]->GetTransform().m[j][k] << " ";
				}
			}

			lightProbeFile << "\n";
		}

		lightProbeFile.close();
	}
	else
	{
		// 파일 오픈 불가
		//spdlog::error("NavMesh File Can't write");
	}

	// 성공
	spdlog::info("LightProbe Save Successful");
}

void fq::game_engine::LightProbeSystem::LoadLightProbes(std::string fileName)
{
	Clear();

	mGameProcess->mGraphics->LoadLightProbes(fileName);

	std::string line;
	std::ifstream lightProbeFile("./resource/LightProbe/" + fileName + ".Lpo");

	const std::string sphereModel = "./resource/internal/lightProbe/sphere.model";
	const std::string texturePath = "./resource/";
	auto key = mGameProcess->mRenderingSystem->GetModelKey(sphereModel, texturePath);
	const fq::common::Model& modelData = mGameProcess->mGraphics->CreateModelResource( key,sphereModel, texturePath);

	if (lightProbeFile.is_open())
	{
		// LightProbeSize
		int probeObjectSize = -1;
		lightProbeFile >> probeObjectSize;

		// Write LightProbe
		for (int i = 0; i < probeObjectSize; i++)
		{
			int index = -1;
			lightProbeFile >> index;

			DirectX::SimpleMath::Matrix mat{};
			// Matrix
			for (int j = 0; j < 4; j++)
			{
				for (int k = 0; k < 4; k++)
				{
					lightProbeFile >> mat.m[j][k];
				}
			}

			for (const auto& [node, mesh] : modelData.Meshes)
			{
				if (mesh.Vertices.empty())
				{
					continue;
				}

				auto meshInterface = mGameProcess->mGraphics->GetStaticMeshByModelPathOrNull(key, mesh.Name);

				fq::graphics::IProbeObject* iProbeObject = mGameProcess->mGraphics->CreateProbeObject(meshInterface, mat, index);
				mProbeObjects.push_back(iProbeObject);
			}
		}

		lightProbeFile.close();
	}

	spdlog::info("LightProbe Load Successful");
}

void fq::game_engine::LightProbeSystem::Clear()
{
	for (int i = 0; i < mProbeObjects.size(); i++)
	{
		mGameProcess->mGraphics->DeleteProbeObject(mProbeObjects[i]);
	}
	mProbeObjects.clear();
}

void fq::game_engine::LightProbeSystem::Finalize()
{
	Clear();
}

