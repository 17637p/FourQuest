#include "FQPath.h"

#include <cassert>

std::filesystem::path fq::path::GetCurrentPath()
{
	return std::filesystem::current_path();
}

std::filesystem::path fq::path::GetResourcePath()
{
	std::filesystem::path resPath = std::filesystem::current_path();
	resPath += L"\\resource";

	return resPath;
}

std::filesystem::path fq::path::GetInternalPath()
{
	std::filesystem::path resPath = std::filesystem::current_path();
	resPath += L"\\resource\\internal";

	return resPath;
}

std::filesystem::path fq::path::CreateFilePath(const std::filesystem::path& directoryPath
	, const std::filesystem::path& fileName
	, const std::filesystem::path& extension)
{
	std::wstring directory = directoryPath;
	if (directory.back() != L'\\')
	{
		directory.push_back(L'\\');
	}

	std::filesystem::path output = directory;
	output += fileName;
	std::wstring name = extension.wstring();

	// 확장자에 .을 가지지 않으면 붙여준다.
	if (name[0] != L'.')
	{
		name = L'.' + name;
	}

	output += name;

	return output;
}

std::filesystem::path fq::path::GetScenePath()
{
	std::filesystem::path scenePath = std::filesystem::current_path();
	scenePath += L"\\resource\\internal\\scene";

	return scenePath;
}

std::vector<std::filesystem::path> fq::path::GetFileList(const std::filesystem::path& directory)
{
	assert(std::filesystem::is_directory(directory));
	std::vector<std::filesystem::path> directoryList;

	for (const auto& entry : std::filesystem::directory_iterator(directory))
	{
		directoryList.push_back(entry.path());
	}

	return directoryList;
}

std::vector<std::filesystem::path> fq::path::GetFileListRecursive(const std::filesystem::path& directory)
{
	assert(std::filesystem::is_directory(directory));
	std::vector<std::filesystem::path> fileList;

	for (const auto& entry : std::filesystem::recursive_directory_iterator(directory))
	{
		fileList.push_back(entry.path());
	}

	return fileList;
}

std::filesystem::path fq::path::GetRelativePath(const std::filesystem::path& filePath)
{
	if (filePath.empty())
	{
		return {};
	}

	std::filesystem::path currentPath = GetCurrentPath();

	std::filesystem::path  relativePath = std::filesystem::relative(filePath, currentPath);

	return relativePath;
}

std::filesystem::path fq::path::GetAbsolutePath(const std::filesystem::path& relativePath)
{
	if (relativePath.empty())
	{
		return {};
	}

	return GetCurrentPath() / relativePath;
}

std::filesystem::path fq::path::GetGarbagePath()
{
	std::filesystem::path scenePath = std::filesystem::current_path();
	scenePath += L"\\resource\\internal\\garbage";

	return scenePath;
}

bool fq::path::ClearDirectory(const std::filesystem::path& directory)
{
	namespace fs = std::filesystem;

	if (!fs::is_directory(directory))
	{
		return false;
	}


	for (const auto& entry : fs::directory_iterator(directory)) {
		if (fs::is_regular_file(entry.path())) {
			fs::remove_all(entry.path());
		}
	}
	return true;
}
