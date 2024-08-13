#include "Setting.h"

#include <imgui.h>
#include "imgui_stdlib.h"

#include "GameProcess.h"
#include "RenderingSystem.h"
#include "../FQGameModule/GameModule.h"
#include "../FQGameModule/SkinnedMeshRenderer.h"
#include "../FQGameModule/StaticMeshRenderer.h"
#include "../FQGameModule/Decal.h"
#include "../FQGraphics/IFQGraphics.h"
#include "../FQCommon/FQPath.h"

fq::game_engine::Setting::Setting()
	:mbUseSnap(false)
	, mbIsOpen(false)
	, mbUseGrayScale(false)
	, mbUseRootPicking(false)
	, mbCanEditPath(false)
	, mSnap{}
	, mMode(ImGuizmo::WORLD)
{

}

void fq::game_engine::Setting::Initialize(GameProcess* game, EditorProcess* editor)
{
	mGameProcess = game;
}

void fq::game_engine::Setting::Render()
{
	if (!mbIsOpen) return;

	if (ImGui::Begin("Setting", &mbIsOpen))
	{
		beginChild_GizumoSetting();
		beginChild_GraphicsSetting();
		beginChild_InspectorSetting();
	}

	ImGui::End();
}

void fq::game_engine::Setting::beginChild_GizumoSetting()
{
	if (ImGui::CollapsingHeader("Gizumo"))
	{
		if (ImGui::BeginChild("Gizumo"), ImVec2(0, 0), ImGuiChildFlags_AutoResizeY)
		{
			ImGui::Checkbox("UseRootPicking", &mbUseRootPicking);
			if (ImGui::IsItemHovered())
			{
				ImGui::SetTooltip(reinterpret_cast<const char*>(u8"오브젝트를 피킹할때 가장 최상위 부모를 선택합니다"));
			}

			ImGui::Checkbox("UseSnap", &mbUseSnap);
			if (ImGui::IsItemHovered())
			{
				//ImGui::SetTooltip(reinterpret_cast<const char*>(u8"기즈모의 이동을 "));
			}

			ImGui::InputFloat3("Snap", mSnap);

			std::string currentMode = (mMode == ImGuizmo::WORLD) ? "World" : "Local";

			if (ImGui::BeginCombo("Mode", currentMode.c_str()))
			{
				if (ImGui::Selectable("World"))
				{
					mMode = ImGuizmo::WORLD;
				}
				if (ImGui::Selectable("Local"))
				{
					mMode = ImGuizmo::LOCAL;
				}

				ImGui::EndCombo();
			}
		}
		ImGui::EndChild();
	}
}


