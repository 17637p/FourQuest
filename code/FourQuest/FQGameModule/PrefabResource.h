#pragma once

#include <string>

namespace fq::game_module
{
	/// <summary>
	/// ������ �ν��Ͻ� 
	/// </summary>
	class PrefabResource
	{
		using Path = std::string;
	public:
		PrefabResource() = default;
		PrefabResource(const Path& string);

		/// <summary>
		/// �������� ��θ� ��ȯ�մϴ� 
		/// </summary>
		/// <returns></returns>
		fq::game_module::PrefabResource::Path GetPrefabPath() const { return mPrefabPath; }

		/// <summary>
		/// �������� ��θ� �����մϴ�
		/// </summary>
		void SetPrefabPath(fq::game_module::PrefabResource::Path path);

	private:
		Path mPrefabPath;
	};
}

