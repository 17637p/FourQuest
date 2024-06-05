#pragma once

#include <filesystem>
#include <vector>

namespace fq
{
	/// <summary>
	/// ����(directory) 
	/// ����(file) 
	/// </summary>
	namespace path
	{
		/// <summary>
		/// ������Ʈ�� ���� ���� ��θ� ��ȯ�մϴ�
		/// </summary>
		/// <returns>���� ���� ���</returns>
		std::filesystem::path GetCurrentPath();

		/// <summary>
		/// ���ҽ� ���� ��θ� ��ȯ�մϴ�
		/// </summary>
		/// <returns>���ҽ� ���� ���</returns>
		std::filesystem::path GetResourcePath();

		/// <summary>
		/// ���ҽ� ���� ������ internal ���� ��θ� ��ȯ�մϴ�
		/// </summary>
		/// <returns>internal ���� ���</returns>
		std::filesystem::path GetInternalPath();

		/// <summary>
		/// ���ҽ� ���� ������ �� ���� ��θ� ��ȯ�մϴ�
		/// </summary>
		/// <returns>�� ���� ���</returns>
		std::filesystem::path GetScenePath();

		/// <summary>
		/// ���ҽ� ���� ������ �������� ���� ��θ� ��ȯ�մϴ�.
		/// </summary>
		/// <returns>�������� ���� ���</returns>
		std::filesystem::path GetGarbagePath();	

		/// <summary>
		/// ���� ������ ��� ������ ����ϴ�
		/// </summary>
		/// <param name="directory">�������</param>
		/// <returns>��� ����� true, �׷��� ������ false</returns>
		bool ClearDirectory(const std::filesystem::path& directory);

		/// <summary>
		/// ���� ��θ� �����մϴ�
		/// </summary>
		/// <param name="directoryPath">����(�����丮) ���</param>
		/// <param name="fileName">���� �̸�</param>
		/// <param name="extension">Ȯ���ڸ�</param>
		/// <returns>���� ��� </returns>
		std::filesystem::path CreateFilePath(const std::filesystem::path& directoryPath
			, const std::filesystem::path& fileName
			, const std::filesystem::path& extension);

		/// <summary>
		/// ���� ��� ���ο� �ִ� ��� ������ vector�� ��Ƽ� ��ȯ�մϴ�
		/// </summary>
		/// <param name="path">Ž���� ���� ���</param>
		/// <returns>���� ��� ������ ��� ����</returns>
		std::vector<std::filesystem::path> GetFileList(const std::filesystem::path& directory);

		/// <summary>
		/// ���� ��� ���ο� �ִ� ��� ���ϰ� ��������� ������ Ž���ؼ� vector�� ��Ƽ� ��ȯ�մϴ�
		/// </summary>
		/// <param name="path">Ž���� ���� ���</param>
		/// <returns>���� ��� ������ ��� ����</returns>
		std::vector<std::filesystem::path> GetFileListRecursive(const std::filesystem::path& directory);

		/// <summary>
		/// CurrentPath ������ ����θ� ��ȯ�մϴ�
		/// </summary>
		/// <param name="filePath">��ȯ�� ���</param>
		/// <returns>�����</returns>
		std::filesystem::path GetRelativePath(const std::filesystem::path& filePath);

		/// <summary>
		/// ����θ� CurrentPath���� �����η� ��ȯ�մϴ�
		/// </summary>
		/// <param name="relativePath">�����</param>
		/// <returns>������</returns>
		std::filesystem::path GetAbsolutePath(const std::filesystem::path& relativePath);

	}
}
