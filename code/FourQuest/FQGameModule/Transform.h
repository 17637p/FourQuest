#pragma once
#include "Component.h"

#include <vector>

#include "../FQCommon/FQCommon.h"

namespace fq::game_module
{
	/// <summary>
	/// 위치, 회전, 스케일, 계층 구조 정보를 관리
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
		/// 복사본을 할당해서 반환합니다 
		/// </summary>
		/// <param name="clone"></param>
		/// <returns>복사본</returns>
		Component* Clone(Component* clone /* = nullptr */)const override;

		/// <summary>
		/// 로컬 공간 정보를 갱신합니다 
		/// </summary>
		/// <param name="position">로컬 위치</param>
		/// <param name="rotation">로컬 회전</param>
		/// <param name="scale">로컬 스케일</param>
		void GenerateLocal(Vector3 position, Quaternion rotation, Vector3 scale);

		/// <summary>
		/// 로컬 공간의 위치 반환합니다
		/// </summary>
		/// <returns>로컬 위치</returns>
		Vector3 GetLocalPosition() const { return mFQTransform.localPosition; }

		/// <summary>
		/// 로컬 공간의 위치 설정합니다
		/// </summary>
		/// <param name="position">로컬 위치</param>
		void SetLocalPosition(Vector3 position);

		/// <summary>
		/// 월드 공간의 위치를 반환합니다
		/// </summary>
		/// <returns>월드 위치</returns>
		Vector3 GetWorldPosition() const { return mFQTransform.worldPosition; }

		/// <summary>
		/// 로컬 공간의 위치에 위치 변화량을 더합니다.
		/// </summary>
		/// <param name="deltaPosition">위치 변화량</param>
		void AddPosition(Vector3 deltaPosition);

		/// <summary>
		/// 로컬 공간의 쿼터니온 회전 값을 반환합니다
		/// </summary>
		/// <returns>로컬 회전</returns>
		Quaternion GetLocalRotation()const { return mFQTransform.localRotation; }

		/// <summary>
		/// 월드 공간의 쿼터니온 회전 값을 반환합니다
		/// </summary>
		/// <returns>월드 회전</returns>
		Quaternion GetWorldRotation()const { return mFQTransform.worldRotation; }

		/// <summary>
		/// 로컬 공간의 회전 값을 변환합니다.
		/// </summary>
		/// <param name="rotation">쿼터니온 회전</param>
		void SetLocalRotation(Quaternion rotation);

		/// <summary>
		/// 로컬 공간의 스케일 값을 반환합니다
		/// </summary>
		/// <returns>로컬 스케일</returns>
		Vector3 GetLocalScale()const { return mFQTransform.localScale; }

		/// <summary>
		/// 월드 공간의 스케일 값을 반환합니다
		/// </summary>
		/// <returns>월드 스케일</returns>
		Vector3 GetWorldScale()const { return mFQTransform.worldScale; }

		/// <summary>
		/// 스케일 값을 변환합니다
		/// </summary>
		/// <param name="scale">변환하는 스케일 값</param>
		void SetLocalScale(Vector3 scale);

		/// <summary>
		/// 트랜스폼 행렬 값으로 설정합니다.
		/// 행렬은 scale, rotation, position로 Decompose되어 사용됩니다.
		/// </summary>
		/// <param name="matrix">행렬</param>
		void SetLocalMatrix(Matrix matrix);

		/// <summary>
		/// 로컬 행렬을 반환합니다
		/// </summary>
		/// <returns>로컬 행렬</returns>
		Matrix GetLocalMatrix()const { return mFQTransform.localMatrix; }

		/// <summary>
		/// 월드 행렬을 반환합니다
		/// </summary>
		/// <returns>월드 행렬</returns>
		Matrix GetWorldMatrix()const { return mFQTransform.worldMatrix; }

		/// <summary>
		/// 부모를 가지는지 확인합니다
		/// </summary>
		/// <returns>부모를 가지면 true, 그렇지 않으면 false를 반환</returns>
		bool HasParent() const { return mParent != nullptr; }

		/// <summary>
		/// 부모의 Transform을 반환합니다
		/// </summary>
		/// <returns>부모가 없는 경우 nullptr</returns>
		Transform* GetParentTransform()const { return mParent; }

		/// <summary>
		/// 부모를 설정합니다
		/// </summary>
		/// <param name="parent">부모 포인터</param>
		void SetParent(Transform* parent);

		/// <summary>
		/// 자식을 계층구조에 추가합니다.
		/// </summary>
		/// <param name="child">자식 포인터</param>
		void AddChild(Transform* inChild);

		/// <summary>
		/// 자식을 계층구조에서 제거합니다.
		/// </summary>
		/// <param name="removeChild">제거할 자식</param>
		void RemoveChild(Transform* removeChild);

		/// <summary>
		/// 계층구조의 자식들을 반환합니다
		/// </summary>
		/// <returns>계층구조의 자식들</returns>
		const std::vector<Transform*>& GetChildren()const { return mChidren; }

		/// <summary>
		/// transform이 자식계층에 속하고 있는지 확인합니다
		/// </summary>
		/// <param name="transfrom">확인할 transform</param>
		/// <returns>속하면 true, 그렇지 않으면 false</returns>
		bool IsDescendant(Transform* transfrom)const;

		/// <summary>
		/// 자식 계층의 여부를 확인합니다
		/// </summary>
		/// <returns>자식이 없으면 true, 그렇지않으면 false</returns>
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
