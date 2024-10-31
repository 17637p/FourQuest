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
		void Save(const std::shared_ptr<fq::physics::Cloth::ClothData> data, const std::vector<DirectX::SimpleMath::Vector3>& vertices, const Path& path);

		/// <summary>
		/// Cloth Data를 로드합니다.
		/// </summary>
		bool LoadArticulationData(const Path& path, fq::physics::Cloth::ClothData& clothData);

	private:
		json Vector3ToJson(const DirectX::SimpleMath::Vector3& vec);
		json Vector2ToJson(const DirectX::SimpleMath::Vector2& vec);
		DirectX::SimpleMath::Vector3 JsonToVector3(const json& vecJson);
		DirectX::SimpleMath::Vector2 JsonToVector2(const json& vecJson);
	};
}

