#pragma once
#include "Component.h"

#include <vector>

#include "../FQCommon/FQCommon.h"

namespace fq::game_module
{
	/// <summary>
	/// ��ġ, ȸ��, ������, ���� ���� ������ ����
	/// </summary>
	class Transform : public Component
	{
		using FQTransform = fq::common::Transform;
		using Vector3 = DirectX::SimpleMath::Vector3;
		using Quaternion = DirectX::SimpleMath::Quaternion;
		using Matrix = DirectX::SimpleMath::Matrix;

	public:
		Transform();
		~Transform();

		/// <summary>
		/// ���纻�� �Ҵ��ؼ� ��ȯ�մϴ� 
		/// </summary>
		/// <param name="clone"></param>
		/// <returns>���纻</returns>
		Component* Clone(Component* clone /* = nullptr */)const override;

		/// <summary>
		/// ���� ���� ������ �����մϴ� 
		/// </summary>
		/// <param name="position">���� ��ġ</param>
		/// <param name="rotation">���� ȸ��</param>
		/// <param name="scale">���� ������</param>
		void GenerateLocal(Vector3 position, Quaternion rotation, Vector3 scale);

		/// <summary>
		/// ���� ������ ��ġ ��ȯ�մϴ�
		/// </summary>
		/// <returns>���� ��ġ</returns>
		Vector3 GetLocalPosition() const { return mFQTransform.localPosition; }

		/// <summary>
		/// ���� ������ ��ġ �����մϴ�
		/// </summary>
		/// <param name="position">���� ��ġ</param>
		void SetLocalPosition(Vector3 position);

		/// <summary>
		/// ���� ������ ��ġ�� ��ȯ�մϴ�
		/// </summary>
		/// <returns>���� ��ġ</returns>
		Vector3 GetWorldPosition() const { return mFQTransform.worldPosition; }

		/// <summary>
		/// ���� ������ ��ġ�� ��ġ ��ȭ���� ���մϴ�.
		/// </summary>
		/// <param name="deltaPosition">��ġ ��ȭ��</param>
		void AddPosition(Vector3 deltaPosition);

		/// <summary>
		/// ���� ������ ���ʹϿ� ȸ�� ���� ��ȯ�մϴ�
		/// </summary>
		/// <returns>���� ȸ��</returns>
		Quaternion GetLocalRotation()const { return mFQTransform.localRotation; }

		/// <summary>
		/// ���� ������ ���ʹϿ� ȸ�� ���� ��ȯ�մϴ�
		/// </summary>
		/// <returns>���� ȸ��</returns>
		Quaternion GetWorldRotation()const { return mFQTransform.worldRotation; }

		/// <summary>
		/// ���� ������ ȸ�� ���� ��ȯ�մϴ�.
		/// </summary>
		/// <param name="rotation">���ʹϿ� ȸ��</param>
		void SetLocalRotation(Quaternion rotation);

		/// <summary>
		/// ���� ������ ������ ���� ��ȯ�մϴ�
		/// </summary>
		/// <returns>���� ������</returns>
		Vector3 GetLocalScale()const { return mFQTransform.localScale; }

		/// <summary>
		/// ���� ������ ������ ���� ��ȯ�մϴ�
		/// </summary>
		/// <returns>���� ������</returns>
		Vector3 GetWorldScale()const { return mFQTransform.worldScale; }

		/// <summary>
		/// ������ ���� ��ȯ�մϴ�
		/// </summary>
		/// <param name="scale">��ȯ�ϴ� ������ ��</param>
		void SetLocalScale(Vector3 scale);

		/// <summary>
		/// Ʈ������ ��� ������ �����մϴ�.
		/// ����� scale, rotation, position�� Decompose�Ǿ� ���˴ϴ�.
		/// </summary>
		/// <param name="matrix">���</param>
		void SetLocalMatrix(Matrix matrix);

		/// <summary>
		/// ���� ����� ��ȯ�մϴ�
		/// </summary>
		/// <returns>���� ���</returns>
		Matrix GetLocalMatrix()const { return mFQTransform.localMatrix; }

		/// <summary>
		/// ���� ����� ��ȯ�մϴ�
		/// </summary>
		/// <returns>���� ���</returns>
		Matrix GetWorldMatrix()const { return mFQTransform.worldMatrix; }

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

		/// <summary>
		/// ���������� �ڽĵ��� ��ȯ�մϴ�
		/// </summary>
		/// <returns>���������� �ڽĵ�</returns>
		const std::vector<Transform*>& GetChildren()const { return mChidren; }

		/// <summary>
		/// transform�� �ڽİ����� ���ϰ� �ִ��� Ȯ���մϴ�
		/// </summary>
		/// <param name="transfrom">Ȯ���� transform</param>
		/// <returns>���ϸ� true, �׷��� ������ false</returns>
		bool IsDescendant(Transform* transfrom)const;

		/// <summary>
		/// �ڽ� ������ ���θ� Ȯ���մϴ�
		/// </summary>
		/// <returns>�ڽ��� ������ true, �׷��������� false</returns>
		bool IsLeaf()const;

	private:
		void decomposeWorldMatrix();
		void decomposeLocalMatrix();
		void updateWorldMatrix();
		void updateLocalMatrix();

		entt::meta_handle GetHandle() override;

	private:
		FQTransform mFQTransform;

		Transform* mParent;
		std::vector<Transform*> mChidren;
	};
}
