#pragma once

#include <directxtk/SimpleMath.h>

namespace fq::game_module
{
	class ICollider
	{
	public:
		virtual ~ICollider(){}

		/// <summary>
		/// �ݶ��̴��� �������� ��ȯ�մϴ�.
		/// </summary>
		virtual DirectX::SimpleMath::Vector3 GetOffset() const abstract;
		
		/// <summary>
		/// �ݶ��̴��� �������� �����մϴ�.
		/// </summary>
		virtual void SetOffset(DirectX::SimpleMath::Vector3 offset) abstract;
	};
}
