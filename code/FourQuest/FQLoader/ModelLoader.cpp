#include "pch.h"

#include <DirectXCollision.h>

#include "ModelLoader.h"
#include "FileUtil.h"

namespace fq::loader
{
	fq::common::Model ModelLoader::Read(const std::string& fileName)
	{
		using namespace std;
		using namespace fq::common;

		Model model;

		FileUtil fileUtil;
		fileUtil.Open(fileName, EFileMode::Read);

		model.Meshes = ReadMesh(fileUtil);
		model.Materials = ReadMaterial(fileUtil);
		model.Animations = ReadAnimation(fileUtil);

		return model;
	}

	void ModelLoader::Write(const fq::common::Model& modelData, const std::string& fileName)
	{
		using namespace std;
		using namespace fq::common;

		filesystem::path path = fileName;
		filesystem::create_directory(path.parent_path());
		path.replace_extension(".model");

		FileUtil fileUtil;
		fileUtil.Open(path.string(), EFileMode::Write);

		WriteMeshByData(modelData.Meshes, fileUtil);
		WriteMaterialByData(modelData.Materials, fileUtil);
		WriteAnimationByData(modelData.Animations, fileUtil);
	}

	std::vector<std::pair<fq::common::Node, fq::common::Mesh>> ModelLoader::ReadMesh(FileUtil& fileUtil)
	{
		using namespace std;
		using namespace fq::common;

		vector<pair<Node, Mesh>> modelData;

		unsigned int nodeCount;
		fileUtil.Read<unsigned int>(&nodeCount);

		modelData.reserve(nodeCount);

		for (unsigned int i = 0; i < nodeCount; ++i)
		{
			Node node;

			fileUtil.Read<string>(&node.Name);
			fileUtil.Read<unsigned int>(&node.Index);
			fileUtil.Read<unsigned int>(&node.ParentIndex);
			fileUtil.Read<DirectX::SimpleMath::Matrix>(&node.ToParentMatrix);
			fileUtil.Read<DirectX::SimpleMath::Matrix>(&node.OffsetMatrix);

			Mesh mesh;
			fileUtil.Read<string>(&mesh.Name);

			unsigned int vertexCount;
			fileUtil.Read<unsigned int>(&vertexCount);
			mesh.Vertices.resize(vertexCount);
			fileUtil.Read(mesh.Vertices.data(), sizeof(Mesh::Vertex) * vertexCount);

			unsigned int boneVertexCount;
			fileUtil.Read<unsigned int>(&boneVertexCount);
			mesh.BoneVertices.resize(boneVertexCount);
			fileUtil.Read(mesh.BoneVertices.data(), sizeof(Mesh::BoneVertex) * boneVertexCount);
			assert(boneVertexCount == 0 || (boneVertexCount != 0 && boneVertexCount == vertexCount));

			unsigned int indexCount;
			fileUtil.Read<unsigned int>(&indexCount);
			mesh.Indices.resize(indexCount);
			fileUtil.Read(mesh.Indices.data(), sizeof(unsigned int) * indexCount);

			fileUtil.Read<string>(&mesh.NodeName);

			unsigned int boneCount;
			fileUtil.Read<unsigned int>(&boneCount);
			mesh.Bones.reserve(boneCount);

			for (size_t j = 0; j < boneCount; ++j)
			{
				Mesh::Bone bone;
				fileUtil.Read<string>(&bone.Name);
				fileUtil.Read<unsigned int>(&bone.NodeIndex);

				mesh.Bones.push_back(bone);
			}

			unsigned int subsetCount;
			fileUtil.Read<unsigned int>(&subsetCount);
			mesh.Subsets.reserve(subsetCount);

			for (unsigned int j = 0; j < subsetCount; ++j)
			{
				Mesh::Subset subset;

				fileUtil.Read<unsigned int>(&subset.VertexStart);
				fileUtil.Read<unsigned int>(&subset.VertexCount);
				fileUtil.Read<unsigned int>(&subset.IndexStart);
				fileUtil.Read<unsigned int>(&subset.IndexCount);
				fileUtil.Read<string>(&subset.MaterialName);

				mesh.Subsets.push_back(subset);
			}

			fileUtil.Read<DirectX::BoundingBox>(&mesh.RenderBoundingBox);
			fileUtil.Read<DirectX::BoundingSphere>(&mesh.RenderBoundingSphere);

			unsigned int tex1Count;
			fileUtil.Read<unsigned int>(&tex1Count);
			mesh.Tex1.reserve(tex1Count);

			for (unsigned int j = 0; j < tex1Count; ++j)
			{
				DirectX::SimpleMath::Vector2 tex1;

				fileUtil.Read<DirectX::SimpleMath::Vector2>(&tex1);

				mesh.Tex1.push_back(tex1);
			}

			unsigned int dynamicDataCount = 0;
			fileUtil.Read<unsigned int>(&dynamicDataCount);

			for (unsigned int j = 0; j < dynamicDataCount; ++j)
			{
				Mesh::DynamicData dynamicData;

				fileUtil.Read<string>(&dynamicData.Name);
				fileUtil.Read<unsigned int>(&dynamicData.Size);
				fileUtil.Read<unsigned int>(&dynamicData.Count);
				dynamicData.Data.resize(dynamicData.Size * dynamicData.Count);
				fileUtil.Read(dynamicData.Data.data(), dynamicData.Size * dynamicData.Count);

				mesh.DynamicInfos.insert({ dynamicData.Name, dynamicData });
			}

			modelData.push_back({ std::move(node), std::move(mesh) });
		}

		return modelData;
	}
	std::vector<fq::common::Material> ModelLoader::ReadMaterial(FileUtil& fileUtil)
	{
		using namespace std;
		using namespace fq::common;

		unsigned int materialCount;
		fileUtil.Read<unsigned int>(&materialCount);

		vector<Material> materials;
		materials.reserve(materialCount);

		for (size_t i = 0; i < materialCount; ++i)
		{
			Material material;

			fileUtil.Read<DirectX::SimpleMath::Color>(&material.BaseColor);
			fileUtil.Read<float>(&material.Metalness);
			fileUtil.Read<float>(&material.Roughness);

			fileUtil.Read<string>(&material.Name);
			fileUtil.Read<wstring>(&material.BaseColorFileName);
			fileUtil.Read<wstring>(&material.MetalnessFileName);
			fileUtil.Read<wstring>(&material.RoughnessFileName);
			fileUtil.Read<wstring>(&material.NormalFileName);
			fileUtil.Read<wstring>(&material.EmissiveFileName);
			fileUtil.Read<wstring>(&material.OpacityFileName);

			materials.push_back(material);
		}

		return materials;
	}
	std::vector<fq::common::AnimationClip> ModelLoader::ReadAnimation(FileUtil& fileUtil)
	{
		using namespace std;
		using namespace fq::common;

		vector<AnimationClip> animationClips;

		unsigned int animationClipCount;
		fileUtil.Read<unsigned int>(&animationClipCount);
		animationClips.reserve(animationClipCount);

		for (unsigned int i = 0; i < animationClipCount; ++i)
		{
			AnimationClip animationClip;

			fileUtil.Read<string>(&animationClip.Name);
			fileUtil.Read<unsigned int>(&animationClip.FrameCount);
			fileUtil.Read<float>(&animationClip.FramePerSecond);
			fileUtil.Read<float>(&animationClip.Duration);

			unsigned int nodeClipCount;
			fileUtil.Read<unsigned int>(&nodeClipCount);

			for (unsigned int j = 0; j < nodeClipCount; ++j)
			{
				NodeClip nodeClip;

				fileUtil.Read<string>(&nodeClip.NodeName);

				unsigned int keyframeCount;
				fileUtil.Read<unsigned int>(&keyframeCount);

				for (unsigned int k = 0; k < keyframeCount; ++k)
				{
					Keyframe keyframe;

					fileUtil.Read<float>(&keyframe.TimePos);
					fileUtil.Read<DirectX::SimpleMath::Vector3>(&keyframe.Translation);
					fileUtil.Read<DirectX::SimpleMath::Vector3>(&keyframe.Scale);
					fileUtil.Read<DirectX::SimpleMath::Quaternion>(&keyframe.Rotation);

					nodeClip.Keyframes.push_back(keyframe);
				}

				animationClip.NodeClips.push_back(nodeClip);
			}

			animationClips.push_back(animationClip);
		}

		return animationClips;
	}


