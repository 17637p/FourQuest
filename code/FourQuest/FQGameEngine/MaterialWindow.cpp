#define _CRT_SECURE_NO_WARNINGS

#include "MaterialWindow.h"

#include <imgui.h>
#include "imgui_stdlib.h"

#include "../FQCommon/FQPath.h"
#include "../FQGraphics/IFQGraphics.h"
#include "GameProcess.h"

fq::game_engine::MaterialWindow::MaterialWindow()
	: mbIsOpen(false)
	, mGameProcess(nullptr)
	, mEditorProcess(nullptr)
	, mEventManager(nullptr)
	, mSelectMaterialPaths{}
	, mMaterialInterface{ nullptr }
{}

void fq::game_engine::MaterialWindow::Initialize(GameProcess* game, EditorProcess* editor)
{
	mGameProcess = game;
	mEditorProcess = editor;
	mEventManager = mGameProcess->mEventManager.get();

	mOnLoadSceneHandler = mEventManager
		->RegisterHandle<fq::event::OnUnloadScene>(this, &MaterialWindow::OnUnloadScene);
}

void fq::game_engine::MaterialWindow::Render()
{
	using namespace fq::graphics;

	if (!mbIsOpen) return;

	graphics::MaterialInfo materialInfo;

	if (ImGui::Begin("Material Info", &mbIsOpen))
	{
		// 텍스처 패스
		static char materialPath[256];
		strcpy(materialPath, mSelectMaterialPaths.string().c_str());
		ImGui::InputText("Material Path", materialPath, 256);

		if (ImGui::BeginDragDropTarget())
		{
			const ImGuiPayload* pathPayLoad = ImGui::AcceptDragDropPayload("Path");

			if (pathPayLoad)
			{
				std::filesystem::path* path
					= static_cast<std::filesystem::path*>(pathPayLoad->Data);

				if (path->extension() == ".material")
				{
					if (!std::filesystem::exists(*path))
					{
						spdlog::warn("{} materialPath is invalid", mSelectMaterialPaths.string());
					}
					else
					{
						mSelectMaterialPaths = *path;

						std::shared_ptr<fq::graphics::IMaterial> materialInterfaceOrNull = mGameProcess->mGraphics->GetMaterialOrNull(mSelectMaterialPaths.string());

						if (materialInterfaceOrNull == nullptr)
						{
							const graphics::MaterialInfo& materialInfo = mGameProcess->mGraphics->ReadMaterialInfo(mSelectMaterialPaths.string());
							materialInterfaceOrNull = mGameProcess->mGraphics->CreateMaterial(mSelectMaterialPaths.string(), materialInfo);
						}
						assert(materialInterfaceOrNull != nullptr);

						mMaterialInterface = materialInterfaceOrNull;
					}
				}
			}
		}

		if (mMaterialInterface != nullptr)
		{
			materialInfo = mMaterialInterface->GetInfo();
		}

		// RenderModeType
		const char* renderModes[] = { "Opaque", "Transparent" };
		int currentRenderMode = static_cast<int>(materialInfo.RenderModeType);
		if (ImGui::Combo("Render Mode", &currentRenderMode, renderModes, IM_ARRAYSIZE(renderModes)))
		{
			materialInfo.RenderModeType = static_cast<MaterialInfo::ERenderMode>(currentRenderMode);
		}

		// BaseColor
		ImGui::ColorEdit4("Base Color", reinterpret_cast<float*>(&materialInfo.BaseColor));

		// Metalness
		ImGui::SliderFloat("Metalness", &materialInfo.Metalness, 0.0f, 1.0f);

		// Roughness
		ImGui::SliderFloat("Roughness", &materialInfo.Roughness, 0.0f, 1.0f);

		// EmissiveColor
		ImGui::ColorEdit4("Emissive Color", reinterpret_cast<float*>(&materialInfo.EmissiveColor));

		//// Texture file names
		char baseColorFileName[256];
		wcstombs(baseColorFileName, materialInfo.BaseColorFileName.c_str(), 256);
		ImGui::InputText("Base Color File", baseColorFileName, 256);
		if (ImGui::BeginDragDropTarget())
		{
			const ImGuiPayload* pathPayLoad = ImGui::AcceptDragDropPayload("Path");

			if (pathPayLoad)
			{
				std::filesystem::path* path = static_cast<std::filesystem::path*>(pathPayLoad->Data);
				materialInfo.BaseColorFileName = path->c_str();
			}
		}

		char metalnessFileName[256];
		wcstombs(metalnessFileName, materialInfo.MetalnessFileName.c_str(), 256);
		ImGui::InputText("Metalness File", metalnessFileName, 256);
		if (ImGui::BeginDragDropTarget())
		{
			const ImGuiPayload* pathPayLoad = ImGui::AcceptDragDropPayload("Path");

			if (pathPayLoad)
			{
				std::filesystem::path* path = static_cast<std::filesystem::path*>(pathPayLoad->Data);
				materialInfo.MetalnessFileName = path->c_str();
			}
		}

		char roughnessFileName[256];
		wcstombs(roughnessFileName, materialInfo.RoughnessFileName.c_str(), 256);
		ImGui::InputText("Roughness File", roughnessFileName, 256);
		if (ImGui::BeginDragDropTarget())
		{
			const ImGuiPayload* pathPayLoad = ImGui::AcceptDragDropPayload("Path");

			if (pathPayLoad)
			{
				std::filesystem::path* path = static_cast<std::filesystem::path*>(pathPayLoad->Data);
				materialInfo.RoughnessFileName = path->c_str();
			}
		}

		char normalFileName[256];
		wcstombs(normalFileName, materialInfo.NormalFileName.c_str(), 256);
		ImGui::InputText("Normal File", normalFileName, 256);
		if (ImGui::BeginDragDropTarget())
		{
			const ImGuiPayload* pathPayLoad = ImGui::AcceptDragDropPayload("Path");

			if (pathPayLoad)
			{
				std::filesystem::path* path = static_cast<std::filesystem::path*>(pathPayLoad->Data);
				materialInfo.NormalFileName = path->c_str();
			}
		}

		char emissiveFileName[256];
		wcstombs(emissiveFileName, materialInfo.EmissiveFileName.c_str(), 256);
		ImGui::InputText("Emissive File", emissiveFileName, 256);
		if (ImGui::BeginDragDropTarget())
		{
			const ImGuiPayload* pathPayLoad = ImGui::AcceptDragDropPayload("Path");

			if (pathPayLoad)
			{
				std::filesystem::path* path = static_cast<std::filesystem::path*>(pathPayLoad->Data);
				materialInfo.EmissiveFileName = path->c_str();
			}
		}

		char metalnessSmoothnessFileName[256];
		wcstombs(metalnessSmoothnessFileName, materialInfo.MetalnessSmoothnessFileName.c_str(), 256);
		ImGui::InputText("MetalnessSmoothness File", emissiveFileName, 256);
		if (ImGui::BeginDragDropTarget())
		{
			const ImGuiPayload* pathPayLoad = ImGui::AcceptDragDropPayload("Path");

			if (pathPayLoad)
			{
				std::filesystem::path* path = static_cast<std::filesystem::path*>(pathPayLoad->Data);
				materialInfo.MetalnessSmoothnessFileName = path->c_str();
			}
		}

		// Usage booleans
		ImGui::Checkbox("Use Base Color", &materialInfo.bUseBaseColor);
		ImGui::Checkbox("Use Metalness", &materialInfo.bUseMetalness);
		ImGui::Checkbox("Use Roughness", &materialInfo.bUseRoughness);
		ImGui::Checkbox("Use Normal", &materialInfo.bUseNormalness);
		ImGui::Checkbox("Use Emissive", &materialInfo.bIsUsedEmissive);
		ImGui::Checkbox("Use MetalnessSmoothness", &materialInfo.bIsUsedMetalnessSmoothness);

		// Tiling and Offset
		ImGui::InputFloat2("Tiling", reinterpret_cast<float*>(&materialInfo.Tiling));
		ImGui::InputFloat2("Offset", reinterpret_cast<float*>(&materialInfo.Offset));

		// AlphaCutoff
		ImGui::SliderFloat("Alpha Cutoff", &materialInfo.AlphaCutoff, 0.0f, 1.0f);

		// SampleType
		const char* sampleModes[] = { "Clamp", "Repeat" };
		int currentSampleMode = static_cast<int>(materialInfo.SampleType);
		if (ImGui::Combo("Sample Mode", &currentSampleMode, sampleModes, IM_ARRAYSIZE(sampleModes)))
		{
			materialInfo.SampleType = static_cast<ESampleMode>(currentSampleMode);
		}

		// RasterizeType
		const char* rasterizeModes[] = { "BackFaceClip", "TwoSide" };
		int currentRasterizeMode = static_cast<int>(materialInfo.RasterizeType);
		if (ImGui::Combo("Rasterize Mode", &currentRasterizeMode, rasterizeModes, IM_ARRAYSIZE(rasterizeModes)))
		{
			materialInfo.RasterizeType = static_cast<ERasterizeMode>(currentRasterizeMode);
		}

		ImGui::InputFloat("EmissiveIntensity", reinterpret_cast<float*>(&materialInfo.EmissiveIntensity));
	}

	if (mMaterialInterface != nullptr)
	{
		mMaterialInterface->SetInfo(materialInfo);
	}


	ImGui::End();
}

void fq::game_engine::MaterialWindow::SaveMaterial()
{
	if (mSelectMaterialPaths.empty())
		return;
	if (mMaterialInterface == nullptr)
		return;

	mGameProcess->mGraphics->WriteMaterialInfo(mSelectMaterialPaths.string(), mMaterialInterface->GetInfo());
}

void fq::game_engine::MaterialWindow::OnUnloadScene()
{
	mSelectMaterialPaths = "";
	mMaterialInterface = nullptr;
}
