#pragma once
#include <set>
#include <memory>
#include <unordered_map>

#include <DirectXCollision.h>

/// <summary>
/// �ø� �Ŵ����� ������.
/// 
/// ������ �ؾ��� �� �������� -> ���⼭? ���� ��������?
/// 
/// �ϴ� ������Ʈ���� �ٿ�� ������ ������ �Ѵ�.
/// -> �����͸� ID�� ������ �ٿ�� ������ �����Ѵ�. ���� ���� 
/// -> ť�� ���� StaticObject�� �����̸� ���� ó���� ������.
/// 
/// -> ����Ʈ�� �׷� �� ��� ���� ���� ��ŭ ����� �ָ� �� �� ����. 
/// -> ����Ʈ �Ŵ������� ? �ø� �Ŵ�������? O
/// -> ���� ������ ��� ����Ǿ��ִ��� Ȯ�� O 
/// 
/// ī�޶� ������ �ʿ��� O -> �������� ��Ʈ������ Frustum �����
/// 
/// ��ο��ϴ� �Ϲ� ������Ʈ, ����Ʈ
/// 
/// </summary>

namespace DirectX 
{ 
	namespace SimpleMath
	{
		struct Vector3;
		struct Quaternion;
		struct Matrix;
	}
	struct BoundingFrustum; 
}

namespace fq::graphics
{
	class IStaticMeshObject;
	class ISkinnedMeshObject;

	class D3D11CullingManager
	{
	public:
		void UpdateCameraFrustum(
			const DirectX::SimpleMath::Vector3& position,
			const DirectX::SimpleMath::Quaternion& rotation,
			const DirectX::SimpleMath::Matrix& projectionMatrix);

		void CreateBoundingBoxOfStaticObject(IStaticMeshObject* meshObject);
		void DeleteBoundingBoxOfStaticObject(IStaticMeshObject* meshObject);

		std::set<IStaticMeshObject*> GetInFrustumStaticObjects(const std::set<IStaticMeshObject*>& staticObjects);
		std::set<ISkinnedMeshObject*> GetInFrustumSkinnedObjects(const std::set<ISkinnedMeshObject*>& skinnedObjects);

	private:
		// Transform�� ������ �ʴ� Local ������ Volume // ������Ʈ�� �߰��� �� �� �� ����, ���� �� �� ����
		std::unordered_map<IStaticMeshObject*, DirectX::BoundingBox> mOriBoxes;

		DirectX::BoundingFrustum mCameraFrustum;
	};
}

