#pragma once

#include <unordered_map>
#include <string>

#include "../FQReflect/entt.hpp"

namespace fq::game_module
{
	/// <summary>
	/// 애니메이션의 상태를 컨트롤하는 클래스
	/// </summary>
	class AnimatorController
	{
		using ParameterID = unsigned int;
		using Parameter = entt::meta_any;
		using ParameterPack = std::unordered_map<ParameterID, Parameter>;

	public:
		AnimatorController();
		~AnimatorController();
		
		void SetParameter(ParameterID id, Parameter parameter);

	private:
		ParameterPack mParmeters;
	};

}