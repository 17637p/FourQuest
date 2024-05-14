#pragma once

#include <unordered_map>
#include <string>

#include "../FQReflect/entt.hpp"

namespace fq::game_module
{
	/// <summary>
	/// �ִϸ��̼��� ���¸� ��Ʈ���ϴ� Ŭ����
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