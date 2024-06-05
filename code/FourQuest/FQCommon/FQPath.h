#pragma once

#include <filesystem>
#include <vector>

namespace fq
{
	/// <summary>
	/// 폴더(directory) 
	/// 파일(file) 
	/// </summary>
	namespace path
	{
		/// <summary>
		/// 프로젝트의 시작 파일 경로를 반환합니다
		/// </summary>
		/// <returns>시작 파일 경로</returns>
		std::filesystem::path GetCurrentPath();

		/// <summary>
		/// 리소스 폴더 경로를 반환합니다
		/// </summary>
		/// <returns>리소스 폴더 경로</returns>
		std::filesystem::path GetResourcePath();

		/// <summary>
		/// 리소스 폴더 내부의 internal 폴더 경로를 반환합니다
		/// </summary>
		/// <returns>internal 폴더 경로</returns>
		std::filesystem::path GetInternalPath();

		/// <summary>
		/// 리소스 폴더 내부의 씬 폴더 경로를 반환합니다
		/// </summary>
		/// <returns>씬 폴더 경로</returns>
		std::filesystem::path GetScenePath();

		/// <summary>
		/// 리소스 폴더 내부의 쓰레기통 폴더 경로를 반환합니다.
		/// </summary>
		/// <returns>쓰레기통 폴더 경로</returns>
		std::filesystem::path GetGarbagePath();	

		/// <summary>
		/// 폴더 내부의 모든 파일을 지웁니다
		/// </summary>
		/// <param name="directory">폴더경로</param>
		/// <returns>모두 지우면 true, 그렇지 않으면 false</returns>
		bool ClearDirectory(const std::filesystem::path& directory);

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
		/// 폴더 경로 내부에 있는 모든 파일을 vector에 담아서 반환합니다
		/// </summary>
		/// <param name="path">탐색할 폴더 경로</param>
		/// <returns>폴더 경로 내부의 모든 파일</returns>
		std::vector<std::filesystem::path> GetFileList(const std::filesystem::path& directory);

		/// <summary>
		/// 폴더 경로 내부에 있는 모든 파일과 재귀적으로 폴더를 탐색해서 vector에 담아서 반환합니다
		/// </summary>
		/// <param name="path">탐색할 폴더 경로</param>
		/// <returns>폴더 경로 내부의 모든 파일</returns>
		std::vector<std::filesystem::path> GetFileListRecursive(const std::filesystem::path& directory);

		/// <summary>
		/// CurrentPath 기준의 상대경로를 반환합니다
		/// </summary>
		/// <param name="filePath">변환할 경로</param>
		/// <returns>상대경로</returns>
		std::filesystem::path GetRelativePath(const std::filesystem::path& filePath);

		/// <summary>
		/// 상대경로를 CurrentPath기준 절대경로로 변환합니다
		/// </summary>
		/// <param name="relativePath">상대경로</param>
		/// <returns>절대경로</returns>
		std::filesystem::path GetAbsolutePath(const std::filesystem::path& relativePath);

	}
}