	void ModelLoader::WriteMeshByData(const std::vector<std::pair<fq::common::Node, fq::common::Mesh>>& modelData, FileUtil& fileUtil)
	{
		using namespace std;
		using namespace fq::common;

		fileUtil.Write<unsigned int>(modelData.size());

		for (const pair<Node, Mesh>& nodeMeshPair : modelData)
		{
			const Node& node = nodeMeshPair.first;
			const Mesh& mesh = nodeMeshPair.second;

			fileUtil.Write<string>(node.Name);
			fileUtil.Write<unsigned int>(node.Index);
			fileUtil.Write<unsigned int>(node.ParentIndex);
			fileUtil.Write<DirectX::SimpleMath::Matrix>(node.ToParentMatrix);
			fileUtil.Write<DirectX::SimpleMath::Matrix>(node.OffsetMatrix);

			fileUtil.Write<string>(mesh.Name);
			fileUtil.Write<unsigned int>(mesh.Vertices.size());
			fileUtil.Write(mesh.Vertices.data(), mesh.Vertices.size() * sizeof(Mesh::Vertex));

			fileUtil.Write<unsigned int>(mesh.BoneVertices.size());
			fileUtil.Write(mesh.BoneVertices.data(), mesh.BoneVertices.size() * sizeof(Mesh::BoneVertex));
			assert(mesh.BoneVertices.empty() || (!mesh.BoneVertices.empty() && mesh.BoneVertices.size() == mesh.Vertices.size()));

			fileUtil.Write<unsigned int>(mesh.Indices.size());
			fileUtil.Write(mesh.Indices.data(), mesh.Indices.size() * sizeof(unsigned int));

			fileUtil.Write<string>(mesh.NodeName);

			fileUtil.Write<unsigned int>(mesh.Bones.size());
			for (const Mesh::Bone& bone : mesh.Bones)
			{
				fileUtil.Write<string>(bone.Name);
				fileUtil.Write<unsigned int>(bone.NodeIndex);
			}

			fileUtil.Write<unsigned int>(mesh.Subsets.size());
			for (const auto& subset : mesh.Subsets)
			{
				fileUtil.Write<unsigned int>(subset.VertexStart);
				fileUtil.Write<unsigned int>(subset.VertexCount);
				fileUtil.Write<unsigned int>(subset.IndexStart);
				fileUtil.Write<unsigned int>(subset.IndexCount);
				fileUtil.Write<string>(subset.MaterialName);
			}

			fileUtil.Write<DirectX::BoundingBox>(mesh.RenderBoundingBox);
			fileUtil.Write<DirectX::BoundingSphere>(mesh.RenderBoundingSphere);

			fileUtil.Write<unsigned int>(mesh.Tex1.size());
			for (const DirectX::SimpleMath::Vector2& tex1 : mesh.Tex1)
			{
				fileUtil.Write<DirectX::SimpleMath::Vector2>(tex1);
			}

			fileUtil.Write<unsigned int>(mesh.DynamicInfos.size());
			for (const auto& [key, dynamicData] : mesh.DynamicInfos)
			{
				fileUtil.Write<string>(dynamicData.Name);
				fileUtil.Write<unsigned int>(dynamicData.Size);
				fileUtil.Write<unsigned int>(dynamicData.Count);
				fileUtil.Write(dynamicData.Data.data(), dynamicData.Size * dynamicData.Count);
			}
		}
	}
	void ModelLoader::WriteMaterialByData(const std::vector<fq::common::Material>& materialData, FileUtil& fileUtil)
	{
		using namespace std;
		using namespace fq::common;

		fileUtil.Write<unsigned int>(materialData.size());

		for (const Material& material : materialData)
		{
			fileUtil.Write<DirectX::SimpleMath::Color>(material.BaseColor);
			fileUtil.Write<float>(material.Metalness);
			fileUtil.Write<float>(material.Roughness);

			fileUtil.Write<string>(material.Name);
			fileUtil.Write<wstring>(material.BaseColorFileName);
			fileUtil.Write<wstring>(material.MetalnessFileName);
			fileUtil.Write<wstring>(material.RoughnessFileName);
			fileUtil.Write<wstring>(material.NormalFileName);
			fileUtil.Write<wstring>(material.EmissiveFileName);
			fileUtil.Write<wstring>(material.OpacityFileName);
		}
	}
	void ModelLoader::WriteAnimationByData(const std::vector<fq::common::AnimationClip>& animationData, FileUtil& fileUtil)
	{
		using namespace std;
		using namespace fq::common;

		fileUtil.Write<unsigned int>(animationData.size());

		for (const AnimationClip& animationClip : animationData)
		{
			fileUtil.Write<string>(animationClip.Name);
			fileUtil.Write<unsigned int>(animationClip.FrameCount);
			fileUtil.Write<float>(animationClip.FramePerSecond);
			fileUtil.Write<float>(animationClip.Duration);

			fileUtil.Write<unsigned int>(animationClip.NodeClips.size());

			for (const NodeClip& nodeClip : animationClip.NodeClips)
			{
				fileUtil.Write<string>(nodeClip.NodeName);

				fileUtil.Write<unsigned int>(nodeClip.Keyframes.size());

				for (const Keyframe& keyframe : nodeClip.Keyframes)
				{
					fileUtil.Write<float>(keyframe.TimePos);
					fileUtil.Write<DirectX::SimpleMath::Vector3>(keyframe.Translation);
					fileUtil.Write<DirectX::SimpleMath::Vector3>(keyframe.Scale);
					fileUtil.Write<DirectX::SimpleMath::Quaternion>(keyframe.Rotation);
				}
			}
		}
	}
	fq::common::UVAnimationClip UVAnimationLoader::Read(const std::filesystem::path& filePath)
	{
		assert(std::filesystem::exists(filePath));

		std::ifstream readData(filePath);
		nlohmann::ordered_json controllerJson;

		if (readData.is_open())
		{
			readData >> controllerJson;
			readData.close();
		}
		else
			assert(!"파일 열기 실패");


		fq::common::UVAnimationClip result;

		result.FrameCount = controllerJson.at("FrameCount").get<int>();
		result.FramePerSecond = controllerJson.at("FramePerSecond").get<float>();
		result.Duration = controllerJson.at("Duration").get<float>();

		auto objects = controllerJson.find("objects");

		for (const auto& [key, value] : objects.value().items())
		{
			fq::common::UVNodeClip uvNodeClip;
			uvNodeClip.NodeName = value.at("object");
			uvNodeClip.UVData.reserve(result.FrameCount);
			auto keyFrameData = value.find("uv_animation_data");

			for (const auto& [innerKey, innerValue] : keyFrameData.value().items())
			{
				fq::common::UVKeyframe uvKeyframe;

				uvKeyframe.TimePos = innerValue.at("time").get<float>();

				uvKeyframe.Translation.x = innerValue.at("translate")[0].get<float>();
				uvKeyframe.Translation.y = innerValue.at("translate")[1].get<float>();

				uvKeyframe.Rotation = innerValue.at("rotate")[0].get<float>();

				uvKeyframe.Scale.x = innerValue.at("scale")[0].get<float>();
				uvKeyframe.Scale.y = innerValue.at("scale")[1].get<float>();

				uvNodeClip.UVData.push_back(uvKeyframe);
			}

			result.NodeClips.insert({ uvNodeClip.NodeName, uvNodeClip });
		}

		return result;
	}
	void UVAnimationLoader::Write(const fq::common::UVAnimationClip& uvAnimationClip, const std::string& filePath)
	{
		nlohmann::ordered_json controllerJson;

		// UVAnimationClip 데이터 채우기
		controllerJson["FrameCount"] = uvAnimationClip.FrameCount;
		controllerJson["FramePerSecond"] = uvAnimationClip.FramePerSecond;
		controllerJson["Duration"] = uvAnimationClip.Duration;

		nlohmann::ordered_json objectsJson;

		for (const auto& [nodeName, uvNodeClip] : uvAnimationClip.NodeClips)
		{
			nlohmann::ordered_json uvNodeClipJson;
			uvNodeClipJson["object"] = uvNodeClip.NodeName;

			nlohmann::ordered_json uvAnimationDataJson;

			for (const auto& uvKeyframe : uvNodeClip.UVData)
			{
				nlohmann::ordered_json uvKeyframeJson;
				uvKeyframeJson["time"] = uvKeyframe.TimePos;

				uvKeyframeJson["translate"] = { uvKeyframe.Translation.x, uvKeyframe.Translation.y };
				uvKeyframeJson["rotate"] = { uvKeyframe.Rotation };
				uvKeyframeJson["scale"] = { uvKeyframe.Scale.x, uvKeyframe.Scale.y };

				uvAnimationDataJson.push_back(uvKeyframeJson);
			}

			uvNodeClipJson["uv_animation_data"] = uvAnimationDataJson;
			objectsJson[uvNodeClip.NodeName] = uvNodeClipJson;
		}

		controllerJson["objects"] = objectsJson;

		// JSON 파일로 쓰기
		std::ofstream writeData(filePath);
		if (writeData.is_open())
		{
			writeData << controllerJson.dump(4); // 4는 JSON 파일의 들여쓰기(indentation) 레벨을 의미합니다.
			writeData.close();
		}
		else
		{
			assert(!"파일 쓰기 실패");
		}
	}

