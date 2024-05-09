#pragma once

#include <string>

namespace fq::game_module
{

	/// <summary>
	/// 프리팹 인스턴스 
	/// </summary>
	class PrefabObject
	{
		using Path = std::wstring;
	public:
		/// <summary>
		/// 프리팹의 경로를 반환합니다 
		/// </summary>
		/// <returns></returns>
		fq::game_module::PrefabObject::Path GetPrefabPath() const { return mPrefabPath; }

		/// <summary>
		/// 프리팹의 경로를 설정합니다
		/// </summary>
		void SetPrefabPath(fq::game_module::PrefabObject::Path path) { mPrefabPath = path; }

	private:
		Path mPrefabPath;
	};
}

