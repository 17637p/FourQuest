#pragma once

#include "entt.hpp"

namespace fq::reflect
{
	/// <summary>
	/// ���÷����� ����ϴ� Ŭ���� �������̽�
	/// </summary>
	class IHandle
	{
	public:
		/// <summary>
		/// �ڽ��� �ڵ� ��ü�� ��ȯ�մϴ�. 
		/// </summary>
		/// <returns>�ڵ� ��ü</returns>
		virtual entt::meta_handle GetHandle() abstract;
	};

}