	fq::common::AnimationClip AnimationLoader::Read(const std::filesystem::path& filePath)
	{
		std::ifstream readData(filePath);
		nlohmann::ordered_json controllerJson;

		if (readData.is_open())
		{
			readData >> controllerJson;
			readData.close();
		}
		else
			assert(!"파일 열기 실패");


		fq::common::AnimationClip result;

		result.FrameCount = controllerJson.at("FrameCount").get<int>();
		result.FramePerSecond = controllerJson.at("FramePerSecond").get<float>();
		result.Duration = controllerJson.at("Duration").get<float>();

		auto objects = controllerJson.find("objects");

		for (const auto& [key, value] : objects.value().items())
		{
			fq::common::NodeClip nodeClip;
			nodeClip.NodeName = value.at("object");
			nodeClip.Keyframes.reserve(result.FrameCount);
			auto keyFrameData = value.find("transform_data");

			for (const auto& [innerKey, innerValue] : keyFrameData.value().items())
			{
				fq::common::Keyframe keyframe;

				keyframe.TimePos = innerValue.at("time").get<float>();

				keyframe.Translation.x = innerValue.at("location")[0].get<float>();
				keyframe.Translation.y = innerValue.at("location")[1].get<float>();
				keyframe.Translation.z = innerValue.at("location")[2].get<float>();

				keyframe.Rotation.x = innerValue.at("rotation")[0].get<float>();
				keyframe.Rotation.y = innerValue.at("rotation")[1].get<float>();
				keyframe.Rotation.z = innerValue.at("rotation")[2].get<float>();
				keyframe.Rotation.w = innerValue.at("rotation")[3].get<float>();

				keyframe.Scale.x = innerValue.at("scale")[0].get<float>();
				keyframe.Scale.y = innerValue.at("scale")[1].get<float>();
				keyframe.Scale.z = innerValue.at("scale")[2].get<float>();

				nodeClip.Keyframes.push_back(keyframe);
			}

			result.NodeClips.push_back(nodeClip);
		}

		return result;
	}

