#pragma once

#include "../FQCommon/FQCommonLoader.h"

namespace fq::loader
{
	class FileUtil;

	class ModelLoader
	{
	public:
		static fq::common::Model Read(const std::string& fileName);
		static void Write(const fq::common::Model& modelData, const std::string& fileName);

	private:
		static std::vector<std::pair<fq::common::Node, fq::common::Mesh>> ReadMesh(FileUtil& fileUtil);
		static std::vector<fq::common::Material> ReadMaterial(FileUtil& fileUtil);
		static std::vector<fq::common::AnimationClip> ReadAnimation(FileUtil& fileUtil);

		static void WriteMeshByData(const std::vector<std::pair<fq::common::Node, fq::common::Mesh>>& modelData, FileUtil& fileUtil);
		static void WriteMaterialByData(const std::vector<fq::common::Material>& materialData, FileUtil& fileUtil);
		static void WriteAnimationByData(const std::vector<fq::common::AnimationClip>& animationDataByData, FileUtil& fileUtil);
	};
}