#include "ObjectManager.h"

#include <assert.h>
#include <fstream>
#include <iostream>

#include "GameObject.h"
#include "Component.h"

using json = nlohmann::json;
namespace fs = std::filesystem;

fq::game_module::ObjectManager::ObjectManager()
	:mConverter{}
{}

fq::game_module::ObjectManager::~ObjectManager()
{}

std::shared_ptr<fq::game_module::GameObject> fq::game_module::ObjectManager::LoadPrefab(const std::filesystem::path& filePath) 
{
	std::ifstream readData(filePath);

	json prefabJson;
	if (readData.is_open())
	{
		readData >> prefabJson;
		readData.close();
	}
	else
	{
		assert(!"파일 열기 실패");
	}


	return nullptr;
}

void fq::game_module::ObjectManager::SavePrefab(GameObject* object, const std::filesystem::path& directory) 
{
	json prefab;

	prefab =  saveGameObject(object);

	std::cout << prefab;
}

nlohmann::json fq::game_module::ObjectManager::saveGameObject(GameObject* object) 
{
	json componentsJson;

	const auto& container = object->GetComponentContainer();

	// 컴포넌트들을 순회하면 저장한다. 
	for (const auto& [id, component] : container)
	{
		entt::meta_type componentType =  entt::resolve(id);
		assert(componentType);

		// Component -> ComponentType으로 변환
		entt::meta_any anyComponent =  componentType.from_void(component.get());
		assert(anyComponent);

		mConverter.ParseClassToJson(anyComponent, componentsJson);
	}

	json GameObjectJson;

	GameObjectJson[object->GetName()] = componentsJson;

	return GameObjectJson;
}