	void AnimationLoader::Write(const fq::common::AnimationClip& animationClip, const std::string& filePath)
	{
		nlohmann::ordered_json controllerJson;

		// AnimationClip 데이터 채우기
		controllerJson["FrameCount"] = animationClip.FrameCount;
		controllerJson["FramePerSecond"] = animationClip.FramePerSecond;
		controllerJson["Duration"] = animationClip.Duration;

		nlohmann::ordered_json objectsJson;

		for (const auto& nodeClip : animationClip.NodeClips)
		{
			nlohmann::ordered_json nodeClipJson;
			nodeClipJson["object"] = nodeClip.NodeName;

			nlohmann::ordered_json transformDataJson;

			for (const auto& keyframe : nodeClip.Keyframes)
			{
				nlohmann::ordered_json keyframeJson;
				keyframeJson["time"] = keyframe.TimePos;

				keyframeJson["location"] = { keyframe.Translation.x, keyframe.Translation.y, keyframe.Translation.z };
				keyframeJson["rotation"] = { keyframe.Rotation.x, keyframe.Rotation.y, keyframe.Rotation.z, keyframe.Rotation.w };
				keyframeJson["scale"] = { keyframe.Scale.x, keyframe.Scale.y, keyframe.Scale.z };

				transformDataJson.push_back(keyframeJson);
			}

			nodeClipJson["transform_data"] = transformDataJson;
			objectsJson[nodeClip.NodeName] = nodeClipJson;
		}

		controllerJson["objects"] = objectsJson;

		// JSON 파일로 쓰기
		std::ofstream writeData(filePath);
		if (writeData.is_open())
		{
			writeData << controllerJson.dump(4); // 4는 JSON 파일의 들여쓰기(indentation) 레벨을 의미합니다.
			writeData.close();
		}
		else
		{
			assert(!"파일 쓰기 실패");
		}
	}

