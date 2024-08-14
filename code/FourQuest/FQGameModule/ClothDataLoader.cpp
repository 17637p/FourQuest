#include "ClothDataLoader.h"

#include <fstream>
#include <iostream>

namespace fq::game_module
{
	void ClothDataLoader::Save(const std::shared_ptr<ClothData> data, const Path& path)
	{
		ordered_json clothJson;

		// JSON 객체 생성
		clothJson["vertices"] = data->vertices;
		clothJson["indices"] = data->indices;
		clothJson["disableIndices"] = data->disableIndices;

		// JSON 데이터를 파일로 저장
		std::ofstream file(path);
		if (file.is_open())
		{
			file << clothJson.dump(6);
			file.close();
		}
		else
		{
			assert(nullptr);
		}
	}

	std::shared_ptr<ClothData> ClothDataLoader::LoadArticulationData(const Path& path)
	{
		assert(fs::exists(path));
		assert(path.extension() == ".cloth");

		std::shared_ptr<ClothData> clothData;




		return std::shared_ptr<ClothData>();
	}
}
