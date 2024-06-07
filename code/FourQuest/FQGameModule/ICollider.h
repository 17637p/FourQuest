#pragma once

#include <directxtk/SimpleMath.h>

namespace fq::game_module
{
	class ICollider
	{
	public:
		virtual ~ICollider(){}

		/// <summary>
		/// 콜라이더의 오프셋을 반환합니다.
		/// </summary>
		virtual DirectX::SimpleMath::Vector3 GetOffset() const abstract;
		
		/// <summary>
		/// 콜라이더의 오프셋을 설정합니다.
		/// </summary>
		virtual void SetOffset(DirectX::SimpleMath::Vector3 offset) abstract;
	};
}
