#pragma once

#include "entt.hpp"

namespace fq::reflect
{
	/// <summary>
	/// 리플렉션을 사용하는 클래스 인터페이스
	/// </summary>
	class IHandle
	{
	public:
		/// <summary>
		/// 자신의 핸들 객체를 반환합니다. 
		/// </summary>
		/// <returns>핸들 객체</returns>
		virtual entt::meta_handle GetHandle() abstract;
	};

}
