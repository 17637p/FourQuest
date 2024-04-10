#pragma once

#include <string>

namespace fq::game_module
{
	class Scene;
	class GameObject;

	class SceneHeleper
	{
	public:
		static void CheckNameDuplication(Scene& scene,
			GameObject& object);

		static bool HasSameName(Scene& scene, GameObject& object,const std::string& name);

		/// 기존의 이름에 새로운 번호를 부여한다.
		static void AddIndexName(std::string& outName);
	};
}