#include "PrefabManager.h"

#include <assert.h>
#include <fstream>
#include <iostream>
#include <queue>

#include "../FQCommon/FQPath.h"
#include "GameObject.h"
#include "Component.h"
#include "Transform.h"
#include "Scene.h"


using json = nlohmann::json;
namespace fs = std::filesystem;

fq::game_module::PrefabManager::PrefabManager()
	:mConverter{}
{}

fq::game_module::PrefabManager::~PrefabManager()
{}

std::vector<std::shared_ptr<fq::game_module::GameObject>> fq::game_module::PrefabManager::LoadPrefab(const std::filesystem::path& filePath)
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

		// Tag ����
		std::string sTag = key.substr(key.find_last_of('-') + 1);
		ETag tag = static_cast<ETag>(std::stoi(sTag));
		object->SetTag(tag);

		key = key.substr(0, key.find_last_of('-'));

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

void fq::game_module::PrefabManager::SavePrefab(GameObject* object, const std::filesystem::path& directory)
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
		std::string sTag = "-" + std::to_string(static_cast<int>(object->GetTag()));

		// �ڽ��� �θ� �־ �����Ҷ��� �����մϴ�
		if (parent == nullptr || tmp == object)
		{
			name = object->GetName() + sTag;
		}
		else // "ParentName-ChildName" ���·� ����
		{
			name = parent->GetName() + "-" + tmp->GetName() + sTag;
		}

		// json �������� ����
		prefabData[name] = saveGameObject(tmp);

		auto children = tmp->GetChildren();
		for (GameObject* child : children)
		{
			objectQueue.push(child);
		}
	}

	auto filePath = fq::path::CreateFilePath(directory, object->GetName(), ".prefab");

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

nlohmann::json fq::game_module::PrefabManager::saveGameObject(GameObject* object)
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

std::shared_ptr<fq::game_module::GameObject> fq::game_module::PrefabManager::loadGameObject(const nlohmann::json& objectData)
{
	auto  object = ObjectPool::GetInstance()->Assign<GameObject>();

	// ������Ʈ �ҷ�����
	for (const auto& element : objectData.items())
	{
		const std::string& componentID = element.key();
		entt::id_type id = std::stoul(componentID);

		// ������ ������Ʈ�� ���� ó��
		if (!entt::resolve(id))
		{
			continue;
		}

		entt::meta_any anyComponent = mConverter.ParseClassFromJson(componentID, element.value());

		object->AddComponent(anyComponent);
	}

	return object;
}

void fq::game_module::PrefabManager::LoadPrefabResource(Scene* scene)
{
	std::queue<GameObject*> q;

	// 1. �����ִ� ������Ʈ�� �����ɴϴ� 
	for (auto& object : scene->GetObjectView())
	{
		q.push(&object);
	}

	// 2. ������ ���ҽ��ε� 
	while (!q.empty())
	{
		GameObject* object = q.front();
		q.pop();

		// ������Ʈ�� ��ȸ�ϸ鼭 �����ո��ҵ带 �ε�
		for (const auto& [componentID, component] : object->GetComponentContainer())
		{
			// PrefabResouece ��������� Ž��
			for (auto [memberID, metaData] : entt::resolve(componentID).data())
			{
				if (metaData.type() == entt::resolve<PrefabResource>())
				{
					// PrefabResource�� Instance�������·� �ε� 
					auto prefabresource = metaData.get(component->GetHandle()).cast<PrefabResource>();
					auto prefabPath = prefabresource.GetPrefabPath();

					if (mPrefabInstances.find(prefabPath) != mPrefabInstances.end()) continue;

					if (!std::filesystem::exists(prefabPath))
					{
						spdlog::warn("[{}-{}-{}] not exist prefab resource", object->GetName()
							, fq::reflect::GetName(entt::resolve(componentID))
							, fq::reflect::GetName(metaData));
						continue;
					}

					auto instance = LoadPrefab(prefabPath);

					// ������ ���ҽ����ο� �����ϴ� ������ ���ҵ� ���̷ε�
					for (auto& object : instance)
					{
						q.push(object.get());
					}

					mPrefabInstances.insert({ prefabPath, std::move(instance) });
				}
			}
		}
	}
}

void fq::game_module::PrefabManager::UnloadPrefabResource()
{
	mPrefabInstances.clear();
}

fq::game_module::PrefabManager::PrefabInstance fq::game_module::PrefabManager::InstantiatePrefabResoure(const PrefabResource& resource) const
{
	auto path = resource.GetPrefabPath();
	auto iter = mPrefabInstances.find(path);
	assert(iter != mPrefabInstances.end());
	const auto& origin = iter->second;

	// origin�� ���纻�� �����մϴ� 
	std::map<std::string, std::shared_ptr<GameObject>> matchParent;
	std::queue<GameObject*> objectQueue;
	objectQueue.push(origin.begin()->get());
	PrefabInstance instance;

	while (!objectQueue.empty())
	{
		auto orginObject = objectQueue.front();
		objectQueue.pop();
		auto clone = std::make_shared<fq::game_module::GameObject>(*orginObject);
		instance.push_back(clone);

		auto parentT = orginObject->GetComponent<Transform>()->GetParentTransform();
		if (parentT != nullptr)
		{
			auto iter = matchParent.find(parentT->GetGameObject()->GetName());
			clone->GetComponent<Transform>()->SetParent(iter->second->GetComponent<Transform>());
		}
		matchParent.insert({ clone->GetName(), clone });

		// �ڽĵ��� ť�� �߰��մϴ�
		for (auto child : orginObject->GetChildren())
		{
			objectQueue.push(child);
		}
	}

	return instance;
}

