#pragma once

#include <string>

namespace fq::game_module
{

	/// <summary>
	/// ������ �ν��Ͻ� 
	/// </summary>
	class PrefabObject
	{
		using Path = std::wstring;
	public:
		/// <summary>
		/// �������� ��θ� ��ȯ�մϴ� 
		/// </summary>
		/// <returns></returns>
		fq::game_module::PrefabObject::Path GetPrefabPath() const { return mPrefabPath; }

		/// <summary>
		/// �������� ��θ� �����մϴ�
		/// </summary>
		void SetPrefabPath(fq::game_module::PrefabObject::Path path) { mPrefabPath = path; }

	private:
		Path mPrefabPath;
	};
}

