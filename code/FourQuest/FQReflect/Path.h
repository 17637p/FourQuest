#pragma once

#include <filesystem>
#include <vector>

namespace fq::path
{
	/// <summary>
	/// ������Ʈ�� ���� ���� ��θ� ��ȯ�մϴ�
	/// </summary>
	/// <returns>���� ���� ���</returns>
	std::filesystem::path GetCurrentPath();

	/// <summary>
	/// �������� ���ҽ� ���� ��θ� ��ȯ�մϴ�
	/// </summary>
	/// <returns>���ҽ� ���� ���</returns>
	std::filesystem::path GetResourcePath();

	/// <summary>
	/// ������Ʈ�� �� ���� ��θ� ��ȯ�մϴ�
	/// </summary>
	/// <returns>�� ���� ���</returns>
	std::filesystem::path GetScenePath();

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
	/// path ��� ���ο� �ִ� ���丮�� vector�� ��Ƽ� ��ȯ�մϴ�
	/// </summary>
	/// <param name="path">Ž���� ���</param>
	/// <returns>path ��� ������ ���丮</returns>
	std::vector<std::filesystem::path> GetDirectoryList(const std::filesystem::path& path);


}