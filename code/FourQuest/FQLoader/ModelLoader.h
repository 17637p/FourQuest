#pragma once

#include "../FQCommon/FQCommonLoader.h"

namespace fq::loader
{
	class ModelLoader
	{
	public:
		static fq::common::Model ReadModel(const std::string& filePath);
		static std::vector<std::pair<fq::common::Node, fq::common::Mesh>> ReadMesh(const std::string& fileName);
		static std::vector<fq::common::Material> ReadMaterial(const std::string& fileName);
		static std::vector<fq::common::AnimationClip> ReadAnimation(const std::string& fileName);

		static void WriteModelByData(const fq::common::Model& modelData, const std::string& savePath);
		static void WriteMeshByData(const std::vector<std::pair<fq::common::Node, fq::common::Mesh>>& modelData, const std::string& fileName);
		static void WriteMaterialByData(const std::vector<fq::common::Material>& materialData, const std::string& fileName);
		static void WriteAnimationByData(const std::vector<fq::common::AnimationClip>& animationDataByData, const std::string& fileName);
	};
}