#pragma once

#include "ModelIOData.h"

namespace fq::loader
{
	class ModelLoader
	{
	public:
		static Model ReadModel(const std::string& filePath);
		static std::vector<std::pair<Node, Mesh>> ReadMesh(const std::string& fileName);
		static std::vector<Material> ReadMaterial(const std::string& fileName);
		static std::vector<AnimationClip> ReadAnimation(const std::string& fileName);

		static void WriteModelByData(const Model& modelData, const std::string& savePath);
		static void WriteMeshByData(const std::vector<std::pair<Node, Mesh>>& modelData, const std::string& fileName);
		static void WriteMaterialByData(const std::vector< Material>& materialData, const std::string& fileName);
		static void WriteAnimationByData(const std::vector< AnimationClip>& animationDataByData, const std::string& fileName);
	};
}