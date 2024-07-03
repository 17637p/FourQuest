#pragma once
#include <unordered_map>
#include <memory>
#include <string>

#include <directxtk/SimpleMath.h>

#pragma comment(lib, "../FQGraphics/tetgen.lib")
#include "../FQGraphics/tetgen.h"

namespace fq::graphics
{
	class D3D11Device;
	class D3D11ResourceManager;

	struct CubeProbe
	{
		int index;
		DirectX::SimpleMath::Vector3 position;
	};

	// -1이면 유효하지 않은 라이트 프로브
	struct LightProbe
	{
		int index;
		DirectX::SimpleMath::Vector3 position;
	};

	struct Tetrahedron
	{
		LightProbe* probes[4]; // vertice 정보가 필요함!
		int neighbors[4];

		DirectX::SimpleMath::Matrix matrix; // 4x3이 맞다
	};

	class D3D11LightProbeManager
	{
	public:
		D3D11LightProbeManager();
		~D3D11LightProbeManager();

		void Initialize(std::shared_ptr<D3D11Device> device, std::shared_ptr<D3D11ResourceManager> resourceManager);

		int AddCubeProbe(const DirectX::SimpleMath::Vector3& position);
		void DeleteCubeProbe(int index);

		int AddLightProbe(const DirectX::SimpleMath::Vector3& position);
		void DeleteLightProbe(int index);

		std::vector<CubeProbe*> GetCubeProbes() const;
		std::vector<LightProbe*> GetLightProbes() const;

		std::wstring SaveCubeProbeTexture(const int index, const std::wstring& direction); //return path Name

		void MakeTetrahedron();

		// p는 오브젝트 pos, coords는 out 할 무게중심 좌표, vertices는 Tet의 vertices이므로 Tet을 보내면 될 거 같은디 
		void GetBarycentriCoordinateForInnerTetrahedron(const DirectX::SimpleMath::Vector3& p,
			const Tetrahedron& tet, DirectX::SimpleMath::Vector4& coords);
		void GetLightProbeInterpolationWeights(const std::vector<Tetrahedron> tets, const DirectX::SimpleMath::Vector3& position, int tetIndex,
			DirectX::SimpleMath::Vector4& weights, float& t, int& steps);
		//void GetBarycentricCoordinates(const std::vector<DirectX::SimpleMath::Vector3> vertices, const std::vector<DirectX::SimpleMath::Vector3>& hullRays,
		//	const DirectX::SimpleMath::Vector3& p, const Tetrahedron& tet, DirectX::SimpleMath::Vector4& coords, float& t);
		/// Outer Cells에 관련된 거라 일단 보류 
		/// 그냥 예외처리로 아무것도 안하면 안되나?
		//void GetBarycentricCoordinatesForOuterCell(const std::vector<DirectX::SimpleMath::Vector3>& vertices,
		//	const std::vector<DirectX::SimpleMath::Vector3>& hullRays,
		//	const DirectX::SimpleMath::Vector3& p,
		//	const Tetrahedron& tet,
		//	DirectX::SimpleMath::Vector4& coord,
		//	float& t);
		// 
		//void CalculateOuterCellsMatrices(LightProbeCloudData& data, int innerTetrahedraCount);

	private:
		std::shared_ptr<D3D11Device> mDevice;
		std::shared_ptr<D3D11ResourceManager> mResourceManager;

		int mCubeProbeIndex;
		int mLightProbeIndex;
		std::vector<CubeProbe*> mCubeProbes;
		std::vector<LightProbe*> mLightProbes;
		std::vector<Tetrahedron*> mTetrahedrons;
	};
}

