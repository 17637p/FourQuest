#pragma once

#include <filesystem>
#include <memory>
#include "../FQReflect/FQReflect.h"

namespace fq::game_module
{
	class AnimatorController;
	class AnimationStateNode;

	/// <summary>
	/// Animation Controller�� ����, �ε��ϴ� Ŭ����
	/// </summary>
	class AnimatorControllerLoader
	{
		using Path = std::filesystem::path;
	public:
		/// <summary>
		/// Controller�� �����մϴ� 
		/// </summary>
		void Save(const AnimatorController& controller, const Path& path);
		
		/// <summary>
		/// Controller�� �ε��մϴ� 
		/// </summary>
		std::shared_ptr<AnimatorController> Load(const Path& path);

	private:
		nlohmann::json serializeStateBehaviours(const AnimationStateNode& node);

	private:
		fq::reflect::Converter mConverter;
	};
}