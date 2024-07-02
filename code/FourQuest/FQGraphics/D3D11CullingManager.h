#pragma once
#include <set>
#include <memory>

#include <DirectXCollision.h>

/// <summary>
/// �ø� �Ŵ����� ������.
/// 
/// ������ �ؾ��� �� �������� -> ���⼭? ���� ��������?
/// 
/// �ϴ� ������Ʈ���� �ٿ�� ������ ������ �Ѵ�.
/// -> �ҷ��� �� ����� �׷��� �˻縸 ���ָ� �� �� ����.
/// -> ����Ʈ�� �׷� �� ��� ���� ���� ��ŭ ����� �ָ� �� �� ����. 
/// -> ����Ʈ �Ŵ������� ? �ø� �Ŵ�������? O
/// -> ���� ������ ��� ����Ǿ��ִ��� Ȯ�� O 
/// 
/// ī�޶� ������ �ʿ��� O -> �������� ��Ʈ������ Frustum �����
/// 
/// ��ο��ϴ� �Ϲ� ������Ʈ, ����Ʈ
/// 
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

		std::set<std::shared_ptr<IStaticMeshObject>> GetInFrustumStaticObjects(const std::set<std::shared_ptr<IStaticMeshObject>>& staticObjects);
		std::set<std::shared_ptr<ISkinnedMeshObject>> GetInFrustumSkinnedObjects(const std::set<std::shared_ptr<ISkinnedMeshObject>>& skinnedObjects);

	private:
		DirectX::BoundingFrustum cameraFrustum;
	};
}

