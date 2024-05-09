#pragma once


#include <memory>
#include <vector>
#include <filesystem>

#include "../FQReflect/FQReflect.h"

namespace fq::game_module
{
	class GameObject;

	/// <summary>
	/// GameObject�� Save, Load�ϴ� Ŭ����
	/// </summary>
	class ObjectManager
	{
	public:
		ObjectManager();
		~ObjectManager();

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

	private:
		nlohmann::json saveGameObject(GameObject* object);

		std::shared_ptr<GameObject> loadGameObject(const nlohmann::json& objectData);

	private:
		fq::reflect::Converter mConverter;
	};


}