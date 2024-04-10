#pragma once


#include <memory>
#include <vector>
#include <filesystem>

#include "../FQReflect/FQReflect.h"

namespace fq::game_module
{
	class GameObject;

	/// <summary>
	/// GameObject를 Save, Load하는 클래스
	/// </summary>
	class ObjectManager
	{
	public:
		ObjectManager();
		~ObjectManager();

		/// <summary>
		/// Prefab을 Gameobject로 인스턴스화합니다
		/// </summary>
		/// <param name="filePath">Prefab 파일경로</param>
		/// <returns>인스턴스화한 GameObject</returns>
		std::vector<std::shared_ptr<GameObject>> LoadPrefab(const std::filesystem::path& filePath);

		/// <summary>
		/// GameObject를 Prefab으로 저장합니다
		/// Prefab은 Gameobject의 계층구조의 자식계층과 함께 저장됩니다.
		/// </summary>
		/// <param name="object">프리팹화할 GameObject</param>
		/// <param name="directory">저장할 파일 경로</param>
		void SavePrefab(GameObject* object, const std::filesystem::path& directory);

	private:
		nlohmann::json saveGameObject(GameObject* object);

		std::shared_ptr<GameObject> loadGameObject(const nlohmann::json& objectData);

	private:
		fq::reflect::Converter mConverter;
	};


}