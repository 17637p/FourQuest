#pragma once

#include <string>

#include "entt.hpp"

namespace fq::reflect
{
	/// <summary>
	/// meta_data �� prop�� name�� ��ȯ�մϴ�
	/// </summary>
	/// <param name="metaData">��Ÿ������</param>
	/// <returns>name ������ ������ �� string�� ��ȯ�մϴ�</returns>
	std::string GetName(const entt::meta_data& metaData);

	/// <summary>
	/// meta_type �� prop�� name�� ��ȯ�մϴ�
	/// </summary>
	/// <param name="metaType">��ŸŸ��</param>
	/// <returns>name ������ ������  �� string�� ��ȯ�մϴ�</returns>
	std::string GetName(const entt::meta_type& metaType);

	/// <summary>
	/// meta_data �� prop�� comment�� ��ȯ�մϴ�
	/// </summary>
	/// <param name="metaData">��Ÿ������</param>
	/// <returns>comment ������ ������ �� string�� ��ȯ�մϴ�</returns>
	std::string GetComment(const entt::meta_data& metaData);
	
	/// <summary>
	/// meta_type�� prop�� label�� ��ȯ�մϴ�.
	/// </summary>
	/// <param name="metaType">��ŸŸ��</param>
	/// <returns>label�� ������ ������ �� string�� ��ȯ�մϴ�</returns>
	std::string GetLabel(const entt::meta_type& metaType);
}

