#define _CRT_SECURE_NO_WARNINGS

#include "MaterialWindow.h"

#include <imgui.h>
#include "imgui_stdlib.h"

#include "../FQCommon/FQPath.h"
#include "../FQGraphics/IFQGraphics.h"

#include "GameProcess.h"
#include "ResourceSystem.h"

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

	auto getFilePath = [](std::wstring& wfilename, const std::string& imguiLabel)
		{
			char filename[256];
			wcstombs(filename, wfilename.c_str(), 256);

			ImGui::InputText(imguiLabel.c_str(), filename, 256);
			if (ImGui::BeginDragDropTarget())
			{
				const ImGuiPayload* pathPayLoad = ImGui::AcceptDragDropPayload("Path");

				if (pathPayLoad)
				{
					std::filesystem::path* path = static_cast<std::filesystem::path*>(pathPayLoad->Data);
					wfilename = path->c_str();
				}
			}
		};

	if (!mbIsOpen) return;

	graphics::MaterialInfo materialInfo;

	if (ImGui::Begin("Material Info", &mbIsOpen))
	{
		// 텍스처 패스
		static char materialPath[256];
		strcpy(materialPath, mSelectMaterialPaths.string().c_str());

		if (ImGui::InputText("Material Path", materialPath, 256))
		{
			std::filesystem::path path = std::filesystem::path(materialPath);

			if (path.extension() == ".material")
			{
				if (!std::filesystem::exists(path))
				{
					spdlog::warn("{} material path is invalid", mSelectMaterialPaths.string());
				}
				else
				{
					mSelectMaterialPaths = path;

					std::string stringPath = mSelectMaterialPaths.string();
					std::shared_ptr<fq::graphics::IMaterial> materialInterfaceOrNull = mGameProcess->mResourceSystem->GetMaterial(stringPath);

					if (materialInterfaceOrNull == nullptr)
					{
						mGameProcess->mResourceSystem->LoadMaterial(stringPath);
						materialInterfaceOrNull = mGameProcess->mResourceSystem->GetMaterial(stringPath);
					}
					assert(materialInterfaceOrNull != nullptr);

					mMaterialInterface = materialInterfaceOrNull;
				}
			}
		}

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
						spdlog::warn("{} material path is invalid", mSelectMaterialPaths.string());
					}
					else
					{
						mSelectMaterialPaths = *path;

						std::string stringPath = mSelectMaterialPaths.string();
						std::shared_ptr<fq::graphics::IMaterial> materialInterfaceOrNull = mGameProcess->mResourceSystem->GetMaterial(stringPath);

						if (materialInterfaceOrNull == nullptr)
						{
							mGameProcess->mResourceSystem->LoadMaterial(stringPath);
							materialInterfaceOrNull = mGameProcess->mResourceSystem->GetMaterial(stringPath);
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
		getFilePath(materialInfo.BaseColorFileName, "Base Color File");
		getFilePath(materialInfo.MetalnessFileName, "Metalness File");
		getFilePath(materialInfo.RoughnessFileName, "Roughness File");
		getFilePath(materialInfo.NormalFileName, "Normal File");
		getFilePath(materialInfo.EmissiveFileName, "Emissive File");
		getFilePath(materialInfo.MetalnessSmoothnessFileName, "MetalnessSmoothness File");

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

		// dissolve
		ImGui::Checkbox("Use Dissolve", &materialInfo.bUseDissolve);
		getFilePath(materialInfo.NoiseFileName, "Noise File");
		ImGui::InputFloat("OutlineThickness", reinterpret_cast<float*>(&materialInfo.OutlineThickness));
		ImGui::InputFloat("DissolveCutoff", reinterpret_cast<float*>(&materialInfo.DissolveCutoff));
		ImGui::ColorEdit4("Dissolve StartColor", reinterpret_cast<float*>(&materialInfo.DissolveStartColor));
		ImGui::ColorEdit4("Dissolve EndColor", reinterpret_cast<float*>(&materialInfo.DissolveEndColor));
		ImGui::ColorEdit4("Dissolve StartEmissive", reinterpret_cast<float*>(&materialInfo.DissolveStartEmissive));
		ImGui::ColorEdit4("Dissolve EndEmissive", reinterpret_cast<float*>(&materialInfo.DissolveEndEmissive));

		// rim light
		ImGui::Checkbox("Use Rim Light", &materialInfo.bUseRimLight);
		ImGui::InputFloat("RimPow", reinterpret_cast<float*>(&materialInfo.RimPow));
		ImGui::InputFloat("RimIntensity", reinterpret_cast<float*>(&materialInfo.RimIntensity));
		ImGui::ColorEdit4("Rim Color", reinterpret_cast<float*>(&materialInfo.RimLightColor));

		// inv rim light
		ImGui::Checkbox("Use Inv Rim Light", &materialInfo.bUseInvRimLight);
		ImGui::InputFloat("InvRimPow", reinterpret_cast<float*>(&materialInfo.InvRimPow));
		ImGui::InputFloat("InvRimIntensity", reinterpret_cast<float*>(&materialInfo.InvRimIntensity));
		ImGui::ColorEdit4("InvRim Color", reinterpret_cast<float*>(&materialInfo.InvRimLightColor));

		// mul emissive Alpha
		ImGui::Checkbox("Use Mul Emissive Alpha", &materialInfo.bUseMulEmissiveAlpha);
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
