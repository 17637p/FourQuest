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
		
		/// <summary>
		/// 파라미터 값을 설정합니다 
		/// </summary>
		void SetParameter(ParameterID id, Parameter parameter);

		/// <summary>
		/// 파라미터를 추가합니다 
		/// </summary>
		void AddParameter(ParameterID id, ParameterID parameter);

	private:
		ParameterPack mParmeters;
	};

}