#include "LightmapWindow.h"

#include <imgui.h>
#include "imgui_stdlib.h"

#include "../FQCommon/FQPath.h"
#include "../FQGraphics/IFQGraphics.h"
#include "GameProcess.h"

fq::game_engine::LightmapWindow::LightmapWindow()
	:mGraphicsEngine(nullptr)
	, mbIsOpen(false)
	, mSkyBoxInfo{}
	, mGameProcess(nullptr)
{}

fq::game_engine::LightmapWindow::~LightmapWindow()
{

}

void fq::game_engine::LightmapWindow::Initialize(GameProcess* game)
{
	mGameProcess = game;
	mGraphicsEngine = game->mGraphics;

	// SkyBox 정보로드
	mOnLoadSceneHandler = game->mEventManager->RegisterHandle<fq::event::OnLoadScene>(
		[this]()
		{
			auto scenePath = fq::path::GetScenePath() / mGameProcess->mSceneManager->GetCurrentScene()->GetSceneName();

			if (std::filesystem::exists(scenePath))
			{
				LoadLightmap(scenePath);
			}
		}
	);

}

void fq::game_engine::LightmapWindow::Render()
{
	if (!mbIsOpen) return;

	if (ImGui::Begin("Lightmap", &mbIsOpen))
	{
		unsigned int index = 0;

		for (std::filesystem::path& path : mLightmapArrayPath)
		{
			std::string stringPath = path.string();
			std::string label = std::string("LightMap") + std::to_string(index);

			ImGui::InputText(label.c_str(), &stringPath);

			if (ImGui::BeginDragDropTarget())
			{
				const ImGuiPayload* pathPayLoad = ImGui::AcceptDragDropPayload("Path");

				if (pathPayLoad)
				{
					std::filesystem::path* inputPath
						= static_cast<std::filesystem::path*>(pathPayLoad->Data);

					if (inputPath->extension() == ".dds") // 모델 생성
					{
						path = *inputPath;
					}
				}
			}
		}

		if (ImGui::Button("AddLightMap", ImVec2{ 133,25 }))
		{
			mLightmapArrayPath.push_back({});
		}
		if (ImGui::Button("DeleteLightMap", ImVec2{ 133,25 }))
		{
			mLightmapArrayPath.pop_back();
		}
		if (ImGui::Button("ClearLightMap", ImVec2{ 133,25 }))
		{
			mLightmapArrayPath.clear();
		}

		index = 0;

		for (std::filesystem::path& path : mDirectionArrayPath)
		{
			std::string stringPath = path.string();
			std::string label = std::string("DirectionMap") + std::to_string(index);

			ImGui::InputText(label.c_str(), &stringPath);

			if (ImGui::BeginDragDropTarget())
			{
				const ImGuiPayload* pathPayLoad = ImGui::AcceptDragDropPayload("Path");

				if (pathPayLoad)
				{
					std::filesystem::path* inputPath
						= static_cast<std::filesystem::path*>(pathPayLoad->Data);

					if (inputPath->extension() == ".dds" || inputPath->extension() == ".png") // 모델 생성
					{
						path = *inputPath;
					}
				}
			}
		}

		if (ImGui::Button("AddDirectionMap", ImVec2{ 133,25 }))
		{
			mDirectionArrayPath.push_back({});
		}
		if (ImGui::Button("DeleteDirectionMap", ImVec2{ 133,25 }))
		{
			mDirectionArrayPath.pop_back();
		}
		if (ImGui::Button("ClearDirectionMap", ImVec2{ 133,25 }))
		{
			mDirectionArrayPath.clear();
		}

		if (ImGui::Button("Apply", ImVec2{ 133,25 }))
		{
			mGameProcess->mGraphics->SetLightMapTexture(mLightmapArrayPath);
			mGameProcess->mGraphics->SetLightMapDirectionTexture(mDirectionArrayPath);
		}
	}

	ImGui::End();
}

void fq::game_engine::LightmapWindow::SaveLightmap(std::filesystem::path path)
{
	nlohmann::json lightmapJson;

	for (const std::filesystem::path texturePath : mLightmapArrayPath)
	{
		lightmapJson["Lightmap"].push_back(texturePath);
	}
	for (const std::filesystem::path texturePath : mDirectionArrayPath)
	{
		lightmapJson["Directionmap"].push_back(texturePath);
	}

	std::filesystem::path filePath = path / "lightmap.txt";

	std::ofstream output(filePath);

	assert(output.is_open());

	if (output.is_open())
	{
		output << std::setw(4) << lightmapJson;
		output.close();
	}
	else
	{
		assert(nullptr);
	}
}

void fq::game_engine::LightmapWindow::LoadLightmap(std::filesystem::path path)
{
	auto filePath = path / "lightmap.txt";

	std::ifstream readData(filePath);
	nlohmann::json lightmapJson;

	if (readData.is_open())
	{
		readData >> lightmapJson;
		readData.close();
	}
	else
	{
		assert(!"파일 열기 실패");
	}

	for (auto& element : lightmapJson["Lightmap"])
	{
		mLightmapArrayPath.push_back(element);
	}
	for (auto& element : lightmapJson["Directionmap"])
	{
		mDirectionArrayPath.push_back(element);
	}
}
