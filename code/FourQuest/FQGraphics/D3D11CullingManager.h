#pragma once
#include <set>
#include <memory>
#include <unordered_map>

#include <DirectXCollision.h>

/// <summary>
/// 컬링 매니저를 만들어보자.
/// 
/// 언젠가 해야할 거 공간분할 -> 여기서? 게임 엔진에서?
/// 
/// 일단 오브젝트마다 바운딩 볼륨을 만들어야 한다.
/// -> 포인터를 ID로 가지고 바운딩 볼륨을 저장한다. 원본 따로 
/// -> 큐를 만들어서 StaticObject가 움직이면 따로 처리를 해주자.
/// 
/// -> 라이트는 그런 거 없어서 구로 범위 만큼 만들어 주면 될 거 같다. 
/// -> 라이트 매니저에서 ? 컬링 매니저에서? O
/// -> 만든 정보는 어디에 저장되어있는지 확인 O 
/// 
/// 카메라 정보가 필요함 O -> 프로젝션 매트릭스로 Frustum 만들기
/// 
/// 드로우하는 일반 오브젝트, 라이트
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
		// Transform을 곱하지 않는 Local 에서의 Volume // 오브젝트가 추가될 때 한 번 생성, 삭제 될 때 삭제
		std::unordered_map<IStaticMeshObject*, DirectX::BoundingBox> mOriBoxes;

		DirectX::BoundingFrustum mCameraFrustum;
	};
}

