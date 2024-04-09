#pragma once

#include <filesystem>

namespace fq::path
{
	/// <summary>
	/// ������Ʈ�� ���� ���� ��θ� �����ɴϴ�
	/// </summary>
	/// <returns>���� ���� ���</returns>
	std::filesystem::path GetCurrentPath();

	/// <summary>
	/// �������� ���ҽ� ���� ��θ� �����ɴϴ�
	/// </summary>
	/// <returns>���ҽ� ���� ���</returns>
	std::filesystem::path GetResourcePath();

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



}