void fq::game_engine::Setting::beginChild_GraphicsSetting()
{
	if (ImGui::CollapsingHeader("Graphics"))
	{
		if (ImGui::BeginChild("Graphics"), ImVec2(0, 0), ImGuiChildFlags_AutoResizeY)
		{
			if (ImGui::Checkbox("UseGrayScale", &mbUseGrayScale))
			{
				auto materials = mGameProcess->mGraphics->GetMaterials();

				for (auto& material : materials)
				{
					auto materialInfo = material->GetInfo();

					materialInfo.bUseBaseColor = !mbUseGrayScale;
					materialInfo.bUseMetalness = !mbUseGrayScale;
					materialInfo.bUseNormalness = !mbUseGrayScale;
					materialInfo.bUseRoughness = !mbUseGrayScale;
					materialInfo.BaseColor = { 1.f,1.f,1.f,1.f };

					material->SetInfo(materialInfo);
				}
			}

			// // 임시적용
			// if (ImGui::Button("Change TexturePath"))
			// {
			// 	auto scene = mGameProcess->mSceneManager->GetCurrentScene();
			// 
			// 	auto texturePath = fq::path::GetResourcePath() / "Texture";
			// 
			// 	scene->ViewComponents<game_module::SkinnedMeshRenderer>(
			// 		[texturePath](game_module::GameObject& object, game_module::SkinnedMeshRenderer& renderer)
			// 		{
			// 			if (renderer.GetTexturePath().empty())
			// 			{
			// 				//renderer.SetTexturePath(texturePath.string());
			// 			}
			// 		}
			// 	);
			// 
			// 	scene->ViewComponents<game_module::StaticMeshRenderer>(
			// 		[texturePath](game_module::GameObject& object, game_module::StaticMeshRenderer& renderer)
			// 		{
			// 			if (renderer.GetTexturePath().empty())
			// 			{
			// 				renderer.SetTexturePath(texturePath.string());
			// 			}
			// 		}
			// 	);
			// }

			// if (ImGui::Button("Reload Texture(TexturePath + TextureName)"))
			// {
			// 	auto scene = mGameProcess->mSceneManager->GetCurrentScene();
			// 
			// 	scene->ViewComponents<game_module::SkinnedMeshRenderer>(
			// 		[](game_module::GameObject& object, game_module::SkinnedMeshRenderer& renderer)
			// 		{
			// 			const auto& texturePath = std::filesystem::path(renderer.GetTexturePath());
			// 			auto materialInfos = renderer.GetMaterialInfos();
			// 
			// 			for (auto& materialInfo : materialInfos)
			// 			{
			// 				if (!std::filesystem::path(materialInfo.BaseColorFileName).filename().empty())materialInfo.BaseColorFileName = (texturePath / std::filesystem::path(materialInfo.BaseColorFileName).filename()).wstring();
			// 				if (!std::filesystem::path(materialInfo.MetalnessFileName).filename().empty())materialInfo.MetalnessFileName = (texturePath / std::filesystem::path(materialInfo.MetalnessFileName).filename()).wstring();
			// 				if (!std::filesystem::path(materialInfo.RoughnessFileName).filename().empty())materialInfo.RoughnessFileName = (texturePath / std::filesystem::path(materialInfo.RoughnessFileName).filename()).wstring();
			// 				if (!std::filesystem::path(materialInfo.NormalFileName).filename().empty())materialInfo.NormalFileName = (texturePath / std::filesystem::path(materialInfo.NormalFileName).filename()).wstring();
			// 				if (!std::filesystem::path(materialInfo.EmissiveFileName).filename().empty())materialInfo.EmissiveFileName = (texturePath / std::filesystem::path(materialInfo.EmissiveFileName).filename()).wstring();
			// 			}
			// 
			// 			renderer.SetMaterialInfos(materialInfos);
			// 		}
			// 	);
			// 
			// 	scene->ViewComponents<game_module::StaticMeshRenderer>(
			// 		[](game_module::GameObject& object, game_module::StaticMeshRenderer& renderer)
			// 		{
			// 			const auto& texturePath = std::filesystem::path(renderer.GetTexturePath());
			// 			auto materialInfos = renderer.GetMaterialInfos();
			// 
			// 			for (auto& materialInfo : materialInfos)
			// 			{
			// 				if (!std::filesystem::path(materialInfo.BaseColorFileName).filename().empty())materialInfo.BaseColorFileName = (texturePath / std::filesystem::path(materialInfo.BaseColorFileName).filename()).wstring();
			// 				if (!std::filesystem::path(materialInfo.MetalnessFileName).filename().empty())materialInfo.MetalnessFileName = (texturePath / std::filesystem::path(materialInfo.MetalnessFileName).filename()).wstring();
			// 				if (!std::filesystem::path(materialInfo.RoughnessFileName).filename().empty())materialInfo.RoughnessFileName = (texturePath / std::filesystem::path(materialInfo.RoughnessFileName).filename()).wstring();
			// 				if (!std::filesystem::path(materialInfo.NormalFileName).filename().empty())materialInfo.NormalFileName = (texturePath / std::filesystem::path(materialInfo.NormalFileName).filename()).wstring();
			// 				if (!std::filesystem::path(materialInfo.EmissiveFileName).filename().empty())materialInfo.EmissiveFileName = (texturePath / std::filesystem::path(materialInfo.EmissiveFileName).filename()).wstring();
			// 			}
			// 
			// 			renderer.SetMaterialInfos(materialInfos);
			// 		}
			// 	);
			// 
			// 	scene->ViewComponents<game_module::Decal>(
			// 		[](game_module::GameObject& object, game_module::Decal& decal)
			// 		{
			// 			const auto& texturePath = fq::path::GetResourcePath() / "Texture";
			// 			auto materialInfo = decal.GetDecalMaterialInfo();
			// 
			// 			if (!std::filesystem::path(materialInfo.BaseColorFileName).filename().empty())materialInfo.BaseColorFileName = (texturePath / std::filesystem::path(materialInfo.BaseColorFileName).filename()).wstring();
			// 			if (!std::filesystem::path(materialInfo.NormalFileName).filename().empty())materialInfo.NormalFileName = (texturePath / std::filesystem::path(materialInfo.NormalFileName).filename()).wstring();
			// 			if (!std::filesystem::path(materialInfo.EmissiveFileName).filename().empty())materialInfo.EmissiveFileName = (texturePath / std::filesystem::path(materialInfo.EmissiveFileName).filename()).wstring();
			// 
			// 			decal.SetDecalMaterialInfo(materialInfo);
			// 		}
			// 	);
			// }

			if (ImGui::Button("Generate Material File"))
			{
				auto scene = mGameProcess->mSceneManager->GetCurrentScene();
				auto graphics = mGameProcess->mGraphics;
				const auto& renderingSystem = mGameProcess->mRenderingSystem;

				auto createMaterial = [&graphics](auto directory, const auto& modelData)
					{
						// material 생성
						for (const auto& material : modelData.Materials)
						{
							using namespace fq::graphics;

							MaterialInfo materialInfo;

							materialInfo.BaseColor = material.BaseColor;
							materialInfo.Metalness = material.Metalness;
							materialInfo.Roughness = material.Roughness;

							std::filesystem::path texturePath = fq::path::GetResourcePath() / "Texture";

							if (material.BaseColorFileName != L"") materialInfo.BaseColorFileName = texturePath / material.BaseColorFileName;
							if (material.MetalnessFileName != L"") materialInfo.MetalnessFileName = texturePath / material.MetalnessFileName;
							if (material.RoughnessFileName != L"") materialInfo.RoughnessFileName = texturePath / material.RoughnessFileName;
							if (material.NormalFileName != L"") materialInfo.NormalFileName = texturePath / material.NormalFileName;
							if (material.EmissiveFileName != L"") materialInfo.EmissiveFileName = texturePath / material.EmissiveFileName;

							auto materialPath = (directory / material.Name).string() + ".material";
							graphics->WriteMaterialInfo(materialPath, materialInfo);
						}
					};

				scene->ViewComponents<game_module::SkinnedMeshRenderer>(
					[&graphics, &renderingSystem, createMaterial](game_module::GameObject& object, game_module::SkinnedMeshRenderer& renderer)
					{
						if (!renderer.GetMaterialPaths().empty())
						{
							return;
						}

						std::filesystem::path modelPath = std::filesystem::path(renderer.GetModelPath());
						std::filesystem::path modelDirectory = modelPath;
						modelDirectory.replace_extension("");
						//modelDirectory /= modelDirectory.filename();

						const auto& modelData = graphics->CreateModelResource(renderingSystem->GetModelKey(modelPath.string()), modelPath.string());
						createMaterial(modelDirectory, modelData);

						std::vector<std::string> materialPaths;
						std::vector<std::shared_ptr<fq::graphics::IMaterial>> materialInterfaces;

						for (const auto& [node, mesh] : modelData.Meshes)
						{
							if (mesh.Name == renderer.GetMeshName())
							{
								for (const auto& subset : mesh.Subsets)
								{
									auto materialPath = (modelDirectory / subset.MaterialName).string() + ".material";

									auto materialInterface = graphics->GetMaterialOrNull(materialPath);

									if (materialInterface == nullptr)
									{
										const graphics::MaterialInfo& materialInfo = graphics->ReadMaterialInfo(materialPath);
										materialInterface = graphics->CreateMaterial(materialPath, materialInfo);
									}

									materialPaths.push_back(materialPath);
									materialInterfaces.push_back(materialInterface);
								}
							}
						}

						renderer.SetMaterialPaths(materialPaths);
						renderer.SetMaterialInterfaces(materialInterfaces);
					}
				);

				scene->ViewComponents<game_module::StaticMeshRenderer>(
					[&graphics, &renderingSystem, createMaterial](game_module::GameObject& object, game_module::StaticMeshRenderer& renderer)
					{
						if (!renderer.GetMaterialPaths().empty())
						{
							return;
						}

						std::filesystem::path modelPath = std::filesystem::path(renderer.GetModelPath());
						std::filesystem::path modelDirectory = modelPath;
						modelDirectory.replace_extension("");
						// modelDirectory /= modelDirectory.filename();

						const auto& modelData = graphics->CreateModelResource(renderingSystem->GetModelKey(modelPath.string()), modelPath.string());
						createMaterial(modelDirectory, modelData);

						std::vector<std::string> materialPaths;
						std::vector<std::shared_ptr<fq::graphics::IMaterial>> materialInterfaces;

						for (const auto& [node, mesh] : modelData.Meshes)
						{
							if (mesh.Name == renderer.GetMeshName())
							{
								for (const auto& subset : mesh.Subsets)
								{
									auto materialPath = (modelDirectory / subset.MaterialName).string() + ".material";

									auto materialInterface = graphics->GetMaterialOrNull(materialPath);

									if (materialInterface == nullptr)
									{
										const graphics::MaterialInfo& materialInfo = graphics->ReadMaterialInfo(materialPath);
										materialInterface = graphics->CreateMaterial(materialPath, materialInfo);
									}

									materialPaths.push_back(materialPath);
									materialInterfaces.push_back(materialInterface);
								}
							}
						}

						renderer.SetMaterialPaths(materialPaths);
						renderer.SetMaterialInterfaces(materialInterfaces);
					}
				);
			}
		}

		ImGui::EndChild();
	}
}

ImGuizmo::MODE fq::game_engine::Setting::GetMode()
{
	return mMode;
}

void fq::game_engine::Setting::beginChild_InspectorSetting()
{
	if (ImGui::CollapsingHeader("Insepctor"))
	{
		if (ImGui::BeginChild("Insepctor"), ImVec2(0, 0), ImGuiChildFlags_AutoResizeY)
		{
			ImGui::Checkbox("CanEditDargDropPath", &mbCanEditPath);
			if (ImGui::IsItemHovered())
			{
				ImGui::SetTooltip(reinterpret_cast<const char*>(u8"드래그 드랍하는 경로들을 수정 가능하게 합니다"));
			}
		}
		ImGui::EndChild();
	}
}

