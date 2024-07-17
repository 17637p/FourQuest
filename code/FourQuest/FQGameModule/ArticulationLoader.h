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
		/// Articulation Editor���� ���� ������ �����մϴ�.
		/// </summary>
		void Save(const std::shared_ptr<ArticulationData> data, const Path& path);

		/// <summary>
		/// Articulation Data�� �ε��մϴ�.
		/// </summary>
		std::shared_ptr<ArticulationData> LoadArticulationData(const Path& path, unsigned int& number);

		/// <summary>
		/// Articulation Data�� �ε��մϴ�.
		/// </summary>
		std::shared_ptr<ArticulationData> LoadArticulationData(const Path& path);

	private:
		/// <summary>
		/// ��ũ �����͸� ����Լ��� ���ؼ� �ε��մϴ�.
		/// </summary>
		std::shared_ptr<LinkData> LoadLinkData(const json& linkJson, unsigned int& number);
	};
}

