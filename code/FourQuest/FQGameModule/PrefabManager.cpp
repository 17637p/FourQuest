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

		// Tag 설정
		std::string sTag = key.substr(key.find_last_of('-') + 1);
		ETag tag = static_cast<ETag>(std::stoi(sTag));
		object->SetTag(tag);

		key = key.substr(0, key.find_last_of('-'));

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

void fq::game_module::PrefabManager::SavePrefab(GameObject* object, const std::filesystem::path& directory)
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
		std::string sTag = "-" + std::to_string(static_cast<int>(object->GetTag()));

		// 자신은 부모가 있어도 저장할때는 제거합니다
		if (parent == nullptr || tmp == object)
		{
			name = object->GetName() + sTag;
		}
		else // "ParentName-ChildName" 형태로 저장
		{
			name = parent->GetName() + "-" + tmp->GetName() + sTag;
		}

		// json 형식으로 저장
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
		assert(!"파일 열기 실패");
	}
}

nlohmann::json fq::game_module::PrefabManager::saveGameObject(GameObject* object)
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

std::shared_ptr<fq::game_module::GameObject> fq::game_module::PrefabManager::loadGameObject(const nlohmann::json& objectData)
{
	auto  object = ObjectPool::GetInstance()->Assign<GameObject>();

	// 컴포넌트 불러오기
	for (const auto& element : objectData.items())
	{
		const std::string& componentID = element.key();
		entt::id_type id = std::stoul(componentID);

		// 삭제된 컴포넌트에 대한 처리
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

	// 1. 씬에있는 오브젝트를 가져옵니다 
	for (auto& object : scene->GetObjectView())
	{
		q.push(&object);
	}

	// 2. 프리팹 리소스로드 
	while (!q.empty())
	{
		GameObject* object = q.front();
		q.pop();

		// 컴포넌트를 순회하면서 프리팹리소드를 로드
		for (const auto& [componentID, component] : object->GetComponentContainer())
		{
			// PrefabResouece 멤버변수를 탐색
			for (auto [memberID, metaData] : entt::resolve(componentID).data())
			{
				if (metaData.type() == entt::resolve<PrefabResource>())
				{
					// PrefabResource를 Instance원본형태로 로드 
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

					// 프리팹 리소스내부에 존재하는 프리팹 리소도 같이로드
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

	// origin의 복사본을 생성합니다 
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

		// 자식들을 큐에 추가합니다
		for (auto child : orginObject->GetChildren())
		{
			objectQueue.push(child);
		}
	}

	return instance;
}

