#pragma once


#include <memory>
#include <vector>
#include <filesystem>
#include <unordered_map>

#include "../FQReflect/FQReflect.h"
#include "PrefabResource.h"

namespace fq::game_module
{
	class GameObject;
	class Scene;

	/// <summary>
	/// �������� �����ϴ� Ŭ����
	/// </summary>
	class PrefabManager
	{
		using PrefabInstance = std::vector<std::shared_ptr<GameObject>>;
		using PrefabContainer = std::unordered_map<std::wstring, PrefabInstance>;
	public:
		PrefabManager();
		~PrefabManager();

		/// <summary>
		/// Prefab�� Gameobject�� �ν��Ͻ�ȭ�մϴ�
		/// </summary>
		/// <param name="filePath">Prefab ���ϰ��</param>
		/// <returns>�ν��Ͻ�ȭ�� GameObject</returns>
		std::vector<std::shared_ptr<GameObject>> LoadPrefab(const std::filesystem::path& filePath);

		/// <summary>
		/// GameObject�� Prefab���� �����մϴ�
		/// Prefab�� Gameobject�� ���������� �ڽİ����� �Բ� ����˴ϴ�.
		/// </summary>
		/// <param name="object">������ȭ�� GameObject</param>
		/// <param name="directory">������ ���� ���</param>
		void SavePrefab(GameObject* object, const std::filesystem::path& directory);

		/// <summary>
		/// ������ ���ҽ��� �޸𸮿� �ε��մϴ� 
		/// </summary>
		void LoadPrefabResource(Scene* scene);

		/// <summary>
		/// ������ �ν��Ͻ����� �޸𸮿��� ��ε��մϴ� 
		/// </summary>
		void UnloadPrefabResource();


	private:
		nlohmann::json saveGameObject(GameObject* object);
		std::shared_ptr<GameObject> loadGameObject(const nlohmann::json& objectData);

	private:
		fq::reflect::Converter mConverter;
		PrefabContainer mPrefabInstances;
	};
}