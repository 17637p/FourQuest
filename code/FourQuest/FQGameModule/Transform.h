#pragma once
#include "Component.h"

#include <vector>
#include <directxtk/SimpleMath.h>

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

		/// <summary>
		/// Local ������ ��ġ ��ȯ�մϴ�
		/// </summary>
		/// <returns></returns>
		DirectX::SimpleMath::Vector3 GetLocalPosition() const { return mPosition; }

		/// <summary>
		/// Local ������ ��ġ �����մϴ�
		/// </summary>
		/// <param name="position"></param>
		void SetPosition(DirectX::SimpleMath::Vector3 position);

		/// <summary>
		/// Local ������ ��ġ�� ��ġ ��ȭ���� ���մϴ�.
		/// </summary>
		/// <param name="deltaPosition">��ġ ��ȭ��</param>
		void AddPosition(DirectX::SimpleMath::Vector3 deltaPosition);

		/// <summary>
		/// World ������ ��ġ�� ��ȯ�մϴ�
		/// </summary>
		/// <returns>World ���� ��ġ</returns>
		DirectX::SimpleMath::Vector3 GetWorldPosition() const;

		/// <summary>
		/// ���� ������ ���ʹϿ� ȸ�� ���� ��ȯ�մϴ�
		/// </summary>
		/// <returns>���ʹϿ� ȸ�� ��</returns>
		DirectX::SimpleMath::Quaternion GetRotation()const { return mRotation; }

		/// <summary>
		/// ȸ�� ���� ��ȯ�մϴ�.
		/// </summary>
		/// <param name="rotation">���ʹϿ� ȸ�� ��</param>
		void SetRotation(DirectX::SimpleMath::Quaternion rotation);

		/// <summary>
		/// ���� ������ ������ ���� ��ȯ�մϴ�
		/// </summary>
		/// <returns>���� ������</returns>
		DirectX::SimpleMath::Vector3 GetScale()const { return mScale; }

		/// <summary>
		/// ������ ���� ��ȯ�մϴ�
		/// </summary>
		/// <param name="scale">��ȯ�ϴ� ������ ��</param>
		void SetScale(DirectX::SimpleMath::Vector3 scale);

		/// <summary>
		/// Ʈ������ ��� ������ �����մϴ�.
		/// ����� scale, rotation, position�� Decompose�Ǿ� ���˴ϴ�.
		/// </summary>
		/// <param name="matrix">���</param>
		void SetTransformMatrix(DirectX::SimpleMath::Matrix matrix);

		/// <summary>
		/// ���� ����� ��ȯ�մϴ�
		/// </summary>
		/// <returns>���� ���</returns>
		DirectX::SimpleMath::Matrix GetLocalMatrix()const { return mLocalMatrix; }

		/// <summary>
		/// ���� ����� ��ȯ�մϴ�
		/// </summary>
		/// <returns>���� ���</returns>
		DirectX::SimpleMath::Matrix GetWorldMatrix()const { return mWorldMatrix; }

		/// <summary>
		/// �θ� �������� Ȯ���մϴ�
		/// </summary>
		/// <returns>�θ� ������ true, �׷��� ������ false�� ��ȯ</returns>
		bool HasParent() const { return mParent != nullptr; }

		/// <summary>
		/// �θ��� Transform�� ��ȯ�մϴ�
		/// </summary>
		/// <returns>�θ� ���� ��� nullptr</returns>
		Transform* GetParentTransform()const { return mParent; }

		/// <summary>
		/// �θ� �����մϴ�
		/// </summary>
		/// <param name="parent">�θ� ������</param>
		void SetParent(Transform* parent);

		/// <summary>
		/// �ڽ��� ���������� �߰��մϴ�.
		/// </summary>
		/// <param name="child">�ڽ� ������</param>
		void AddChild(Transform* inChild);

		/// <summary>
		/// �ڽ��� ������������ �����մϴ�.
		/// </summary>
		/// <param name="removeChild">������ �ڽ�</param>
		void RemoveChild(Transform* removeChild);

	private:
		void updateMatrix();

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
