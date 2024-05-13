#pragma once

#include <vector>
#include <string>
#include <directxtk/SimpleMath.h>
#include <DirectXCollision.h>

class aiMesh;

namespace fq::common
{
	struct Node
	{
		enum { INVALID_INDEX = -1 };

		std::string Name;
		unsigned int Index = INVALID_INDEX;
		unsigned int ParentIndex = INVALID_INDEX;
		DirectX::SimpleMath::Matrix ToParentMatrix;
	};

	struct Mesh
	{
		struct Vertex
		{
			DirectX::SimpleMath::Vector3 Pos;
			DirectX::SimpleMath::Vector3 Normal;
			DirectX::SimpleMath::Vector3 Tangent;
			DirectX::SimpleMath::Vector2 Tex;
		};

		struct BoneVertex
		{
			enum { MAX_BONE_COUNT = 4 };
			enum { INVALID_INDEX = -1 };

			int BoneIndices[MAX_BONE_COUNT] = { INVALID_INDEX, INVALID_INDEX, INVALID_INDEX, INVALID_INDEX };
			float BoneWeights[MAX_BONE_COUNT] = { 0.f, 0.f, 0.f, 0.f };
		};

		struct Bone
		{
			std::string Name;
			unsigned int NodeIndex;

			friend bool operator<(const Bone& lhs, const Bone& rhs)
			{
				if (lhs.NodeIndex == rhs.NodeIndex)
				{
					return lhs.Name < rhs.Name;
				}

				return lhs.NodeIndex < rhs.NodeIndex;
			}
		};

		struct Subset
		{
			unsigned int VertexStart = 0u;
			unsigned int VertexCount = 0u;
			unsigned int IndexStart = 0u;
			unsigned int IndexCount = 0u;
			std::string MaterialName;

			aiMesh* AiMeshPtr = nullptr;
		};

		std::string Name;
		std::vector<Vertex> Vertices;
		std::vector<BoneVertex> BoneVertices;
		std::vector<unsigned int> Indices;

		std::string NodeName;

		std::vector<Bone> Bones;
		std::vector<Subset> Subsets;

		DirectX::BoundingBox RenderBoundingBox;
		DirectX::BoundingSphere GetRenderBoundingSphere;
	};

	struct Material
	{
		struct Desc
		{
			DirectX::SimpleMath::Color BaseColor = { 0.f, 0.f, 0.f, 1.f };
			float Metalness = 0.f;
			float Roughness = 0.f;
		} MaterialDesc;

		std::string Name;

		std::wstring BaseColorFileName;
		std::wstring MetalnessFileName;
		std::wstring RoughnessFileName;
		std::wstring NormalFileName;
		std::wstring EmissiveFileName;
		std::wstring OpacityFileName;
	};

	struct TerrainMaterial
	{
		struct Desc
		{
			DirectX::SimpleMath::Color BaseColor = { 0.f, 0.f, 0.f, 1.f };
			float Metalness = 0.f;
			float Roughness = 0.f;
		} MaterialDesc;

		std::string Name;

		unsigned short NumOfTexture; // 일단 최대 4

		std::vector<std::wstring> BaseColorFileNames;
		std::vector<std::wstring> MetalnessFileNames;
		std::vector<std::wstring> RoughnessFileNames;
		std::vector<std::wstring> NormalFileNames;

		std::wstring AlPhaFileName; // R에는 BaseColor1, G에는 2, B에는 3, A에는 4

		// 유니티에서 메탈릭 맵이랑 러프니스 맵을 왜 안쓰는지 벌써 알 것만 같다... 해보고 그래픽 퀄리티 차이가 없다면 하나로 통일해야겠다...
		//std::wstring Metalness1FileName;
		//std::wstring Metalness2FileName;
		//std::wstring Metalness3FileName;
		//std::wstring Metalness4FileName;
		//
		//std::wstring Roughness1FileName;
		//std::wstring Roughness2FileName;
		//std::wstring Roughness3FileName;
		//std::wstring Roughness4FileName;
		//
		//std::wstring Normal1FileName;
		//std::wstring Normal2FileName;
		//std::wstring Normal3FileName;
		//std::wstring Normal4FileName;
	};

	struct Keyframe
	{
		float TimePos = 0.f;
		DirectX::SimpleMath::Vector3 Translation = { 0.f, 0.f, 0.f };
		DirectX::SimpleMath::Vector3 Scale = { 1.f, 1.f, 1.f };
		DirectX::SimpleMath::Quaternion Rotation;
	};

	struct NodeClip
	{
		std::string NodeName;
		std::vector<Keyframe> Keyframes;
	};

	struct AnimationClip
	{
		std::string Name;
		unsigned int FrameCount;
		float FramePerSecond;
		float Duration;
		std::vector<NodeClip> NodeClips;
	};

	struct Model
	{
		std::vector<std::pair<Node, Mesh>> Meshes;
		std::vector<AnimationClip> Animations;
		std::vector<Material> Materials;
		DirectX::BoundingBox RenderBoundingBox;
		DirectX::BoundingSphere GetRenderBoundingSphere;
	};
}