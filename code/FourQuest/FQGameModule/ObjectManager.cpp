#include "ObjectManager.h"

#include <assert.h>
#include <fstream>
#include <iostream>
#include <queue>

#include "GameObject.h"
#include "Component.h"
#include "Transform.h"

using json = nlohmann::json;
namespace fs = std::filesystem;

fq::game_module::ObjectManager::ObjectManager()
	:mConverter{}
{}

fq::game_module::ObjectManager::~ObjectManager()
{}

std::vector<std::shared_ptr<fq::game_module::GameObject>> fq::game_module::ObjectManager::LoadPrefab(const std::filesystem::path& filePath)
{

	// 1. Prefab데이터 불러오기
	std::ifstream readData(filePath);

	// json 객체는 key 값으로 정렬하기때문에 
	// ordered_json을 사용해서 객체를 정렬하지 않습니다.
	//https://github.com/nlohmann/json/blob/develop/README.md#order-of-object-keys
	nlohmann::ordered_json prefabJson;

	if (readData.is_open())
	{
		readData >> prefabJson;
		readData.close();
	}
	else
	{
		assert(!"파일 열기 실패");
	}

	// 2. json을 Instance로 변환
	std::map<std::string, GameObject*> matchObject;

	std::vector<std::shared_ptr<fq::game_module::GameObject>> instanceVector{};

	for (const auto& element : prefabJson.items())
	{
		// gameobject instance르 변환
		instanceVector.push_back(loadGameObject(element.value()));

		// 이름 설정
		const auto& object = instanceVector.back();

		std::string key = element.key();

		// 부모가 있는 경우
		if (key.find('-') != std::string::npos)
		{
			std::string parentName = key.substr(0, key.find('-'));
			std::string name = key.substr(key.find('-') + 1);
			object->SetName(name);

			// 부모 오브젝트 연결
			GameObject* parent = matchObject[parentName];
			object->GetComponent<Transform>()->SetParent(parent->GetComponent<Transform>());
		}
		else
		{
			object->SetName(key);
		}

		matchObject.insert({ object->GetName(), object.get() });
	}


	return instanceVector;
}

void fq::game_module::ObjectManager::SavePrefab(GameObject* object, const std::filesystem::path& directory)
{
	nlohmann::ordered_json prefabData;

	std::queue<GameObject*> objectQueue;
	objectQueue.push(object);

	// 1. 자신과 자식 계층구조를 저장합니다.
	while (!objectQueue.empty())
	{
		GameObject* tmp = objectQueue.front();
		objectQueue.pop();

		GameObject* parent = tmp->GetParent();

		std::string name;

		// 자신은 부모가 있어도 저장할때는 제거합니다
		if (parent == nullptr || tmp == object)
		{
			name = object->GetName();
		}
		else // "ParentName-ChildName" 형태로 저장
		{
			name = parent->GetName() + "-" + tmp->GetName();
		}

		// json 형식으로 저장
		prefabData[name] = saveGameObject(tmp);

		auto children = tmp->GetChildren();
		for (GameObject* child : children)
		{
			objectQueue.push(child);
		}
	}

	auto filePath = fq::path::CreateFilePath(directory, object->GetName(), ".json");

	std::ofstream output(filePath);

	if (output.is_open())
	{
		output << std::setw(4) << prefabData;
		output.close();
	}
	else
	{
		assert(!"파일 열기 실패");
	}
}

nlohmann::json fq::game_module::ObjectManager::saveGameObject(GameObject* object)
{
	json componentsJson;

	const auto& container = object->GetComponentContainer();

	// 컴포넌트들을 순회하면 저장한다. 
	for (const auto& [id, component] : container)
	{
		entt::meta_type componentType = entt::resolve(id);
		assert(componentType);

		// Component -> ComponentType으로 변환
		entt::meta_any anyComponent = componentType.from_void(component.get());
		assert(anyComponent);

		mConverter.ParseClassToJson(anyComponent, componentsJson);
	}

	return componentsJson;
}

std::shared_ptr<fq::game_module::GameObject> fq::game_module::ObjectManager::loadGameObject(const nlohmann::json& objectData)
{
	auto  object = std::make_shared<GameObject>();

	// 컴포넌트 불러오기
	for (const auto& element : objectData.items())
	{
		const std::string& componentID = element.key();
		const entt::meta_any anyComponent = mConverter.ParseClassFromJson(componentID, element.value());
		object->AddComponent(anyComponent);
	}

	return object;
}
