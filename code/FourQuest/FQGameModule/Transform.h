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

		/// <summary>
		/// Local 공간의 위치 반환합니다
		/// </summary>
		/// <returns></returns>
		DirectX::SimpleMath::Vector3 GetLocalPosition() const { return mPosition; }

		/// <summary>
		/// Local 공간의 위치 설정합니다
		/// </summary>
		/// <param name="position"></param>
		void SetPosition(DirectX::SimpleMath::Vector3 position);

		/// <summary>
		/// Local 공간의 위치에 위치 변화량을 더합니다.
		/// </summary>
		/// <param name="deltaPosition">위치 변화량</param>
		void AddPosition(DirectX::SimpleMath::Vector3 deltaPosition);

		/// <summary>
		/// World 공간의 위치를 반환합니다
		/// </summary>
		/// <returns>World 공간 위치</returns>
		DirectX::SimpleMath::Vector3 GetWorldPosition() const;

		/// <summary>
		/// 로컬 공간의 쿼터니온 회전 값을 반환합니다
		/// </summary>
		/// <returns>쿼터니온 회전 값</returns>
		DirectX::SimpleMath::Quaternion GetRotation()const { return mRotation; }

		/// <summary>
		/// 회전 값을 변환합니다.
		/// </summary>
		/// <param name="rotation">쿼터니온 회전 값</param>
		void SetRotation(DirectX::SimpleMath::Quaternion rotation);

		/// <summary>
		/// 로컬 공간의 스케일 값을 반환합니다
		/// </summary>
		/// <returns>로컬 스케일</returns>
		DirectX::SimpleMath::Vector3 GetScale()const { return mScale; }

		/// <summary>
		/// 스케일 값을 변환합니다
		/// </summary>
		/// <param name="scale">변환하는 스케일 값</param>
		void SetScale(DirectX::SimpleMath::Vector3 scale);

		/// <summary>
		/// 트랜스폼 행렬 값으로 설정합니다.
		/// 행렬은 scale, rotation, position로 Decompose되어 사용됩니다.
		/// </summary>
		/// <param name="matrix">행렬</param>
		void SetTransformMatrix(DirectX::SimpleMath::Matrix matrix);

		/// <summary>
		/// 로컬 행렬을 반환합니다
		/// </summary>
		/// <returns>로컬 행렬</returns>
		DirectX::SimpleMath::Matrix GetLocalMatrix()const { return mLocalMatrix; }

		/// <summary>
		/// 월드 행렬을 반환합니다
		/// </summary>
		/// <returns>월드 행렬</returns>
		DirectX::SimpleMath::Matrix GetWorldMatrix()const { return mWorldMatrix; }

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
