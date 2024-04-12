#pragma once

#include <filesystem>
#include <vector>

namespace fq::path
{
	/// <summary>
	/// 프로젝트의 시작 파일 경로를 반환합니다
	/// </summary>
	/// <returns>시작 파일 경로</returns>
	std::filesystem::path GetCurrentPath();

	/// <summary>
	/// 프로젝의 리소스 파일 경로를 반환합니다
	/// </summary>
	/// <returns>리소스 파일 경로</returns>
	std::filesystem::path GetResourcePath();

	/// <summary>
	/// 프로젝트의 씬 파일 경로를 반환합니다
	/// </summary>
	/// <returns>씬 파일 경로</returns>
	std::filesystem::path GetScenePath();

	/// <summary>
	/// 파일 경로를 생성합니다
	/// </summary>
	/// <param name="directoryPath">폴더(디텍토리) 경로</param>
	/// <param name="fileName">파일 이름</param>
	/// <param name="extension">확장자명</param>
	/// <returns>파일 경로 </returns>
	std::filesystem::path CreateFilePath(const std::filesystem::path& directoryPath
		, const std::filesystem::path& fileName
		, const std::filesystem::path& extension);

	/// <summary>
	/// path 경로 내부에 있는 디렉토리를 vector에 담아서 반환합니다
	/// </summary>
	/// <param name="path">탐색한 경로</param>
	/// <returns>path 경로 내부의 디렉토리</returns>
	std::vector<std::filesystem::path> GetDirectoryList(const std::filesystem::path& path);


}