#include "pch.h"
#include "ModelLoader.h"
#include "FileUtil.h"

namespace fq::loader
{
	Model ModelLoader::ReadModel(const std::string& filePath)
	{
		Model model;

		model.Meshes = ReadMesh(filePath + ".mesh");
		model.Materials = ReadMaterial(filePath + ".material");
		model.Animations = ReadAnimation(filePath + ".animation");

		return model;
	}

	std::vector<std::pair<Node, Mesh>> ModelLoader::ReadMesh(const std::string& fileName)
	{
		using namespace std;

		FileUtil fileUtil;
		fileUtil.Open(fileName, EFileMode::Read);

		vector<pair<Node, Mesh>> modelData;

		unsigned int nodeCount;
		fileUtil.Read<unsigned int>(&nodeCount);

		modelData.reserve(nodeCount);

		for (unsigned int i = 0; i < nodeCount; ++i)
		{
			Node node;

			fileUtil.Read<string>(&node.Name);
			fileUtil.Read<unsigned int>(&node.Index);
			fileUtil.Read<unsigned int>(&node.Parentindex);
			fileUtil.Read<DirectX::SimpleMath::Matrix>(&node.ToParentMatrix);

			Mesh mesh;
			fileUtil.Read<string>(&mesh.Name);

			unsigned int vertexCount;
			fileUtil.Read<unsigned int>(&vertexCount);
			mesh.Vertices.resize(vertexCount);
			fileUtil.Read(mesh.Vertices.data(), sizeof(Mesh::Vertex) * vertexCount);

			unsigned int indexCount;
			fileUtil.Read<unsigned int>(&indexCount);
			mesh.Indices.resize(indexCount);
			fileUtil.Read(mesh.Indices.data(), sizeof(unsigned int) * indexCount);

			fileUtil.Read<string>(&mesh.NodeName);
			fileUtil.Read<bool>(&mesh.bHasBone);
			fileUtil.Read<bool>(&mesh.bHasVertex);

			unsigned int boneNameCount;
			fileUtil.Read<unsigned int>(&boneNameCount);
			mesh.BoneNames.reserve(boneNameCount);

			for (size_t j = 0; j < boneNameCount; ++j)
			{
				std::string boneName;
				fileUtil.Read<string>(&boneName);
				mesh.BoneNames.push_back(boneName);
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

			modelData.push_back({ std::move(node), std::move(mesh) });
		}

		return modelData;
	}
	std::vector<Material> ModelLoader::ReadMaterial(const std::string& fileName)
	{
		using namespace std;

		FileUtil fileUtil;
		fileUtil.Open(fileName, EFileMode::Read);

		unsigned int materialCount;
		fileUtil.Read<unsigned int>(&materialCount);

		vector<Material> materials;
		materials.reserve(materialCount);

		for (size_t i = 0; i < materialCount; ++i)
		{
			Material material;

			fileUtil.Read<DirectX::SimpleMath::Color>(&material.MaterialDesc.BaseColor);
			fileUtil.Read<float>(&material.MaterialDesc.Metalness);
			fileUtil.Read<float>(&material.MaterialDesc.Roughness);

			fileUtil.Read<string>(&material.Name);
			fileUtil.Read<string>(&material.BaseColorFileName);
			fileUtil.Read<string>(&material.MetalnessFileName);
			fileUtil.Read<string>(&material.RoughnessFileName);
			fileUtil.Read<string>(&material.NormalFileName);
			fileUtil.Read<string>(&material.EmissiveFileName);
			fileUtil.Read<string>(&material.OpacityFileName);

			materials.push_back(material);
		}

		return materials;
	}
	std::vector<AnimationClip> ModelLoader::ReadAnimation(const std::string& fileName)
	{
		using namespace std;

		vector<AnimationClip> animationClips;

		FileUtil fileUtil;
		fileUtil.Open(fileName, EFileMode::Read);

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

	void ModelLoader::WriteModelByData(const Model& modelData, const std::string& savePath)
	{
		WriteMeshByData(modelData.Meshes, savePath + ".mesh");
		WriteMaterialByData(modelData.Materials, savePath + ".material");
		WriteAnimationByData(modelData.Animations, savePath + "animation");
	}

	void ModelLoader::WriteMeshByData(const std::vector<std::pair<Node, Mesh>>& modelData, const std::string& savePath)
	{
		using namespace std;

		filesystem::path path = savePath;//  +".mesh";
		filesystem::create_directory(path.parent_path());

		FileUtil fileUtil;
		fileUtil.Open(path.string(), EFileMode::Write);

		fileUtil.Write<unsigned int>(modelData.size());

		for (const pair<Node, Mesh>& model : modelData)
		{
			fileUtil.Write<string>(model.first.Name);
			fileUtil.Write<unsigned int>(model.first.Index);
			fileUtil.Write<unsigned int>(model.first.Parentindex);
			fileUtil.Write<DirectX::SimpleMath::Matrix>(model.first.ToParentMatrix);

			fileUtil.Write<string>(model.second.Name);
			fileUtil.Write<unsigned int>(model.second.Vertices.size());
			fileUtil.Write(model.second.Vertices.data(), model.second.Vertices.size() * sizeof(Mesh::Vertex));
			fileUtil.Write<unsigned int>(model.second.Indices.size());
			fileUtil.Write(model.second.Indices.data(), model.second.Indices.size() * sizeof(unsigned int));

			fileUtil.Write<string>(model.second.NodeName);
			fileUtil.Write<bool>(model.second.bHasBone);
			fileUtil.Write<bool>(model.second.bHasVertex);

			fileUtil.Write<unsigned int>(model.second.BoneNames.size());
			for (const auto& boneName : model.second.BoneNames)
			{
				fileUtil.Write<string>(boneName);
			}

			fileUtil.Write<unsigned int>(model.second.Subsets.size());
			for (const auto& subset : model.second.Subsets)
			{
				fileUtil.Write<unsigned int>(subset.VertexStart);
				fileUtil.Write<unsigned int>(subset.VertexCount);
				fileUtil.Write<unsigned int>(subset.IndexStart);
				fileUtil.Write<unsigned int>(subset.IndexCount);
				fileUtil.Write<string>(subset.MaterialName);
			}
		}
	}
	void ModelLoader::WriteMaterialByData(const std::vector<Material>& materialData, const std::string& savePath)
	{
		using namespace std;

		filesystem::path path = savePath;// +".material";
		filesystem::create_directory(path.parent_path());

		FileUtil fileUtil;
		fileUtil.Open(path.string(), EFileMode::Write);

		fileUtil.Write<unsigned int>(materialData.size());

		for (const Material& material : materialData)
		{
			fileUtil.Write<DirectX::SimpleMath::Color>(material.MaterialDesc.BaseColor);
			fileUtil.Write<float>(material.MaterialDesc.Metalness);
			fileUtil.Write<float>(material.MaterialDesc.Roughness);

			fileUtil.Write<string>(material.Name);
			fileUtil.Write<string>(material.BaseColorFileName);
			fileUtil.Write<string>(material.MetalnessFileName);
			fileUtil.Write<string>(material.RoughnessFileName);
			fileUtil.Write<string>(material.NormalFileName);
			fileUtil.Write<string>(material.EmissiveFileName);
			fileUtil.Write<string>(material.OpacityFileName);
		}
	}
	void ModelLoader::WriteAnimationByData(const std::vector<AnimationClip>& animationData, const std::string& savePath)
	{
		using namespace std;

		filesystem::path path = savePath;//  +".animation";
		filesystem::create_directory(path.parent_path());

		FileUtil fileUtil;
		fileUtil.Open(path.string(), EFileMode::Write);

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
}