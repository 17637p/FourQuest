#pragma once
#include "Component.h"

#include <vector>
#include <directxtk/SimpleMath.h>

namespace fq::game_module
{
	/// <summary>
	/// 위치, 회전, 스케일, 계층 구조 정보를 관리
	/// </summary>
	class Transform : public Component
	{
	public:
		Transform();
		~Transform();


	private:
		DirectX::SimpleMath::Vector3 mPosition;
		DirectX::SimpleMath::Quaternion mRotation;
		DirectX::SimpleMath::Vector3 mScale;

		DirectX::SimpleMath::Matrix mLocalMatrix;
		DirectX::SimpleMath::Matrix mWorldMatrix;
	
		Transform* mParent;
		std::vector<Transform*> mChidren;
	};
}
