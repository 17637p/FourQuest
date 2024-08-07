#include "pch.h"

#include "ModelConverter.h"
#include "Util.h"

namespace fq::loader
{
	ModelConverter::ModelConverter()
		: mAiScene(nullptr)
	{
		mImpoter = std::make_unique<Assimp::Importer>();
		mImpoter->SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, 0);
	}

	ModelConverter::~ModelConverter()
	{
		mImpoter->FreeScene();
	}

	bool ModelConverter::ReadFBXFile(const std::string& fileName)
	{
		std::filesystem::path path = std::filesystem::path(fileName);

		if (!std::filesystem::exists(path))
		{
			return false;
		}

		unsigned int importFlags =
			aiProcess_Triangulate |
			aiProcess_GenNormals |
			aiProcess_GenUVCoords |
			aiProcess_CalcTangentSpace |
			aiProcess_LimitBoneWeights |
			aiProcess_GenBoundingBoxes |
			aiProcess_ConvertToLeftHanded;

		mAiScene = mImpoter->ReadFile(path.string(), importFlags);

		return mAiScene != nullptr;
	}

	fq::common::Model ModelConverter::Convert()
	{
		assert(mAiScene != nullptr);
		using namespace std;
		using namespace fq::common;

		Model model;
		model.Meshes = convertModel();
		model.Materials = convertMaterial();
		model.Animations = convertAnimation();

		return model;
	}

	std::vector<std::pair<fq::common::Node, fq::common::Mesh>> ModelConverter::convertModel()
	{
		using namespace std;
		using namespace fq::common;

		vector<pair<Node, Mesh>> meshData;

		parseNode(mAiScene->mRootNode, &meshData);

		size_t index = 0;
		set<string> meshNameSet;

		for (auto& nodeMeshPair : meshData)
		{
			std::string& meshName = nodeMeshPair.second.Name;

			auto find = meshNameSet.find(meshName);

			if (find != meshNameSet.end())
			{
				meshName += std::to_string(index++);
			}

			meshNameSet.insert(meshName);
		}

		parseBone(&meshData);

		return meshData;
	}

	std::vector<fq::common::Material> ModelConverter::convertMaterial()
	{
		using namespace std;
		using namespace fq::common;

		vector<Material> materials;

		for (size_t i = 0; i < mAiScene->mNumMaterials; ++i)
		{
			aiMaterial* aiMaterialPtr = mAiScene->mMaterials[i];
			Material material;

			material.Name = aiMaterialPtr->GetName().C_Str();

			// to do : 기본으로 설정된 값 데이터 가져오기
			if (aiMaterialPtr->Get(AI_MATKEY_BASE_COLOR, material.BaseColor) != AI_SUCCESS);
			{
				aiMaterialPtr->Get(AI_MATKEY_COLOR_DIFFUSE, material.BaseColor);
			}
			aiMaterialPtr->Get(AI_MATKEY_METALLIC_FACTOR, material.Metalness);
			aiMaterialPtr->Get(AI_MATKEY_ROUGHNESS_FACTOR, material.Roughness);

			aiString filePath;
			if (aiMaterialPtr->GetTexture(aiTextureType_BASE_COLOR, 0, &filePath) == AI_SUCCESS)
			{
				std::filesystem::path fileFullPath = Util::ToWide(filePath.C_Str());
				material.BaseColorFileName = fileFullPath.filename().wstring();
			}
			else if (aiMaterialPtr->GetTexture(aiTextureType_DIFFUSE, 0, &filePath) == AI_SUCCESS)
			{
				std::filesystem::path fileFullPath = Util::ToWide(filePath.C_Str());
				material.BaseColorFileName = fileFullPath.filename().wstring();
			}
			if (aiMaterialPtr->GetTexture(aiTextureType_METALNESS, 0, &filePath) == AI_SUCCESS)
			{
				std::filesystem::path fileFullPath = Util::ToWide(filePath.C_Str());
				material.MetalnessFileName = fileFullPath.filename().wstring();
			}
			if (aiMaterialPtr->GetTexture(aiTextureType_SHININESS, 0, &filePath) == AI_SUCCESS)
			{
				std::filesystem::path fileFullPath = Util::ToWide(filePath.C_Str());
				material.RoughnessFileName = fileFullPath.filename().wstring();
			}
			if (aiMaterialPtr->GetTexture(aiTextureType_NORMALS, 0, &filePath) == AI_SUCCESS)
			{
				std::filesystem::path fileFullPath = Util::ToWide(filePath.C_Str());
				material.NormalFileName = fileFullPath.filename().wstring();
			}
			if (aiMaterialPtr->GetTexture(aiTextureType_EMISSIVE, 0, &filePath) == AI_SUCCESS)
			{
				std::filesystem::path fileFullPath = Util::ToWide(filePath.C_Str());
				material.EmissiveFileName = fileFullPath.filename().wstring();
			}
			if (aiMaterialPtr->GetTexture(aiTextureType_OPACITY, 0, &filePath) == AI_SUCCESS)
			{
				std::filesystem::path fileFullPath = Util::ToWide(filePath.C_Str());
				material.OpacityFileName = fileFullPath.filename().wstring();
			}

			materials.push_back(material);
		}

		return materials;
	}

	std::vector<fq::common::AnimationClip> ModelConverter::convertAnimation()
	{
		using namespace std;
		using namespace fq::common;

		vector<AnimationClip> animationClips;
		animationClips.reserve(mAiScene->mNumAnimations);

		for (unsigned int i = 0; i < mAiScene->mNumAnimations; ++i)
		{
			aiAnimation* aiAnimationPtr = mAiScene->mAnimations[i];

			AnimationClip animationCilp;
			animationCilp.Name = aiAnimationPtr->mName.C_Str();
			animationCilp.FrameCount = (unsigned int)aiAnimationPtr->mDuration;
			animationCilp.FramePerSecond = 1 / (aiAnimationPtr->mTicksPerSecond);
			animationCilp.Duration = animationCilp.FrameCount * animationCilp.FramePerSecond;

			animationCilp.NodeClips.reserve(aiAnimationPtr->mNumChannels);

			for (unsigned int j = 0; j < aiAnimationPtr->mNumChannels; ++j)
			{
				aiNodeAnim* aiNodeAnimPtr = aiAnimationPtr->mChannels[j];
				NodeClip nodeClip;

				nodeClip.NodeName = aiNodeAnimPtr->mNodeName.C_Str();
				nodeClip.Keyframes.reserve(aiNodeAnimPtr->mNumPositionKeys);

				assert(aiNodeAnimPtr->mNumPositionKeys == aiNodeAnimPtr->mNumRotationKeys);
				assert(aiNodeAnimPtr->mNumPositionKeys == aiNodeAnimPtr->mNumScalingKeys);

				for (unsigned int k = 0; k < aiNodeAnimPtr->mNumPositionKeys; ++k)
				{
					assert(aiNodeAnimPtr->mPositionKeys[k].mTime == aiNodeAnimPtr->mRotationKeys[k].mTime);
					assert(aiNodeAnimPtr->mPositionKeys[k].mTime == aiNodeAnimPtr->mScalingKeys[k].mTime);

					auto translation = aiNodeAnimPtr->mPositionKeys[k].mValue;
					auto rotation = aiNodeAnimPtr->mRotationKeys[k].mValue;
					auto scale = aiNodeAnimPtr->mScalingKeys[k].mValue;

					Keyframe keyframe;
					keyframe.TimePos = aiNodeAnimPtr->mPositionKeys[k].mTime * animationCilp.FramePerSecond;
					keyframe.Translation = { translation.x, translation.y, translation.z };
					keyframe.Rotation = { rotation.x, rotation.y, rotation.z, rotation.w };
					keyframe.Scale = { scale.x, scale.y, scale.z };

					nodeClip.Keyframes.push_back(std::move(keyframe));
				}

				if (nodeClip.Keyframes.size() < animationCilp.FrameCount)
				{
					size_t count = animationCilp.FrameCount - nodeClip.Keyframes.size();

					for (size_t k = 0; k < count; ++k)
					{
						nodeClip.Keyframes.push_back(nodeClip.Keyframes.back());
					}
				}

				animationCilp.NodeClips.push_back(nodeClip);
			}

			animationClips.push_back(animationCilp);
		}

		return animationClips;
	}

	void ModelConverter::parseNode(aiNode* aiNode, std::vector<std::pair<fq::common::Node, fq::common::Mesh>>* outMeshes)
	{
		using namespace std;
		using namespace fq::common;

		parseNodeRecursive(aiNode, outMeshes, Node::INVALID_INDEX);
	}

	void ModelConverter::parseNodeRecursive(aiNode* aiNode, std::vector<std::pair<fq::common::Node, fq::common::Mesh>>* outMeshes, unsigned int parentIndex)
	{
		using namespace std;
		using namespace fq::common;

		Node node;

		node.Name = aiNode->mName.C_Str();
		node.Index = outMeshes->size();
		node.ParentIndex = parentIndex;
		node.ToParentMatrix = DirectX::SimpleMath::Matrix(aiNode->mTransformation[0]).Transpose();

		Mesh mesh = parseMesh(aiNode);

		outMeshes->push_back({ node, mesh });

		for (size_t i = 0; i < aiNode->mNumChildren; ++i)
		{
			parseNodeRecursive(aiNode->mChildren[i], outMeshes, node.Index);
		}
	}

	fq::common::Mesh ModelConverter::parseMesh(aiNode* aiNode)
	{
		using namespace DirectX::SimpleMath;
		using namespace fq::common;

		if (aiNode->mNumMeshes == 0)
		{
			return Mesh();
		}

		std::vector<aiMesh*> aiSubMeshes;
		aiSubMeshes.reserve(aiNode->mNumMeshes);

		aiMesh* firstMesh = mAiScene->mMeshes[aiNode->mMeshes[0]];
		std::string meshName = firstMesh->mName.C_Str();
		size_t vertexSize = 0u;
		size_t indexSize = 0u;
		bool bHasBone = firstMesh->HasBones();

		for (size_t i = 0; i < aiNode->mNumMeshes; ++i)
		{
			size_t meshIndex = aiNode->mMeshes[i];
			aiMesh* aiMesh = mAiScene->mMeshes[meshIndex];
			assert(meshName == std::string(aiMesh->mName.C_Str()));
			assert(bHasBone == aiMesh->HasBones());

			vertexSize += aiMesh->mNumVertices;
			indexSize += aiMesh->mNumFaces * 3;

			aiSubMeshes.push_back(aiMesh);
		}

		Mesh resultMesh;
		resultMesh.Name = meshName;
		resultMesh.NodeName = aiNode->mName.C_Str();
		resultMesh.Vertices.reserve(vertexSize);

		Vector3 min;
		Vector3 max;

		for (aiMesh* aiMeshPtr : aiSubMeshes)
		{
			std::vector<Mesh::Vertex> vertices;
			std::vector<DirectX::SimpleMath::Vector2> uv1s;
			std::vector<unsigned int> indices;

			vertices.reserve(aiMeshPtr->mNumVertices);
			indices.reserve(aiMeshPtr->mNumFaces * 3);

			Vector3 meshMin = { aiMeshPtr->mAABB.mMin.x, aiMeshPtr->mAABB.mMin.y, aiMeshPtr->mAABB.mMin.z };
			Vector3 meshMax = { aiMeshPtr->mAABB.mMax.x,  aiMeshPtr->mAABB.mMax.y,  aiMeshPtr->mAABB.mMax.z };

			min = Vector3::Min(min, meshMin);
			max = Vector3::Max(max, meshMax);

			for (unsigned int i = 0; i < aiMeshPtr->mNumVertices; ++i)
			{
				Mesh::Vertex vertex;

				if (aiMeshPtr->HasPositions())
				{
					vertex.Pos.x = aiMeshPtr->mVertices[i].x;
					vertex.Pos.y = aiMeshPtr->mVertices[i].y;
					vertex.Pos.z = aiMeshPtr->mVertices[i].z;
				}

				if (aiMeshPtr->HasNormals())
				{
					vertex.Normal.x = aiMeshPtr->mNormals[i].x;
					vertex.Normal.y = aiMeshPtr->mNormals[i].y;
					vertex.Normal.z = aiMeshPtr->mNormals[i].z;
				}

				if (aiMeshPtr->HasTangentsAndBitangents())
				{
					vertex.Tangent.x = aiMeshPtr->mTangents[i].x;
					vertex.Tangent.y = aiMeshPtr->mTangents[i].y;
					vertex.Tangent.z = aiMeshPtr->mTangents[i].z;
				}

				if (aiMeshPtr->HasTextureCoords(0))
				{
					vertex.Tex.x = (float)aiMeshPtr->mTextureCoords[0][i].x;
					vertex.Tex.y = (float)aiMeshPtr->mTextureCoords[0][i].y;
				}

				if (aiMeshPtr->HasTextureCoords(1))
				{
					uv1s.push_back({ (float)aiMeshPtr->mTextureCoords[1][i].x, (float)aiMeshPtr->mTextureCoords[1][i].y });
				}

				vertices.push_back(vertex);
			}

			if (aiMeshPtr->HasVertexColors(0))
			{
				Mesh::DynamicData colorData;

 				colorData.Name = "Color0";
				colorData.Size = sizeof(aiColor4D);
				colorData.Count = aiMeshPtr->mNumVertices;

				size_t bufferSize = colorData.Size * colorData.Count;
				colorData.Data.resize(bufferSize);
				memcpy(colorData.Data.data(), &aiMeshPtr->mColors[0][0], bufferSize);
				
				resultMesh.DynamicInfos.insert({ colorData.Name, colorData });
			}

			for (unsigned int i = 0; i < aiMeshPtr->mNumFaces; ++i)
			{
				const auto& face = aiMeshPtr->mFaces[i];
				assert(face.mNumIndices == 3);

				indices.push_back(face.mIndices[0]);
				indices.push_back(face.mIndices[1]);
				indices.push_back(face.mIndices[2]);
			}

			Mesh::Subset subset;
			subset.VertexStart = resultMesh.Vertices.size();
			subset.VertexCount = vertices.size();
			subset.IndexStart = resultMesh.Indices.size();
			subset.IndexCount = indices.size();
			subset.MaterialName = mAiScene->mMaterials[aiMeshPtr->mMaterialIndex]->GetName().C_Str();
			subset.AiMeshPtr = aiMeshPtr;

			resultMesh.Subsets.push_back(subset);
			resultMesh.Vertices.insert(resultMesh.Vertices.end(), vertices.begin(), vertices.end());
			resultMesh.Indices.insert(resultMesh.Indices.end(), indices.begin(), indices.end());
			resultMesh.Tex1.insert(resultMesh.Tex1.end(), uv1s.begin(), uv1s.end());
			
		}

		DirectX::BoundingBox::CreateFromPoints(resultMesh.RenderBoundingBox, min, max);
		DirectX::BoundingSphere::CreateFromBoundingBox(resultMesh.RenderBoundingSphere, resultMesh.RenderBoundingBox);

		return resultMesh;
	}

	void ModelConverter::parseBone(std::vector<std::pair<fq::common::Node, fq::common::Mesh>>* inoutMeshes)
	{
		using namespace fq::common;

		auto findNodeIndex = [inoutMeshes](std::string nodeName)
			{
				for (auto& nodeMeshPair : *inoutMeshes)
				{
					if (nodeMeshPair.first.Name == nodeName)
					{
						return nodeMeshPair.first.Index;
					}
				}

				return (unsigned int)Node::INVALID_INDEX;
			};

		std::set<Mesh::Bone> bones;

		for (auto& nodeMeshPair : *inoutMeshes)
		{
			Mesh& mesh = nodeMeshPair.second;

			for (auto& subset : mesh.Subsets)
			{
				aiMesh* aiMeshPtr = subset.AiMeshPtr;

				if (!aiMeshPtr->HasBones())
				{
					continue;
				}

				std::vector<Mesh::BoneVertex> boneVertices(subset.VertexCount);

				for (unsigned int i = 0; i < aiMeshPtr->mNumBones; ++i)
				{
					aiBone* bonePtr = aiMeshPtr->mBones[i];

					unsigned int nodeIndex = findNodeIndex((std::string)bonePtr->mName.C_Str());

					Mesh::Bone bone;
					bone.Name = bonePtr->mName.C_Str();
					bone.NodeIndex = nodeIndex;
					(*inoutMeshes)[nodeIndex].first.OffsetMatrix = DirectX::SimpleMath::Matrix(bonePtr->mOffsetMatrix[0]).Transpose();
					bones.insert(bone);
					mesh.Bones.push_back(bone);

					for (unsigned int j = 0; j < bonePtr->mNumWeights; ++j)
					{
						aiVertexWeight curVertexWeight = bonePtr->mWeights[j];

						unsigned int vertexID = curVertexWeight.mVertexId; //  +subset.VertexStart;
						float vertexWeight = curVertexWeight.mWeight;

						int* indexPtr = boneVertices[vertexID].BoneIndices;
						float* weightPtr = boneVertices[vertexID].BoneWeights;

						for (unsigned int k = 0; k < 4; ++k)
						{
							if (*indexPtr == Mesh::BoneVertex::INVALID_INDEX)
							{
								*indexPtr = nodeIndex;
								*weightPtr = vertexWeight;

								break;
							}

							++indexPtr;
							++weightPtr;
						}
					}
				}

				for (auto& boneVertex : boneVertices)
				{
					float weightSum = boneVertex.BoneWeights[0] +
						boneVertex.BoneWeights[1] +
						boneVertex.BoneWeights[2] +
						boneVertex.BoneWeights[3];

					if (weightSum < 1.f)
					{
						boneVertex.BoneWeights[0] *= (1.f / weightSum);
						boneVertex.BoneWeights[1] *= (1.f / weightSum);
						boneVertex.BoneWeights[2] *= (1.f / weightSum);
						boneVertex.BoneWeights[3] *= (1.f / weightSum);
					}
				}

				subset.AiMeshPtr = nullptr;
				mesh.BoneVertices.insert(mesh.BoneVertices.end(), boneVertices.begin(), boneVertices.end());

				for (const auto& bone : bones)
				{
					mesh.Bones.push_back(bone);
				}
			}
		}
	}
}