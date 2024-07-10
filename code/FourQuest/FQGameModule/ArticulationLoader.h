#pragma once

#include <filesystem>
#include <memory>

#include "../FQReflect/FQReflect.h"

namespace fq::game_module
{
	class ArticulationData;
	class LinkData;

	using json = nlohmann::json;
	using ordered_json = nlohmann::ordered_json;
	namespace fs = std::filesystem;

	class ArticulationLoader
	{
		using Path = std::filesystem::path;
	public:
		/// <summary>
		/// Articulation Editor에서 만든 정보를 저장합니다.
		/// </summary>
		void Save(const std::shared_ptr<ArticulationData> data, const Path& path);

		/// <summary>
		/// 
		/// </summary>
		std::shared_ptr<LinkData> LoadLinkData(const json& linkJson);

		/// <summary>
		/// Articulation Data를 로드합니다.
		/// </summary>
		std::shared_ptr<ArticulationData> LoadArticulationData(const Path& path);
	};
}