	std::vector<fq::common::Node> NodeHierarchyLoader::Read(const std::filesystem::path& filePath)
	{
		std::ifstream readData(filePath);
		nlohmann::ordered_json nodeHierarchyJson;

		if (readData.is_open())
		{
			readData >> nodeHierarchyJson;
			readData.close();
		}
		else
			assert(!"파일 열기 실패");

		auto ReadMatrix = [](const nlohmann::ordered_json& matrixJson) {
			DirectX::SimpleMath::Matrix matrix;

			for (int i = 0; i < 4; ++i) {
				for (int j = 0; j < 4; ++j) {
					matrix.m[i][j] = matrixJson[i * 4 + j].get<float>();
				}
			}
			return matrix;
			};

		std::vector<fq::common::Node> nodeHierarchy;

		for (const auto& nodeJson : nodeHierarchyJson["nodes"])
		{
			fq::common::Node node;
			node.Name = nodeJson["Name"].get<std::string>();
			node.Index = nodeJson["Index"].get<int>();
			node.ParentIndex = nodeJson["ParentIndex"].get<int>();
			node.ToParentMatrix = ReadMatrix(nodeJson["ToParentMatrix"]);
			node.OffsetMatrix = ReadMatrix(nodeJson["OffsetMatrix"]);
			nodeHierarchy.push_back(node);
		}

		return nodeHierarchy;
	}

