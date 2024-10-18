#pragma once

#include <filesystem>
#include <vector>
#include <memory>
#include <directxtk\SimpleMath.h>

#include "../FQReflect/FQReflect.h"
#include "../FQCommon/FQCommonPhysics.h"

namespace fq::game_module
{
	using json = nlohmann::json;
	using ordered_json = nlohmann::ordered_json;
	namespace fs = std::filesystem;


	class ClothDataLoader
	{
		using Path = std::filesystem::path;
	public:
		/// <summary>
		/// Cloth Editor에서 만든 정보를 저장합니다.
		/// </summary>
		void Save(const std::shared_ptr<fq::physics::Cloth::ClothData> data, const Path& path);

		/// <summary>
		/// Cloth Data를 로드합니다.
		/// </summary>
		std::shared_ptr<fq::physics::Cloth::ClothData> LoadArticulationData(const Path& path, fq::physics::Cloth::ClothData& clothData);

	private:
		json Vector3ToJson(const DirectX::SimpleMath::Vector3& vec);
		DirectX::SimpleMath::Vector3 JsonToVector3(const json& vecJson);
	};
}

