#include "ClothDataLoader.h"
#include "spdlog\spdlog.h"

#include <fstream>
#include <iostream>

namespace fq::game_module
{
	json ClothDataLoader::Vector3ToJson(const DirectX::SimpleMath::Vector3& vec)
	{
		return json{ vec.x, vec.y, vec.z };
	}

	json ClothDataLoader::Vector2ToJson(const DirectX::SimpleMath::Vector2& vec)
	{
		return json{ vec.x, vec.y };
	}

	DirectX::SimpleMath::Vector3 ClothDataLoader::JsonToVector3(const json& vecJson)
	{
		return DirectX::SimpleMath::Vector3(vecJson[0], vecJson[1], vecJson[2]);
	}

	DirectX::SimpleMath::Vector2 ClothDataLoader::JsonToVector2(const json& vecJson)
	{
		return DirectX::SimpleMath::Vector2(vecJson[0], vecJson[1]);
	}

	void ClothDataLoader::Save(const std::shared_ptr<fq::physics::Cloth::ClothData> data, const Path& path)
	{
		ordered_json clothJson;

		clothJson["modelPath"] = data->modelPath;

		// vertices를 JSON 배열로 변환
		clothJson["vertices"] = json::array();
		for (const auto& vertex : data->vertices)
		{
			clothJson["vertices"].push_back(Vector3ToJson(vertex));
		}

		// uv를 JSON 배열로 변환
		clothJson["uvs"] = json::array();
		for (const auto& uv : data->uvs)
		{
			clothJson["uvs"].push_back(Vector2ToJson(uv));
		}

		clothJson["indices"] = data->indices;
		clothJson["disableIndices"] = data->disableIndices;

		// JSON 데이터를 파일로 저장
		std::ofstream file(path);
		std::string filePath = path.string();

		if (file.is_open())
		{
			file << clothJson.dump(6);
			file.close();

			spdlog::trace("[Success] Create \'{}\' File!!", filePath);
		}
		else
		{
			spdlog::error("[Fail] Create \'{}\' File!!", filePath);
		}
	}

	bool ClothDataLoader::LoadArticulationData(const Path& path, fq::physics::Cloth::ClothData& clothData)
	{
		if (!fs::exists(path))
		{
			spdlog::error("[ClothDataLoader ({})] ClothPath is Null", __LINE__);
			return false;
		}
		if (!(path.extension() == ".cloth"))
		{
			spdlog::error("[ClothDataLoader ({})] File is not \'.cloth\' file", __LINE__);
			return false;
		}

		// 파일에서 JSON 데이터를 읽기
		std::ifstream file(path);
		std::string filePath = path.string();

		if (!file.is_open())
		{
			spdlog::error("[ClothDataLoader ({})] Failed Open \'{}\' File!!", __LINE__, filePath);
			return false;
		}

		ordered_json clothJson;
		file >> clothJson;
		file.close();

		// modelPath 읽기
		clothData.modelPath = clothJson["modelPath"];

		// vertices 읽기
		if (clothJson.contains("vertices") && clothJson["vertices"].is_array())
		{
			for (const auto& vertexJson : clothJson["vertices"])
			{
				if (vertexJson.is_array() && vertexJson.size() == 3)
				{
					clothData.vertices.push_back(JsonToVector3(vertexJson));
				}
			}
		}

		// uvs 읽기
		if (clothJson.contains("uvs") && clothJson["uvs"].is_array())
		{
			for (const auto& uvJson : clothJson["uvs"])
			{
				if (uvJson.is_array() && uvJson.size() == 2)
				{
					clothData.uvs.push_back(JsonToVector2(uvJson));
				}
			}
		}

		// indices 읽기
		if (clothJson.contains("indices") && clothJson["indices"].is_array())
		{
			clothData.indices = clothJson["indices"].get<std::vector<unsigned int>>();
		}

		// disableIndices 읽기
		if (clothJson.contains("disableIndices") && clothJson["disableIndices"].is_array())
		{
			clothData.disableIndices = clothJson["disableIndices"].get<std::vector<unsigned int>>();
		}

		return true;
	}
}