	void NodeHierarchyLoader::Write(const  std::vector<fq::common::Node>& nodeHierarchy, const std::string& filePath)
	{
		nlohmann::ordered_json nodeHierarchyJson;
		{
			nlohmann::ordered_json nodesJson;

			for (const auto& node : nodeHierarchy)
			{
				nlohmann::ordered_json nodeJson;

				nodeJson["Name"] = node.Name;
				nodeJson["Index"] = node.Index;
				nodeJson["ParentIndex"] = node.ParentIndex;
				nodeJson["ToParentMatrix"] = {
					node.ToParentMatrix.m[0][0], node.ToParentMatrix.m[0][1],  node.ToParentMatrix.m[0][2], node.ToParentMatrix.m[0][3],
					node.ToParentMatrix.m[1][0], node.ToParentMatrix.m[1][1],  node.ToParentMatrix.m[1][2], node.ToParentMatrix.m[1][3],
					node.ToParentMatrix.m[2][0], node.ToParentMatrix.m[2][1],  node.ToParentMatrix.m[2][2], node.ToParentMatrix.m[2][3],
					node.ToParentMatrix.m[3][0], node.ToParentMatrix.m[3][1],  node.ToParentMatrix.m[3][2], node.ToParentMatrix.m[3][3],
				};
				nodeJson["OffsetMatrix"] =
				{
					node.OffsetMatrix.m[0][0], node.OffsetMatrix.m[0][1],  node.OffsetMatrix.m[0][2], node.OffsetMatrix.m[0][3],
					node.OffsetMatrix.m[1][0], node.OffsetMatrix.m[1][1],  node.OffsetMatrix.m[1][2], node.OffsetMatrix.m[1][3],
					node.OffsetMatrix.m[2][0], node.OffsetMatrix.m[2][1],  node.OffsetMatrix.m[2][2], node.OffsetMatrix.m[2][3],
					node.OffsetMatrix.m[3][0], node.OffsetMatrix.m[3][1],  node.OffsetMatrix.m[3][2], node.OffsetMatrix.m[3][3],
				};

				nodesJson.push_back(nodeJson);
			}

			nodeHierarchyJson["nodes"] = nodesJson;
		}

		// JSON 파일로 쓰기
		std::ofstream writeData(filePath);
		if (writeData.is_open())
		{
			writeData << nodeHierarchyJson.dump(4); // 4는 JSON 파일의 들여쓰기(indentation) 레벨을 의미합니다.
			writeData.close();
		}
		else
		{
			assert(!"파일 쓰기 실패");
		}
	}

	void NodeHierarchyLoader::Write(const fq::common::Model& modeData, const std::string& filePath)
	{
		nlohmann::ordered_json nodeHierarchyJson;
		{
			nlohmann::ordered_json nodesJson;

			for (const auto& [node, mesh] : modeData.Meshes)
			{
				nlohmann::ordered_json nodeJson;

				nodeJson["Name"] = node.Name;
				nodeJson["Index"] = node.Index;
				nodeJson["ParentIndex"] = node.ParentIndex;
				nodeJson["ToParentMatrix"] = {
					node.ToParentMatrix.m[0][0], node.ToParentMatrix.m[0][1],  node.ToParentMatrix.m[0][2], node.ToParentMatrix.m[0][3],
					node.ToParentMatrix.m[1][0], node.ToParentMatrix.m[1][1],  node.ToParentMatrix.m[1][2], node.ToParentMatrix.m[1][3],
					node.ToParentMatrix.m[2][0], node.ToParentMatrix.m[2][1],  node.ToParentMatrix.m[2][2], node.ToParentMatrix.m[2][3],
					node.ToParentMatrix.m[3][0], node.ToParentMatrix.m[3][1],  node.ToParentMatrix.m[3][2], node.ToParentMatrix.m[3][3],
				};
				nodeJson["OffsetMatrix"] =
				{
					node.OffsetMatrix.m[0][0], node.OffsetMatrix.m[0][1],  node.OffsetMatrix.m[0][2], node.OffsetMatrix.m[0][3],
					node.OffsetMatrix.m[1][0], node.OffsetMatrix.m[1][1],  node.OffsetMatrix.m[1][2], node.OffsetMatrix.m[1][3],
					node.OffsetMatrix.m[2][0], node.OffsetMatrix.m[2][1],  node.OffsetMatrix.m[2][2], node.OffsetMatrix.m[2][3],
					node.OffsetMatrix.m[3][0], node.OffsetMatrix.m[3][1],  node.OffsetMatrix.m[3][2], node.OffsetMatrix.m[3][3],
				};

				nodesJson.push_back(nodeJson);
			}

			nodeHierarchyJson["nodes"] = nodesJson;
		}

		// JSON 파일로 쓰기
		std::ofstream writeData(filePath);
		if (writeData.is_open())
		{
			writeData << nodeHierarchyJson.dump(4); // 4는 JSON 파일의 들여쓰기(indentation) 레벨을 의미합니다.
			writeData.close();
		}
		else
		{
			assert(!"파일 쓰기 실패");
		}
	}

