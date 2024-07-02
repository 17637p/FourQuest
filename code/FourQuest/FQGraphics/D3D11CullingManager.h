#pragma once
#include <set>
#include <memory>

#include <DirectXCollision.h>

/// <summary>
/// 컬링 매니저를 만들어보자.
/// 
/// 언젠가 해야할 거 공간분할 -> 여기서? 게임 엔진에서?
/// 
/// 일단 오브젝트마다 바운딩 볼륨을 만들어야 한다.
/// -> 불러올 떄 만든다 그래서 검사만 해주면 될 거 같다.
/// -> 라이트는 그런 거 없어서 구로 범위 만큼 만들어 주면 될 거 같다. 
/// -> 라이트 매니저에서 ? 컬링 매니저에서? O
/// -> 만든 정보는 어디에 저장되어있는지 확인 O 
/// 
/// 카메라 정보가 필요함 O -> 프로젝션 매트릭스로 Frustum 만들기
/// 
/// 드로우하는 일반 오브젝트, 라이트
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

