#include "ClothDataLoader.h"

#include <fstream>
#include <iostream>

namespace fq::game_module
{
	json ClothDataLoader::Vector3ToJson(const DirectX::SimpleMath::Vector3& vec)
	{
		return json{ vec.x, vec.y, vec.z };
	}

	DirectX::SimpleMath::Vector3 ClothDataLoader::JsonToVector3(const json& vecJson)
	{
		return DirectX::SimpleMath::Vector3(vecJson[0], vecJson[1], vecJson[2]);
	}

	void ClothDataLoader::Save(const std::shared_ptr<ClothData> data, const Path& path)
	{
		ordered_json clothJson;

		// vertices�� JSON �迭�� ��ȯ
		clothJson["vertices"] = json::array();
		for (const auto& vertex : data->vertices)
		{
			clothJson["vertices"].push_back(Vector3ToJson(vertex));
		}

		clothJson["indices"] = data->indices;
		clothJson["disableIndices"] = data->disableIndices;

		// JSON �����͸� ���Ϸ� ����
		std::ofstream file(path);
		if (file.is_open())
		{
			file << clothJson.dump(6);
			file.close();
		}
		else
		{
			assert(false);
		}
	}

	std::shared_ptr<ClothData> ClothDataLoader::LoadArticulationData(const Path& path)
	{
		assert(fs::exists(path));
		assert(path.extension() == ".cloth");

		std::shared_ptr<ClothData> clothData = std::make_shared<ClothData>();

		// ���Ͽ��� JSON �����͸� �б�
		std::ifstream file(path);
		if (!file.is_open())
		{
			std::cerr << "Failed to open file: " << path << std::endl;
			assert(false);
			return nullptr;
		}

		ordered_json clothJson;
		file >> clothJson;
		file.close();

		// vertices �б�
		if (clothJson.contains("vertices") && clothJson["vertices"].is_array())
		{
			for (const auto& vertexJson : clothJson["vertices"])
			{
				if (vertexJson.is_array() && vertexJson.size() == 3)
				{
					clothData->vertices.push_back(JsonToVector3(vertexJson));
				}
			}
		}

		// indices �б�
		if (clothJson.contains("indices") && clothJson["indices"].is_array())
		{
			clothData->indices = clothJson["indices"].get<std::vector<unsigned int>>();
		}

		// disableIndices �б�
		if (clothJson.contains("disableIndices") && clothJson["disableIndices"].is_array())
		{
			clothData->disableIndices = clothJson["disableIndices"].get<std::vector<unsigned int>>();
		}

		return clothData;
	}
}
