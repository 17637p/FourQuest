#pragma once

#include <bitset>
#include <filesystem>

#include "GameModuleEnum.h"

namespace fq::physics
{
	class CollisionMatrix
	{
		using Tag = fq::game_module::ETag;
		using Path = std::filesystem::path;

	public:
		/// <summary>
		/// 충돌 매트릭스를 저장합니다 
		/// </summary>
		void Save(const Path& directory);

		/// <summary>
		/// 충돌 매트릭스를 로드합니다 
		/// </summary>
		/// <param name="path"></param>
		void Load(const Path& path);

		std::bitset<32> data[static_cast<int>(Tag::End)];
	};

}