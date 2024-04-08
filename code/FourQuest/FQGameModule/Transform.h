#pragma once
#include "Component.h"

namespace fq::game_module
{
	/// <summary>
	/// ��ġ, ȸ��, ������, ���� ���� ������ ����
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

		FQ_REGISTRATION_FRIEND
	};
}
