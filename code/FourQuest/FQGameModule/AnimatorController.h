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
		using ParameterID = std::string;
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
		/// 파라미터 값을 반환합니다 
		/// </summary>
		Parameter GetParameter(ParameterID id)const;

		/// <summary>
		/// 파라미터를 삭제합니다 
		/// </summary>
		void EraseParameter(ParameterID id);

		/// <summary>
		/// 파라미터를 추가합니다 
		/// </summary>
		void AddParameter(ParameterID id, Parameter parameter);

		/// <summary>
		/// 파타미터 팩을 반환합니다  
		/// </summary>
		ParameterPack GetParameterPack()const { return mParmeters; }

	public:
		static constexpr char OnTrigger = static_cast<char>(true);
		static constexpr char OffTrigger = static_cast<char>(false);

	private:
		ParameterPack mParmeters;
	};

}