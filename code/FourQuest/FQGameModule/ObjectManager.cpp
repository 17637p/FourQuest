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

	// 1. Prefab������ �ҷ�����
	std::ifstream readData(filePath);

	// json ��ü�� key ������ �����ϱ⶧���� 
	// ordered_json�� ����ؼ� ��ü�� �������� �ʽ��ϴ�.
	//https://github.com/nlohmann/json/blob/develop/README.md#order-of-object-keys
	nlohmann::ordered_json prefabJson;

	if (readData.is_open())
	{
		readData >> prefabJson;
		readData.close();
	}
	else
	{
		assert(!"���� ���� ����");
	}

	// 2. json�� Instance�� ��ȯ
	std::map<std::string, GameObject*> matchObject;

	std::vector<std::shared_ptr<fq::game_module::GameObject>> instanceVector{};

	for (const auto& element : prefabJson.items())
	{
		// gameobject instance�� ��ȯ
		instanceVector.push_back(loadGameObject(element.value()));

		// �̸� ����
		const auto& object = instanceVector.back();

		std::string key = element.key();

		// �θ� �ִ� ���
		if (key.find('-') != std::string::npos)
		{
			std::string parentName = key.substr(0, key.find('-'));
			std::string name = key.substr(key.find('-') + 1);
			object->SetName(name);

			// �θ� ������Ʈ ����
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

	// 1. �ڽŰ� �ڽ� ���������� �����մϴ�.
	while (!objectQueue.empty())
	{
		GameObject* tmp = objectQueue.front();
		objectQueue.pop();

		GameObject* parent = tmp->GetParent();

		std::string name;

		// �ڽ��� �θ� �־ �����Ҷ��� �����մϴ�
		if (parent == nullptr || tmp == object)
		{
			name = object->GetName();
		}
		else // "ParentName-ChildName" ���·� ����
		{
			name = parent->GetName() + "-" + tmp->GetName();
		}

		// json �������� ����
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
		assert(!"���� ���� ����");
	}
}

nlohmann::json fq::game_module::ObjectManager::saveGameObject(GameObject* object)
{
	json componentsJson;

	const auto& container = object->GetComponentContainer();

	// ������Ʈ���� ��ȸ�ϸ� �����Ѵ�. 
	for (const auto& [id, component] : container)
	{
		entt::meta_type componentType = entt::resolve(id);
		assert(componentType);

		// Component -> ComponentType���� ��ȯ
		entt::meta_any anyComponent = componentType.from_void(component.get());
		assert(anyComponent);

		mConverter.ParseClassToJson(anyComponent, componentsJson);
	}

	return componentsJson;
}

std::shared_ptr<fq::game_module::GameObject> fq::game_module::ObjectManager::loadGameObject(const nlohmann::json& objectData)
{
	auto  object = std::make_shared<GameObject>();

	// ������Ʈ �ҷ�����
	for (const auto& element : objectData.items())
	{
		const std::string& componentID = element.key();
		const entt::meta_any anyComponent = mConverter.ParseClassFromJson(componentID, element.value());
		object->AddComponent(anyComponent);
	}

	return object;
}
