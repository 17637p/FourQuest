#include "Path.h"

std::filesystem::path fq::path::GetCurrentPath()
{
	return std::filesystem::current_path();
}

std::filesystem::path fq::path::GetResourcePath()
{
	std::filesystem::path resPath = std::filesystem::current_path();
	resPath +=  L"\\resource\\";

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
