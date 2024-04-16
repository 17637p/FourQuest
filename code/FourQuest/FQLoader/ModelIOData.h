#pragma once

namespace fq::loader
{
	struct Node
	{
		enum { INVALID_INDEX = -1 };

		std::string Name;
		unsigned int Index = INVALID_INDEX;
		unsigned int Parentindex = INVALID_INDEX;
		DirectX::SimpleMath::Matrix ToParentMatrix;
	};

	struct Mesh
	{
		struct Vertex
		{
			enum { MAX_BONE_COUNT = 4 };
			enum { INVALID_INDEX = -1 };

			DirectX::SimpleMath::Vector3 Pos;
			DirectX::SimpleMath::Vector3 Normal;
			DirectX::SimpleMath::Vector3 Tangent;
			DirectX::SimpleMath::Vector2 Tex;

			int BoneIndices[MAX_BONE_COUNT] = { INVALID_INDEX, INVALID_INDEX, INVALID_INDEX, INVALID_INDEX };
			float BoneWeights[MAX_BONE_COUNT] = { 0.f, 0.f, 0.f, 0.f };
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
		std::vector<unsigned int> Indices;

		std::string NodeName;
		bool bHasBone{ false };
		bool bHasVertex{ false };

		std::vector<std::string> BoneNames;
		std::vector<Subset> Subsets;
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

		std::string BaseColorFileName;
		std::string MetalnessFileName;
		std::string RoughnessFileName;
		std::string NormalFileName;
		std::string EmissiveFileName;
		std::string OpacityFileName;
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
		std::vector<Material> Materials;
		std::vector<AnimationClip> Animations;
	};
}