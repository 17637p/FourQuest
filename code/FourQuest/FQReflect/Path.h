#pragma once

#include <filesystem>

namespace fq::path
{
	/// <summary>
	/// 프로젝트의 시작 파일 경로를 가져온다
	/// </summary>
	/// <returns>시작 파일 경로</returns>
	std::filesystem::path GetCurrentPath();

	/// <summary>
	/// 프로젝의 리소스 파일 경로를 가져온다.
	/// </summary>
	/// <returns>리소스 파일 경로</returns>
	std::filesystem::path GetResourcePath();

	/// <summary>
	/// 파일 경로를 생성한다.
	/// </summary>
	/// <param name="directoryPath">폴더(디텍토리) 경로</param>
	/// <param name="fileName">파일 이름</param>
	/// <param name="extension">확장자명</param>
	/// <returns>파일 경로 </returns>
	std::filesystem::path CreateFilePath(const std::filesystem::path& directoryPath
		, const std::filesystem::path& fileName
		, const std::filesystem::path& extension);
}