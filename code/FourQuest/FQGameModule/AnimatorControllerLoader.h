#pragma once

#include <filesystem>
#include <memory>
#include "../FQReflect/FQReflect.h"

namespace fq::game_module
{
	class AnimatorController;
	class AnimationStateNode;

	/// <summary>
	/// Animation Controller를 저장, 로드하는 클래스
	/// </summary>
	class AnimatorControllerLoader
	{
		using Path = std::filesystem::path;
	public:
		/// <summary>
		/// Controller를 저장합니다 
		/// </summary>
		void Save(const AnimatorController& controller, const Path& path);
		
		/// <summary>
		/// Controller를 로드합니다 
		/// </summary>
		std::shared_ptr<AnimatorController> Load(const Path& path);

	private:
		nlohmann::json serializeStateBehaviours(const AnimationStateNode& node);

	private:
		fq::reflect::Converter mConverter;
	};
}