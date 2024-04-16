#pragma once

#include "ModelIOData.h"

class aiScene;

namespace fq::loader
{
	class ModelConverter
	{
	public:
		ModelConverter();
		~ModelConverter();

		bool ReadFBXFile(const std::string& fileName);

		void WriteModel(const std::string& savePath);
		void WriteMesh(const std::string& saveName);
		void WriteMaterial(const std::string& saveName);
		void WriteAnimation(const std::string& saveName);

	private:
		std::vector<std::pair<Node, Mesh>> convertModel();
		std::vector<Material> convertMaterial();
		std::vector<AnimationClip> convertAnimation();

		void parseNode(aiNode* node, std::vector<std::pair<Node, Mesh>>* outMeshes);
		void parseNodeRecursive(aiNode* node, std::vector<std::pair< Node, Mesh>>* outMeshes, unsigned int parentIndex);
		Mesh parseMesh(aiNode* node);
		void parseBone(std::vector<std::pair< Node, Mesh>>* inoutMeshes);

	private:
		Assimp::Importer mImpoter;
		const aiScene* mAiScene;
	};
}