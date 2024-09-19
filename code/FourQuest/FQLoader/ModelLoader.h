#pragma once

#include "../FQCommon/FQCommonLoader.h"
#include "../FQReflect/FQReflect.h"
#include "../FQCommon/FQCommonGraphics.h"

namespace fq::loader
{
	class FileUtil;


	class LoaderHelpaer
	{
	public:
		static std::filesystem::path CorrectPathCharacters(const std::filesystem::path& path);
	};

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

	class UVAnimationLoader
	{
	public:
		static fq::common::UVAnimationClip Read(const std::filesystem::path& animFileName);
		static void Write(const fq::common::UVAnimationClip& uvAnimationClip, const std::string& fileName);

	private:
		fq::reflect::Converter mConverter;
	};

	class AnimationLoader
	{
	public:
		static fq::common::AnimationClip Read(const std::filesystem::path& animFileName);
		static void Write(const fq::common::AnimationClip& animationClip, const std::string& fileName);

	private:
		fq::reflect::Converter mConverter;
	};

	class NodeHierarchyLoader
	{
	public:
		static std::vector<fq::common::Node> Read(const std::filesystem::path& animFileName);
		static void Write(const std::vector<fq::common::Node>& nodeHierarchy, const std::string& fileName);
		static void Write(const fq::common::Model& modeData, const std::string& fileName);
	private:

		fq::reflect::Converter mConverter;
	};

	class MaterialLoader
	{
	public:
		static fq::graphics::MaterialInfo Read(const std::filesystem::path& filePath);
		static void Write(const std::filesystem::path& filePath, const fq::graphics::MaterialInfo& material);

	private:
		fq::reflect::Converter mConverter;
	};
}