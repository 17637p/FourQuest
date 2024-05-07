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
			fileUtil.Read<DirectX::BoundingSphere>(&mesh.GetRenderBoundingSphere);

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

			fileUtil.Read<DirectX::SimpleMath::Color>(&material.MaterialDesc.BaseColor);
			fileUtil.Read<float>(&material.MaterialDesc.Metalness);
			fileUtil.Read<float>(&material.MaterialDesc.Roughness);

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
			fileUtil.Write<DirectX::BoundingSphere>(mesh.GetRenderBoundingSphere);
		}
	}
	void ModelLoader::WriteMaterialByData(const std::vector<fq::common::Material>& materialData, FileUtil& fileUtil)
	{
		using namespace std;
		using namespace fq::common;

		fileUtil.Write<unsigned int>(materialData.size());

		for (const Material& material : materialData)
		{
			fileUtil.Write<DirectX::SimpleMath::Color>(material.MaterialDesc.BaseColor);
			fileUtil.Write<float>(material.MaterialDesc.Metalness);
			fileUtil.Write<float>(material.MaterialDesc.Roughness);

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
}