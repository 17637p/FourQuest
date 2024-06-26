#include "SkyBoxWindow.h"

#include <imgui.h>
#include "imgui_stdlib.h"

#include "../FQCommon/FQPath.h"
#include "../FQGraphics/IFQGraphics.h"
#include "GameProcess.h"

fq::game_engine::SkyBoxWindow::SkyBoxWindow()
	:mGraphicsEngine(nullptr)
	, mbIsOpen(false)
	, mSkyBoxInfo{}
	, mGameProcess(nullptr)
{}

fq::game_engine::SkyBoxWindow::~SkyBoxWindow()
{

}

void fq::game_engine::SkyBoxWindow::Initialize(GameProcess* game)
{
	mGameProcess = game;
	mGraphicsEngine = game->mGraphics;

	// SkyBox 沥焊肺靛
	mOnLoadSceneHandler = game->mEventManager->RegisterHandle<fq::event::OnLoadScene>(
		[this]()
		{
			auto scenePath = fq::path::GetScenePath() / mGameProcess->mSceneManager->GetCurrentScene()->GetSceneName();

			if (std::filesystem::exists(scenePath))
				mSkyBoxInfo.Load(scenePath);
		}
	);

}

void fq::game_engine::SkyBoxWindow::Render()
{
	if (!mbIsOpen) return;

	if (ImGui::Begin("SkyBox", &mbIsOpen))
	{
		// skybox
		std::string skyBox = mSkyBoxInfo.mSkyBox.string();
		ImGui::InputText("SkyBox", &skyBox);

		if (ImGui::BeginDragDropTarget())
		{
			const ImGuiPayload* pathPayLoad = ImGui::AcceptDragDropPayload("Path");

			if (pathPayLoad)
			{
				std::filesystem::path* path
					= static_cast<std::filesystem::path*>(pathPayLoad->Data);

				if (path->extension() == ".dds") // 葛胆 积己
				{
					mSkyBoxInfo.mSkyBox = *path;

					mGraphicsEngine->SetSkyBox(mSkyBoxInfo.mSkyBox);
				}
			}
		}

		// diffuse
		std::string diffuse = mSkyBoxInfo.mDiffuse.string();
		ImGui::InputText("Diffuse", &diffuse);

		if (ImGui::BeginDragDropTarget())
		{
			const ImGuiPayload* pathPayLoad = ImGui::AcceptDragDropPayload("Path");

			if (pathPayLoad)
			{
				std::filesystem::path* path
					= static_cast<std::filesystem::path*>(pathPayLoad->Data);

				if (path->extension() == ".dds") // 葛胆 积己
				{
					mSkyBoxInfo.mDiffuse = *path;

					mGraphicsEngine->SetIBLTexture(mSkyBoxInfo.mDiffuse
						, mSkyBoxInfo.mSpecular
						, mSkyBoxInfo.mBrdfLUT);
				}
			}
		}

		// specular
		std::string specular = mSkyBoxInfo.mSpecular.string();
		ImGui::InputText("Specular", &specular);

		if (ImGui::BeginDragDropTarget())
		{
			const ImGuiPayload* pathPayLoad = ImGui::AcceptDragDropPayload("Path");

			if (pathPayLoad)
			{
				std::filesystem::path* path
					= static_cast<std::filesystem::path*>(pathPayLoad->Data);

				if (path->extension() == ".dds") // 葛胆 积己
				{
					mSkyBoxInfo.mSpecular = *path;

					mGraphicsEngine->SetIBLTexture(mSkyBoxInfo.mDiffuse
						, mSkyBoxInfo.mSpecular
						, mSkyBoxInfo.mBrdfLUT);
				}
			}
		}

		// brdfLUT
		std::string brdfLUT = mSkyBoxInfo.mBrdfLUT.string();
		ImGui::InputText("BrdfLUT", &brdfLUT);

		if (ImGui::BeginDragDropTarget())
		{
			const ImGuiPayload* pathPayLoad = ImGui::AcceptDragDropPayload("Path");

			if (pathPayLoad)
			{
				std::filesystem::path* path
					= static_cast<std::filesystem::path*>(pathPayLoad->Data);

				if (path->extension() == ".dds") // 葛胆 积己
				{
					mSkyBoxInfo.mBrdfLUT = *path;

					mGraphicsEngine->SetIBLTexture(mSkyBoxInfo.mDiffuse
						, mSkyBoxInfo.mSpecular
						, mSkyBoxInfo.mBrdfLUT);
				}
			}
		}
	}

	ImGui::End();
}


void fq::game_engine::SkyBoxWindow::SaveSkyBox(std::filesystem::path path)
{
	mSkyBoxInfo.Save(path);
}

