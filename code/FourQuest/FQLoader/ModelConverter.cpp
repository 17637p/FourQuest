#include "pch.h"

#include "ModelConverter.h"
#include "ModelLoader.h"
#include "FileUtil.h"
#include "Util.h"

namespace fq::loader
{
	ModelConverter::ModelConverter()
		: mAiScene(nullptr)
	{
		mImpoter.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, 0);
	}

	ModelConverter::~ModelConverter()
	{
		mImpoter.FreeScene();
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
			aiProcess_ConvertToLeftHanded;

		mAiScene = mImpoter.ReadFile(path.string(), importFlags);

		return mAiScene != nullptr;
	}

	void ModelConverter::WriteModel(const std::string& savePath)
	{
		WriteMesh(savePath + ".mesh");
		WriteMaterial(savePath + ".material");
		WriteAnimation(savePath + ".animation");
	}

	void ModelConverter::WriteMesh(const std::string& saveName)
	{
		assert(mAiScene != nullptr);
		using namespace std;

		vector<pair<Node, Mesh>> modelData = convertModel();
		ModelLoader::WriteMeshByData(modelData, saveName);
	}

	void ModelConverter::WriteMaterial(const std::string& saveName)
	{
		assert(mAiScene != nullptr);
		using namespace std;

		vector<Material> materials = convertMaterial();
		ModelLoader::WriteMaterialByData(materials, saveName);
	}

	void ModelConverter::WriteAnimation(const std::string& saveName)
	{
		assert(mAiScene != nullptr);
		using namespace std;

		vector<AnimationClip> animationClips = convertAnimation();
		ModelLoader::WriteAnimationByData(animationClips, saveName);
	}

	std::vector<std::pair< Node, Mesh>> ModelConverter::convertModel()
	{
		using namespace std;

		vector<pair<Node, Mesh>> meshData;

		parseNode(mAiScene->mRootNode, &meshData);
		parseBone(&meshData);

		return meshData;
	}

	std::vector< Material> ModelConverter::convertMaterial()
	{
		using namespace std;

		vector<Material> materials;

		for (size_t i = 0; i < mAiScene->mNumMaterials; ++i)
		{
			aiMaterial* aiMaterialPtr = mAiScene->mMaterials[i];
			Material material;

			material.Name = aiMaterialPtr->GetName().C_Str();

			// to do : 기본으로 설정된 값 데이터 가져오기
			if (aiMaterialPtr->Get(AI_MATKEY_BASE_COLOR, material.MaterialDesc.BaseColor) != AI_SUCCESS);
			{
				aiMaterialPtr->Get(AI_MATKEY_COLOR_DIFFUSE, material.MaterialDesc.BaseColor);
			}
			aiMaterialPtr->Get(AI_MATKEY_METALLIC_FACTOR, material.MaterialDesc.Metalness);
			aiMaterialPtr->Get(AI_MATKEY_ROUGHNESS_FACTOR, material.MaterialDesc.Roughness);

			aiString filePath;
			if (aiMaterialPtr->GetTexture(aiTextureType_BASE_COLOR, 0, &filePath) == AI_SUCCESS)
			{
				std::filesystem::path fileFullPath = Util::ToWide(filePath.C_Str());
				material.BaseColorFileName = fileFullPath.filename().string();
			}
			else if (aiMaterialPtr->GetTexture(aiTextureType_DIFFUSE, 0, &filePath) == AI_SUCCESS)
			{
				std::filesystem::path fileFullPath = Util::ToWide(filePath.C_Str());
				material.BaseColorFileName = fileFullPath.filename().string();
			}
			if (aiMaterialPtr->GetTexture(aiTextureType_METALNESS, 0, &filePath) == AI_SUCCESS)
			{
				std::filesystem::path fileFullPath = Util::ToWide(filePath.C_Str());
				material.MetalnessFileName = fileFullPath.filename().string();
			}
			if (aiMaterialPtr->GetTexture(aiTextureType_SHININESS, 0, &filePath) == AI_SUCCESS)
			{
				std::filesystem::path fileFullPath = Util::ToWide(filePath.C_Str());
				material.RoughnessFileName = fileFullPath.filename().string();
			}
			if (aiMaterialPtr->GetTexture(aiTextureType_NORMALS, 0, &filePath) == AI_SUCCESS)
			{
				std::filesystem::path fileFullPath = Util::ToWide(filePath.C_Str());
				material.NormalFileName = fileFullPath.filename().string();
			}
			if (aiMaterialPtr->GetTexture(aiTextureType_EMISSIVE, 0, &filePath) == AI_SUCCESS)
			{
				std::filesystem::path fileFullPath = Util::ToWide(filePath.C_Str());
				material.EmissiveFileName = fileFullPath.filename().string();
			}
			if (aiMaterialPtr->GetTexture(aiTextureType_OPACITY, 0, &filePath) == AI_SUCCESS)
			{
				std::filesystem::path fileFullPath = Util::ToWide(filePath.C_Str());
				material.OpacityFileName = fileFullPath.filename().string();
			}

			materials.push_back(material);
		}

		return materials;
	}

	std::vector< AnimationClip> ModelConverter::convertAnimation()
	{
		using namespace std;

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

	void ModelConverter::parseNode(aiNode* aiNode, std::vector<std::pair< Node, Mesh>>* outMeshes)
	{
		using namespace std;


		parseNodeRecursive(aiNode, outMeshes, Node::INVALID_INDEX);
	}

	void ModelConverter::parseNodeRecursive(aiNode* aiNode, std::vector<std::pair< Node, Mesh>>* outMeshes, unsigned int parentIndex)
	{
		using namespace std;

		Node node;

		node.Name = aiNode->mName.C_Str();
		node.Index = outMeshes->size();
		node.Parentindex = parentIndex;
		node.ToParentMatrix = DirectX::SimpleMath::Matrix(aiNode->mTransformation[0]).Transpose();

		Mesh mesh = parseMesh(aiNode);

		outMeshes->push_back({ node, mesh });

		for (size_t i = 0; i < aiNode->mNumChildren; ++i)
		{
			parseNodeRecursive(aiNode->mChildren[i], outMeshes, node.Index);
		}
	}

	Mesh ModelConverter::parseMesh(aiNode* aiNode)
	{
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
		resultMesh.Indices.reserve(indexSize);
		resultMesh.bHasBone = bHasBone;
		resultMesh.bHasVertex = vertexSize != 0;

		for (aiMesh* aiMeshPtr : aiSubMeshes)
		{
			std::vector<Mesh::Vertex> vertices;
			std::vector<unsigned int> indices;

			vertices.reserve(aiMeshPtr->mNumVertices);
			indices.reserve(aiMeshPtr->mNumFaces * 3);

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

				vertices.push_back(vertex);
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
		}

		return resultMesh;
	}

	void ModelConverter::parseBone(std::vector<std::pair<Node, Mesh>>* inoutMeshes)
	{
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


		for (auto& nodeMeshPair : *inoutMeshes)
		{
			if (!nodeMeshPair.second.bHasBone)
			{
				continue;
			}

			std::map<std::string, unsigned int> boneIndices;

			for (auto& subset : nodeMeshPair.second.Subsets)
			{
				if (!subset.AiMeshPtr->HasBones())
				{
					continue;
				}

				for (unsigned int i = 0; i < subset.AiMeshPtr->mNumBones; ++i)
				{
					aiBone* bonePtr = subset.AiMeshPtr->mBones[i];
					boneIndices.insert({ bonePtr->mName.C_Str(), boneIndices.size() });
				}
			}

			std::vector<std::string> boneNames;
			boneNames.resize(boneIndices.size());

			for (auto& boenIndexPair : boneIndices)
			{
				boneNames[boenIndexPair.second] = boenIndexPair.first;
			}

			nodeMeshPair.second.BoneNames = std::move(boneNames);

			for (auto& subset : nodeMeshPair.second.Subsets)
			{
				for (unsigned int i = 0; i < subset.AiMeshPtr->mNumBones; ++i)
				{
					aiBone* bonePtr = subset.AiMeshPtr->mBones[i];

					unsigned int boneIndex = findNodeIndex((std::string)bonePtr->mName.C_Str()); // Node 기준
					// unsigned int boneIndex = boneIndices.find((std::string)bonePtr->mName.C_Str())->second; // Mesh Bone 기준

					for (unsigned int j = 0; j < bonePtr->mNumWeights; ++j)
					{
						aiVertexWeight curVertexWeight = bonePtr->mWeights[j];

						unsigned int vertexID = curVertexWeight.mVertexId + subset.VertexStart;
						float vertexWeight = curVertexWeight.mWeight;

						int* indexPtr = nodeMeshPair.second.Vertices[vertexID].BoneIndices;
						float* weightPtr = nodeMeshPair.second.Vertices[vertexID].BoneWeights;

						for (unsigned int k = 0; k < 4; ++k)
						{
							if (*indexPtr == Mesh::Vertex::INVALID_INDEX)
							{
								*indexPtr = boneIndex;
								*weightPtr = vertexWeight;

								break;
							}

							++indexPtr;
							++weightPtr;
						}
					}
				}

				subset.AiMeshPtr = nullptr;
			}
		}
	}
}