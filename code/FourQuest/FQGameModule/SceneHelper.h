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

		/// ������ �̸��� ���ο� ��ȣ�� �ο��Ѵ�.
		static void AddIndexName(std::string& outName);
	};
}