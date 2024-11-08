#include "Setting.h"

#include <imgui.h>
#include <spdlog/spdlog.h>
#include "imgui_stdlib.h"

#include "../FQGameModule/GameModule.h"
#include "../FQGameModule/SkinnedMeshRenderer.h"
#include "../FQGameModule/StaticMeshRenderer.h"
#include "../FQGameModule/Transform.h"
#include "../FQGameModule/Decal.h"
#include "../FQGameModule/Particle.h"
#include "../FQGameModule/Terrain.h"
#include "../FQGraphics/IFQGraphics.h"
#include "../FQCommon/FQPath.h"
#include "../FQCommon/StringUtil.h"

#include "GameProcess.h"
#include "RenderingSystem.h"
#include "ResourceSystem.h"

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
				auto materials = mGameProcess->mResourceSystem->GetMaterials();

				for (auto& [key, material] : materials)
				{
					auto materialInfo = material->GetInfo();

					materialInfo.bUseBaseColor = !mbUseGrayScale;
					materialInfo.bUseMetalness = !mbUseGrayScale;
					materialInfo.bUseNormalness = !mbUseGrayScale;
					materialInfo.bUseRoughness = !mbUseGrayScale;
					materialInfo.BaseColor = { 1.f,1.f,1.f,1.f };

					material->SetInfo(materialInfo);
				}

				mGameProcess->mGraphics->SetIsUseLightmapOnly(mbUseGrayScale);
			}

			std::string materialDirectory = mExtractedPrefabPath.string();
			ImGui::InputText("Material Extracted Prefab", &materialDirectory);

			if (ImGui::BeginDragDropTarget())
			{
				const ImGuiPayload* pathPayLoad = ImGui::AcceptDragDropPayload("Path");

				if (pathPayLoad)
				{
					std::filesystem::path* path
						= static_cast<std::filesystem::path*>(pathPayLoad->Data);

					if (std::filesystem::exists(*path) && path->extension() == ".prefab")
					{
						mExtractedPrefabPath = *path;
					}
				}
			}

			if (ImGui::Button("Generate Material by Extracted Prefab"))
			{
				if (std::filesystem::exists(mExtractedPrefabPath) && mExtractedPrefabPath.extension() == ".prefab")
				{
					std::ifstream readData(mExtractedPrefabPath);

					nlohmann::ordered_json prefabJson;

					if (readData.is_open())
					{
						readData >> prefabJson;
						readData.close();
					}
					else
					{
						assert(!"파일 열기 실패");
					}

					auto generateWString = [](const nlohmann::ordered_json& arrayJson)
						{
							std::wstring tempString;

							for (auto iter = arrayJson.begin();
								iter != arrayJson.end();
								++iter)
							{
								WCHAR wch = *iter;
								tempString += wch;
							}

							return tempString;
						};

					auto generateMaterial = [generateWString](const nlohmann::ordered_json& materialInfoJson)
						{
							using namespace fq::graphics;
							std::string temp, renderModeStr, sampleTypeStr, rasterizeTypeStr;

							fq::graphics::MaterialInfo materialInfo;
							materialInfo.AlphaCutoff = materialInfoJson.at("AlphaCutoff").get<float>();
							materialInfo.BaseColor.x = materialInfoJson.at("BaseColor").at("x").get<float>();
							materialInfo.BaseColor.y = materialInfoJson.at("BaseColor").at("y").get<float>();
							materialInfo.BaseColor.z = materialInfoJson.at("BaseColor").at("z").get<float>();
							materialInfo.BaseColor.w = materialInfoJson.at("BaseColor").at("w").get<float>();

							materialInfo.BaseColorFileName = generateWString(materialInfoJson.at("BaseColorFileName"));

							materialInfo.EmissiveColor.x = materialInfoJson.at("EmissiveColor").at("x").get<float>();
							materialInfo.EmissiveColor.y = materialInfoJson.at("EmissiveColor").at("y").get<float>();
							materialInfo.EmissiveColor.z = materialInfoJson.at("EmissiveColor").at("z").get<float>();
							materialInfo.EmissiveColor.w = materialInfoJson.at("EmissiveColor").at("w").get<float>();

							materialInfo.EmissiveFileName = generateWString(materialInfoJson.at("EmissiveFileName"));

							materialInfoJson.at("Metalness").get_to(materialInfo.Metalness);

							materialInfo.MetalnessFileName = generateWString(materialInfoJson.at("MetalnessFileName"));

							materialInfo.NormalFileName = generateWString(materialInfoJson.at("NormalFileName"));

							materialInfo.Offset.x = materialInfoJson.at("Offset").at("x").get<float>();
							materialInfo.Offset.y = materialInfoJson.at("Offset").at("y").get<float>();

							materialInfo.RasterizeType = static_cast<ERasterizeMode>(materialInfoJson.at("RasterizeType").get<int>());

							materialInfo.RenderModeType = static_cast<MaterialInfo::ERenderMode>(materialInfoJson.at("RenderModeType").get<int>());

							materialInfoJson.at("Roughness").get_to(materialInfo.Roughness);

							materialInfo.RoughnessFileName = generateWString(materialInfoJson.at("RoughnessFileName"));

							materialInfo.SampleType = static_cast<ESampleMode>(materialInfoJson.at("SampleType").get<int>());

							materialInfo.Tiling.x = materialInfoJson.at("Tiling").at("x").get<float>();
							materialInfo.Tiling.y = materialInfoJson.at("Tiling").at("y").get<float>();

							materialInfoJson.at("bIsUsedEmissive").get_to(materialInfo.bIsUsedEmissive);
							materialInfoJson.at("bUseBaseColor").get_to(materialInfo.bUseBaseColor);
							materialInfoJson.at("bUseMetalness").get_to(materialInfo.bUseMetalness);
							materialInfoJson.at("bUseNormalness").get_to(materialInfo.bUseNormalness);
							materialInfoJson.at("bUseRoughness").get_to(materialInfo.bUseRoughness);

							return materialInfo;
						};

					for (const auto& element : prefabJson.items())
					{
						for (const auto& element1 : element.value().items())
						{
							const auto& value = element1.value();

							auto materialInfoJson = value.find("MaterialInfo");
							std::vector<fq::graphics::MaterialInfo> materialInfos;

							if (materialInfoJson == value.end())
							{
								continue;
							}

							for (const auto& element2 : materialInfoJson.value().items())
							{
								const auto& info = generateMaterial(element2.value());
								materialInfos.push_back(info);
							}

							auto materialNameJson = value.find("Materials");
							std::vector<std::string> materialNames;

							if (materialNameJson == value.end())
							{
								continue;
							}

							for (const auto& element2 : materialNameJson.value().items())
							{
								const auto& name = element2.value();
								materialNames.push_back(name);
							}

							size_t MAX_INDEX = std::min<size_t>(materialInfos.size(), materialNames.size());
							auto modelPathJson = value.find("ModelPath");

							if (modelPathJson == value.end())
							{
								continue;
							}

							for (size_t i = 0; i < MAX_INDEX; ++i)
							{
								std::filesystem::path modelPath = modelPathJson.value();
								modelPath.replace_extension("");
								std::filesystem::path materialPath = modelPath / (materialNames[i] + ".material");

								mGameProcess->mGraphics->WriteMaterialInfo(materialPath.string(), materialInfos[i]);
							}
						}
					}
				}
			}

			// 빈 머터리얼 경로를 가진 오브젝트를 참조한 모델데이터에 의해 재연결 해줍니다.
			if (ImGui::Button("Generate Material File(empty path object target, by model data)"))
			{
				auto scene = mGameProcess->mSceneManager->GetCurrentScene();
				auto graphics = mGameProcess->mGraphics;
				const auto& renderingSystem = mGameProcess->mRenderingSystem;
				const auto& resourceSystem = mGameProcess->mResourceSystem;

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
					[&graphics, &renderingSystem, &resourceSystem, createMaterial](game_module::GameObject& object, game_module::SkinnedMeshRenderer& renderer)
					{
						if (!renderer.GetMaterialPaths().empty())
						{
							return;
						}

						std::filesystem::path modelPath = std::filesystem::path(renderer.GetModelPath());
						std::filesystem::path modelDirectory = modelPath;
						modelDirectory.replace_extension("");
						//modelDirectory /= modelDirectory.filename();

						if (std::filesystem::exists(modelPath))
						{
							return;
						}

						if (!resourceSystem->HasModel(modelPath.string()))
						{
							resourceSystem->LoadModelResource(modelPath.string());
						}

						const fq::common::Model& modelData = resourceSystem->GetModel(modelPath.string());

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
									assert(std::filesystem::exists(materialPath));
									auto materialInterface = resourceSystem->GetMaterial(materialPath);

									if (materialInterface == nullptr)
									{
										resourceSystem->LoadMaterial(materialPath);
										materialInterface = resourceSystem->GetMaterial(materialPath);
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
					[&graphics, &renderingSystem, &resourceSystem, createMaterial](game_module::GameObject& object, game_module::StaticMeshRenderer& renderer)
					{
						if (!renderer.GetMaterialPaths().empty())
						{
							return;
						}

						std::filesystem::path modelPath = std::filesystem::path(renderer.GetModelPath());
						std::filesystem::path modelDirectory = modelPath;
						modelDirectory.replace_extension("");
						// modelDirectory /= modelDirectory.filename();

						if (std::filesystem::exists(modelPath))
						{
							return;
						}

						if (!resourceSystem->HasModel(modelPath.string()))
						{
							resourceSystem->LoadModelResource(modelPath.string());
						}

						const fq::common::Model& modelData = resourceSystem->GetModel(modelPath.string());

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
									assert(std::filesystem::exists(materialPath));
									auto materialInterface = resourceSystem->GetMaterial(materialPath);

									if (materialInterface == nullptr)
									{
										resourceSystem->LoadMaterial(materialPath);
										materialInterface = resourceSystem->GetMaterial(materialPath);
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

			std::string basePathString = mRewriteMaterialDir.string();
			ImGui::InputText("Material Reference Directory", &basePathString);

			if (ImGui::BeginDragDropTarget())
			{
				const ImGuiPayload* pathPayLoad = ImGui::AcceptDragDropPayload("Path");

				if (pathPayLoad)
				{
					std::filesystem::path* path
						= static_cast<std::filesystem::path*>(pathPayLoad->Data);

					if (std::filesystem::exists(*path) && std::filesystem::is_directory(*path))
					{
						mRewriteMaterialDir = *path;
					}
				}
			}

			if (ImGui::Button("rewrite material texture path(change relative path)"))
			{
				auto makeRelativePath = [](std::wstring& filename)
					{
						if (filename.empty())
						{
							return;
						}

						size_t index = filename.find(L"resource");

						if (index == std::wstring::npos) {
							index = 0;
						}

						filename = filename.substr(index, filename.size());
					};

				if (std::filesystem::exists(mRewriteMaterialDir) && std::filesystem::is_directory(mRewriteMaterialDir))
				{
					for (auto path : std::filesystem::recursive_directory_iterator(mRewriteMaterialDir))
					{
						if (path.path().extension() == ".material")
						{
							auto data = mGameProcess->mGraphics->ReadMaterialInfo(path.path().string());

							makeRelativePath(data.BaseColorFileName);
							makeRelativePath(data.MetalnessFileName);
							makeRelativePath(data.RoughnessFileName);
							makeRelativePath(data.EmissiveFileName);
							makeRelativePath(data.NormalFileName);
							makeRelativePath(data.MetalnessSmoothnessFileName);

							mGameProcess->mGraphics->WriteMaterialInfo(path.path().string(), data);
						}
					}
				}
			}

			if (ImGui::Button("rewrite material texture path(try change file format to dds)"))
			{
				if (std::filesystem::exists(mRewriteMaterialDir) && std::filesystem::is_directory(mRewriteMaterialDir))
				{
					for (auto path : std::filesystem::recursive_directory_iterator(mRewriteMaterialDir))
					{
						if (path.path().extension() == ".material")
						{
							auto data = mGameProcess->mGraphics->ReadMaterialInfo(path.path().string());

							data.BaseColorFileName = ChangeDDSFormat(mGameProcess, data.BaseColorFileName);
							data.MetalnessFileName = ChangeDDSFormat(mGameProcess, data.MetalnessFileName);
							data.RoughnessFileName = ChangeDDSFormat(mGameProcess, data.RoughnessFileName);
							data.EmissiveFileName = ChangeDDSFormat(mGameProcess, data.EmissiveFileName);
							data.NormalFileName = ChangeDDSFormat(mGameProcess, data.NormalFileName);
							data.MetalnessSmoothnessFileName = ChangeDDSFormat(mGameProcess, data.MetalnessSmoothnessFileName);
							data.NoiseFileName = ChangeDDSFormat(mGameProcess, data.NoiseFileName);
							data.BlendTextureName = ChangeDDSFormat(mGameProcess, data.BlendTextureName);

							mGameProcess->mGraphics->WriteMaterialInfo(path.path().string(), data);
						}
					}
				}
			}

			if (ImGui::Button("rewrite decal, particle prefab png -> dds"))
			{
				if (std::filesystem::exists(mRewriteMaterialDir) && std::filesystem::is_directory(mRewriteMaterialDir))
				{
					for (auto path : std::filesystem::recursive_directory_iterator(mRewriteMaterialDir))
					{
						if (path.path().extension() == ".prefab")
						{
							auto gameObjects = mGameProcess->mPrefabManager->LoadPrefab(path.path());

							for (auto gameObject : gameObjects)
							{
								auto decalComp = gameObject->GetComponent<game_module::Decal>();

								if (decalComp != nullptr)
								{
									auto decalMaterialInfo = decalComp->GetDecalMaterialInfo();
									decalMaterialInfo.BaseColorFileName = ChangeDDSFormat(mGameProcess, decalMaterialInfo.BaseColorFileName);
									decalMaterialInfo.NormalFileName = ChangeDDSFormat(mGameProcess, decalMaterialInfo.NormalFileName);
									decalMaterialInfo.EmissiveFileName = ChangeDDSFormat(mGameProcess, decalMaterialInfo.EmissiveFileName);
									decalComp->SetDecalMaterialInfo(decalMaterialInfo);
								}

								auto particleComp = gameObject->GetComponent<game_module::Particle>();

								if (particleComp != nullptr)
								{
									auto particleMaterialInfo = particleComp->GetParticleMaterialInfo();
									particleMaterialInfo.BaseColorFileName = ChangeDDSFormat(mGameProcess, particleMaterialInfo.BaseColorFileName);
									particleMaterialInfo.EmissiveFileName = ChangeDDSFormat(mGameProcess, particleMaterialInfo.EmissiveFileName);
									particleComp->SetParticleMaterialInfo(particleMaterialInfo);
								}

								auto terrainComp = gameObject->GetComponent<game_module::Terrain>();

								if (terrainComp != nullptr)
								{
									auto terrainLayers = terrainComp->GetTerrainLayers();
									for (auto& layer : terrainLayers)
									{
										layer.BaseColor =  common::StringUtil::ToMultiByte(ChangeDDSFormat(mGameProcess, common::StringUtil::ToWide(layer.BaseColor)));
										layer.NormalMap =  common::StringUtil::ToMultiByte(ChangeDDSFormat(mGameProcess, common::StringUtil::ToWide(layer.NormalMap)));
									}

									terrainComp->SetTerrainLayers(terrainLayers);
								}
							}

							if (!gameObjects.empty())
							{
								mGameProcess->mPrefabManager->SavePrefab(gameObjects.front().get(), path.path().parent_path());
							}
						}
					}
				}
			}

			if (ImGui::Button("rewrite decal, particle prefab png -> dds, internal"))
			{
				if (std::filesystem::exists(mRewriteMaterialDir) && std::filesystem::is_directory(mRewriteMaterialDir))
				{
					for (auto path : std::filesystem::recursive_directory_iterator("./resource/internal"))
					{
						if (path.path().extension() == ".prefab")
						{
							auto gameObjects = mGameProcess->mPrefabManager->LoadPrefab(path.path());

							for (auto gameObject : gameObjects)
							{
								auto decalComp = gameObject->GetComponent<game_module::Decal>();

								if (decalComp != nullptr)
								{
									auto decalMaterialInfo = decalComp->GetDecalMaterialInfo();
									decalMaterialInfo.BaseColorFileName = ChangeDDSFormat(mGameProcess, decalMaterialInfo.BaseColorFileName);
									decalMaterialInfo.NormalFileName = ChangeDDSFormat(mGameProcess, decalMaterialInfo.NormalFileName);
									decalMaterialInfo.EmissiveFileName = ChangeDDSFormat(mGameProcess, decalMaterialInfo.EmissiveFileName);
									decalComp->SetDecalMaterialInfo(decalMaterialInfo);
								}

								auto particleComp = gameObject->GetComponent<game_module::Particle>();

								if (particleComp != nullptr)
								{
									auto particleMaterialInfo = particleComp->GetParticleMaterialInfo();
									particleMaterialInfo.BaseColorFileName = ChangeDDSFormat(mGameProcess, particleMaterialInfo.BaseColorFileName);
									particleMaterialInfo.EmissiveFileName = ChangeDDSFormat(mGameProcess, particleMaterialInfo.EmissiveFileName);
									particleComp->SetParticleMaterialInfo(particleMaterialInfo);
								}

								auto terrainComp = gameObject->GetComponent<game_module::Terrain>();

								if (terrainComp != nullptr)
								{
									auto terrainLayers = terrainComp->GetTerrainLayers();
									for (auto& layer : terrainLayers)
									{
										layer.BaseColor = common::StringUtil::ToMultiByte(ChangeDDSFormat(mGameProcess, common::StringUtil::ToWide(layer.BaseColor)));
										layer.NormalMap = common::StringUtil::ToMultiByte(ChangeDDSFormat(mGameProcess, common::StringUtil::ToWide(layer.NormalMap)));
									}

									terrainComp->SetTerrainLayers(terrainLayers);
								}
							}

							if (!gameObjects.empty())
							{
								mGameProcess->mPrefabManager->SavePrefab(gameObjects.front().get(), path.path().parent_path());
							}
						}
					}
				}
			}



			if (ImGui::Button("rewrite renderer material path(change relative path)"))
			{
				auto scene = mGameProcess->mSceneManager->GetCurrentScene();

				scene->ViewComponents<game_module::SkinnedMeshRenderer>(
					[](game_module::GameObject& object, game_module::SkinnedMeshRenderer& renderer)
					{
						if (renderer.GetMaterialPaths().empty())
						{
							return;
						}

						std::vector<std::string> materialPaths = renderer.GetMaterialPaths();

						for (auto& materialPath : materialPaths)
						{
							size_t index = materialPath.find("resource");
							materialPath = materialPath.substr(index, materialPath.size());
						}

						renderer.SetMaterialPaths(materialPaths);
					}
				);

				scene->ViewComponents<game_module::StaticMeshRenderer>(
					[](game_module::GameObject& object, game_module::StaticMeshRenderer& renderer)
					{
						if (renderer.GetMaterialPaths().empty())
						{
							return;
						}

						std::vector<std::string> materialPaths = renderer.GetMaterialPaths();

						for (auto& materialPath : materialPaths)
						{
							size_t index = materialPath.find("resource");
							materialPath = fq::path::GetAbsolutePath(materialPath.substr(index, materialPath.size())).string();
						}

						renderer.SetMaterialPaths(materialPaths);
					}
				);
			}

			if (ImGui::Button("convert preApplied transform"))
			{
				auto scene = mGameProcess->mSceneManager->GetCurrentScene();

				scene->ViewComponents<game_module::StaticMeshRenderer, game_module::Transform>(
					[](game_module::GameObject& object, game_module::StaticMeshRenderer& renderer, game_module::Transform& transform)
					{
						if (renderer.GetMaterialPaths().empty())
						{
							return;
						}

						DirectX::SimpleMath::Matrix preAppliedTransform = renderer.GetPrevApplyTransform();
						renderer.SetPrevApplyTransform(DirectX::SimpleMath::Matrix::Identity);
						transform.SetPreAppliedTransform(preAppliedTransform);
					}
				);
			}


			if (ImGui::Button("all staticMesh apply decal"))
			{
				auto scene = mGameProcess->mSceneManager->GetCurrentScene();

				scene->ViewComponents<game_module::StaticMeshRenderer, game_module::Transform>(
					[](game_module::GameObject& object, game_module::StaticMeshRenderer& renderer, game_module::Transform& transform)
					{
						auto meshObjectInfo = renderer.GetMeshObjectInfomation();
						meshObjectInfo.bIsAppliedDecal = true;
						renderer.SetMeshObjectInfomation(meshObjectInfo);
					}
				);
			}

			if (ImGui::Button("convert terrain texture(jpg, png -> dds"))
			{
				auto scene = mGameProcess->mSceneManager->GetCurrentScene();

				scene->ViewComponents<game_module::Terrain>(
					[](game_module::GameObject& object, game_module::Terrain& terrain)
					{
						auto tryChangeDDSFormat = [](std::string& filename)
							{
								if (filename.empty())
								{
									return;
								}

								std::filesystem::path ddsFilename = filename;
								ddsFilename.replace_extension(".dds");

								if (std::filesystem::exists(ddsFilename))
								{
									filename = ddsFilename.string();
								}
							};

						auto terrainLayers = terrain.GetTerrainLayers();
						for (auto& layer : terrainLayers)
						{
							tryChangeDDSFormat(layer.BaseColor);
							tryChangeDDSFormat(layer.NormalMap);
						}

						terrain.SetTerrainLayers(terrainLayers);
					}
				);
			}

			std::string rewriteBasePathString = mRewriteUVAnimatorPath.string();
			ImGui::InputText("Rewrite UVAnimation Path", &rewriteBasePathString);

			if (ImGui::BeginDragDropTarget())
			{
				const ImGuiPayload* pathPayLoad = ImGui::AcceptDragDropPayload("Path");

				if (pathPayLoad)
				{
					std::filesystem::path* path
						= static_cast<std::filesystem::path*>(pathPayLoad->Data);

					if (std::filesystem::exists(*path) && path->extension() == ".uvAnimation")
					{
						mRewriteUVAnimatorPath = *path;
					}
				}
			}

			if (ImGui::Button("Reverse X-axis of the UVAnimation file"))
			{
				auto uvAnimation = mGameProcess->mGraphics->ReadUVAnimation(mRewriteUVAnimatorPath.string());
				for (auto& [key, clip] : uvAnimation.NodeClips)
				{
					for (auto& keyframe : clip.UVData)
					{
						keyframe.Translation.x *= -1;
					}
				}
				mGameProcess->mGraphics->WriteUVAnimation(mRewriteUVAnimatorPath.string(), uvAnimation);

				auto iUVAnimation = mGameProcess->mResourceSystem->GetUVAnimation(mRewriteUVAnimatorPath.string());
				if (iUVAnimation != nullptr)
				{
					iUVAnimation->SetUVAnimationClip(uvAnimation);
				}
			}
			if (ImGui::Button("Change X-axis minus 1 of the UVAnimation file"))
			{
				auto uvAnimation = mGameProcess->mGraphics->ReadUVAnimation(mRewriteUVAnimatorPath.string());
				for (auto& [key, clip] : uvAnimation.NodeClips)
				{
					for (auto& keyframe : clip.UVData)
					{
						keyframe.Translation.x = 1 - keyframe.Translation.x;
					}
				}
				mGameProcess->mGraphics->WriteUVAnimation(mRewriteUVAnimatorPath.string(), uvAnimation);

				auto iUVAnimation = mGameProcess->mResourceSystem->GetUVAnimation(mRewriteUVAnimatorPath.string());
				if (iUVAnimation != nullptr)
				{
					iUVAnimation->SetUVAnimationClip(uvAnimation);
				}
			}

			if (ImGui::Button("Reverse Y-axis of the UVAnimation file"))
			{
				auto uvAnimation = mGameProcess->mGraphics->ReadUVAnimation(mRewriteUVAnimatorPath.string());
				for (auto& [key, clip] : uvAnimation.NodeClips)
				{
					for (auto& keyframe : clip.UVData)
					{
						keyframe.Translation.y *= -1;
					}
				}
				mGameProcess->mGraphics->WriteUVAnimation(mRewriteUVAnimatorPath.string(), uvAnimation);

				auto iUVAnimation = mGameProcess->mResourceSystem->GetUVAnimation(mRewriteUVAnimatorPath.string());
				if (iUVAnimation != nullptr)
				{
					iUVAnimation->SetUVAnimationClip(uvAnimation);
				}
			}
			if (ImGui::Button("Change Y-axis minus 1 of the UVAnimation file"))
			{
				auto uvAnimation = mGameProcess->mGraphics->ReadUVAnimation(mRewriteUVAnimatorPath.string());
				for (auto& [key, clip] : uvAnimation.NodeClips)
				{
					for (auto& keyframe : clip.UVData)
					{
						keyframe.Translation.y = 1 - keyframe.Translation.y;
					}
				}
				mGameProcess->mGraphics->WriteUVAnimation(mRewriteUVAnimatorPath.string(), uvAnimation);

				auto iUVAnimation = mGameProcess->mResourceSystem->GetUVAnimation(mRewriteUVAnimatorPath.string());
				if (iUVAnimation != nullptr)
				{
					iUVAnimation->SetUVAnimationClip(uvAnimation);
				}
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

std::wstring fq::game_engine::Setting::ChangeDDSFormat(GameProcess* gameProcess, const std::wstring& fileName)
{
	// 이미 확장자가 dds라면 아무것도 안함
	if (std::filesystem::path(fileName).extension() == ".dds")
	{
		return fileName;
	}

	std::filesystem::path ddsFileName = fileName;
	ddsFileName.replace_extension(".dds");

	// 이미 변환된 파일이 있다면 기존 파일을 지운 후 dds파일 이름을 반환
	if (std::filesystem::exists(ddsFileName))
	{
		std::filesystem::remove(fileName);
		return ddsFileName;
	}

	// 파일이 존재하지 않으면 아무 처리도 하지 않음
	if (!std::filesystem::exists(fileName))
	{
		return fileName;
	}

	// 현재 이름으로 텍스처 생성
	auto textureInterface = gameProcess->mGraphics->CreateTexture(fileName);

	if (textureInterface == nullptr)
	{
		return fileName;
	}

	// 실패 시 기존 이름 반환
	if (!gameProcess->mGraphics->SaveDDS(textureInterface, ddsFileName))
	{
		return fileName;
	}

	// 성공 시 이름 반환 및 기존 텍스처 삭제
	bool isRemove = std::filesystem::remove(fileName);
	assert(isRemove);

	return ddsFileName;
}