	fq::graphics::MaterialInfo MaterialLoader::Read(const std::filesystem::path& filePath)
	{
		std::ifstream readData(filePath);
		nlohmann::ordered_json materialInfoJson;

		if (readData.is_open())
		{
			readData >> materialInfoJson;
			readData.close();
		}
		else
			assert(!"파일 열기 실패");

		using namespace fq::graphics;

		fq::graphics::MaterialInfo materialInfo;

		std::string renderModeStr, sampleTypeStr, rasterizeTypeStr;
		materialInfoJson.at("RenderModeType").get_to(renderModeStr);
		materialInfo.RenderModeType = (renderModeStr == "Opaque") ? MaterialInfo::ERenderMode::Opaque : MaterialInfo::ERenderMode::Transparent;

		materialInfo.BaseColor.x = materialInfoJson.at("BaseColor")[0].get<float>();
		materialInfo.BaseColor.y = materialInfoJson.at("BaseColor")[1].get<float>();
		materialInfo.BaseColor.z = materialInfoJson.at("BaseColor")[2].get<float>();
		materialInfo.BaseColor.w = materialInfoJson.at("BaseColor")[3].get<float>();
		materialInfoJson.at("Metalness").get_to(materialInfo.Metalness);
		materialInfoJson.at("Roughness").get_to(materialInfo.Roughness);
		materialInfo.EmissiveColor.x = materialInfoJson.at("EmissiveColor")[0].get<float>();
		materialInfo.EmissiveColor.y = materialInfoJson.at("EmissiveColor")[1].get<float>();
		materialInfo.EmissiveColor.z = materialInfoJson.at("EmissiveColor")[2].get<float>();
		materialInfo.EmissiveColor.w = materialInfoJson.at("EmissiveColor")[3].get<float>();

		std::string temp;
		materialInfoJson.at("BaseColorFileName").get_to(temp);
		materialInfo.BaseColorFileName = std::wstring(temp.begin(), temp.end());

		materialInfoJson.at("MetalnessFileName").get_to(temp);
		materialInfo.MetalnessFileName = std::wstring(temp.begin(), temp.end());

		materialInfoJson.at("RoughnessFileName").get_to(temp);
		materialInfo.RoughnessFileName = std::wstring(temp.begin(), temp.end());

		materialInfoJson.at("NormalFileName").get_to(temp);
		materialInfo.NormalFileName = std::wstring(temp.begin(), temp.end());

		materialInfoJson.at("EmissiveFileName").get_to(temp);
		materialInfo.EmissiveFileName = std::wstring(temp.begin(), temp.end());

		materialInfoJson.at("bUseBaseColor").get_to(materialInfo.bUseBaseColor);
		materialInfoJson.at("bUseMetalness").get_to(materialInfo.bUseMetalness);
		materialInfoJson.at("bUseRoughness").get_to(materialInfo.bUseRoughness);
		materialInfoJson.at("bUseNormalness").get_to(materialInfo.bUseNormalness);
		materialInfoJson.at("bIsUsedEmissive").get_to(materialInfo.bIsUsedEmissive);

		materialInfo.Tiling.x = materialInfoJson.at("Tiling")[0].get<float>();
		materialInfo.Tiling.y = materialInfoJson.at("Tiling")[1].get<float>();
		materialInfo.Offset.x = materialInfoJson.at("Offset")[0].get<float>();
		materialInfo.Offset.y = materialInfoJson.at("Offset")[1].get<float>();

		materialInfoJson.at("AlphaCutoff").get_to(materialInfo.AlphaCutoff);

		materialInfoJson.at("SampleType").get_to(sampleTypeStr);
		materialInfo.SampleType = (sampleTypeStr == "Clamp") ? ESampleMode::Clamp : ESampleMode::Wrap;

		materialInfoJson.at("RasterizeType").get_to(rasterizeTypeStr);
		materialInfo.RasterizeType = (rasterizeTypeStr == "BackFaceClip") ? ERasterizeMode::BackFaceClip : ERasterizeMode::TwoSide;

		auto find = materialInfoJson.find("bUseDissolve");
		if (find != materialInfoJson.end())
		{
			find->get_to(materialInfo.bUseDissolve);
		}

		find = materialInfoJson.find("NoiseFileName");
		if (find != materialInfoJson.end())
		{
			materialInfoJson.at("NoiseFileName").get_to(temp);
			materialInfo.NoiseFileName = std::wstring(temp.begin(), temp.end());
		}

		find = materialInfoJson.find("OutlineThickness");
		if (find != materialInfoJson.end())
		{
			materialInfoJson.at("OutlineThickness").get_to(materialInfo.OutlineThickness);
		}

		find = materialInfoJson.find("DissolveCutoff");
		if (find != materialInfoJson.end())
		{
			materialInfoJson.at("DissolveCutoff").get_to(materialInfo.DissolveCutoff);
		}

		find = materialInfoJson.find("DissolveStartColor");
		if (find != materialInfoJson.end())
		{
			materialInfo.DissolveStartColor.x = materialInfoJson.at("DissolveStartColor")[0].get<float>();
			materialInfo.DissolveStartColor.y = materialInfoJson.at("DissolveStartColor")[1].get<float>();
			materialInfo.DissolveStartColor.z = materialInfoJson.at("DissolveStartColor")[2].get<float>();
			materialInfo.DissolveStartColor.w = materialInfoJson.at("DissolveStartColor")[3].get<float>();
		}

		find = materialInfoJson.find("DissolveEndColor");
		if (find != materialInfoJson.end())
		{
			materialInfo.DissolveEndColor.x = materialInfoJson.at("DissolveEndColor")[0].get<float>();
			materialInfo.DissolveEndColor.y = materialInfoJson.at("DissolveEndColor")[1].get<float>();
			materialInfo.DissolveEndColor.z = materialInfoJson.at("DissolveEndColor")[2].get<float>();
			materialInfo.DissolveEndColor.w = materialInfoJson.at("DissolveEndColor")[3].get<float>();
		}

		return materialInfo;
	}
	void MaterialLoader::Write(const std::filesystem::path& filePath, const fq::graphics::MaterialInfo& material)
	{
		using namespace fq::graphics;

		nlohmann::ordered_json materialInfoJson;

		materialInfoJson =
		{
			{"RenderModeType", material.RenderModeType == MaterialInfo::ERenderMode::Opaque ? "Opaque" : "Transparent"},
			{"BaseColor", {material.BaseColor.x, material.BaseColor.y, material.BaseColor.z, material.BaseColor.w}},
			{"Metalness", material.Metalness},
			{"Roughness", material.Roughness},
			{"EmissiveColor", {material.EmissiveColor.x, material.EmissiveColor.y, material.EmissiveColor.z, material.EmissiveColor.w}},
			{"BaseColorFileName", std::string(material.BaseColorFileName.begin(), material.BaseColorFileName.end())},
			{"MetalnessFileName", std::string(material.MetalnessFileName.begin(), material.MetalnessFileName.end())},
			{"RoughnessFileName", std::string(material.RoughnessFileName.begin(), material.RoughnessFileName.end())},
			{"NormalFileName", std::string(material.NormalFileName.begin(), material.NormalFileName.end())},
			{"EmissiveFileName", std::string(material.EmissiveFileName.begin(), material.EmissiveFileName.end())},
			{"bUseBaseColor", material.bUseBaseColor},
			{"bUseMetalness", material.bUseMetalness},
			{"bUseRoughness", material.bUseRoughness},
			{"bUseNormalness", material.bUseNormalness},
			{"bIsUsedEmissive", material.bIsUsedEmissive},
			{"Tiling", {material.Tiling.x, material.Tiling.y}},
			{"Offset", {material.Offset.x, material.Offset.y}},
			{"AlphaCutoff", material.AlphaCutoff},
			{"SampleType", material.SampleType == ESampleMode::Clamp ? "Clamp" : "Wrap"},
			{"RasterizeType", material.RasterizeType == ERasterizeMode::BackFaceClip ? "BackFaceClip" : "TwoSide"},
			{"bUseDissolve", material.bUseDissolve },
			{"NoiseFileName", std::string(material.NoiseFileName.begin(),material.NoiseFileName.end()) },
			{"OutlineThickness", material.OutlineThickness },
			{"DissolveCutoff", material.DissolveCutoff },
			{"DissolveStartColor", {material.DissolveStartColor.x, material.DissolveStartColor.y, material.DissolveStartColor.z, material.DissolveStartColor.w}},
			{"DissolveEndColor", {material.DissolveEndColor.x, material.DissolveEndColor.y, material.DissolveEndColor.z, material.DissolveEndColor.w}},
		};

		std::string tempPath = filePath.string();

		for (auto& ch : tempPath)
		{
			if (ch == '|')
			{
				ch = 'a';
			}
		}

		if (!std::filesystem::exists(filePath.parent_path()))
		{
			std::filesystem::create_directory(filePath.parent_path());
		}

		// JSON 파일로 쓰기
		std::ofstream writeData(tempPath);

		if (writeData.is_open())
		{
			writeData << materialInfoJson.dump(4); // 4는 JSON 파일의 들여쓰기(indentation) 레벨을 의미합니다.
			writeData.close();
		}
		else
		{
			assert(!"파일 쓰기 실패");
		}
	}
}