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
		/// �浹 ��Ʈ������ �����մϴ� 
		/// </summary>
		void Save(const Path& directory);

		/// <summary>
		/// �浹 ��Ʈ������ �ε��մϴ� 
		/// </summary>
		/// <param name="path"></param>
		void Load(const Path& path);

		std::bitset<32> data[static_cast<int>(Tag::End)];
	};

}