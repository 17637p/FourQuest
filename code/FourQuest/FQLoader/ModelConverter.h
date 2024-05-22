#pragma once

#include <memory>

#include "../FQCommon/FQCommonLoader.h"

struct aiScene;
struct aiNode;
namespace Assimp { class Importer; }

namespace fq::loader
{
	class ModelConverter
	{
	public:
		ModelConverter();
		~ModelConverter();

		bool ReadFBXFile(const std::string& fileName);
		fq::common::Model Convert();

	private:
		std::vector<std::pair<fq::common::Node, fq::common::Mesh>> convertModel();
		std::vector<fq::common::Material> convertMaterial();
		std::vector<fq::common::AnimationClip> convertAnimation();

		void parseNode(aiNode* node, std::vector<std::pair<fq::common::Node, fq::common::Mesh>>* outMeshes);
		void parseNodeRecursive(aiNode* node, std::vector<std::pair<fq::common::Node, fq::common::Mesh>>* outMeshes, unsigned int parentIndex);
		fq::common::Mesh parseMesh(aiNode* node);
		void parseBone(std::vector<std::pair<fq::common::Node, fq::common::Mesh>>* inoutMeshes);

	private:
		std::unique_ptr<Assimp::Importer> mImpoter;
		const aiScene* mAiScene;
	};
}