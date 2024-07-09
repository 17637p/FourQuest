#pragma once
#include <unordered_map>
#include <memory>
#include <string>

#include <directxtk/SimpleMath.h>

#ifdef _DEBUG
#pragma comment(lib, "../FQGraphics/tetgen.lib")
#else
#pragma comment(lib, "../FQGraphics/tetgenL.lib")
#endif // DEBUG

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

	struct LightProbe
	{
		int index; // 저장했다가 불러올 때 필요
		DirectX::SimpleMath::Vector3 position;

		float coefficient[27]; // 0~8 Red, 9~17 Green, 18~26 Blue
	};

	struct Tetrahedron
	{
		LightProbe* probes[4]; // vertice 정보가 필요함!
		int neighbors[4];

		DirectX::SimpleMath::Matrix matrix; // 4x3이 맞다
	};

	// 배치하기
	// 큐브맵 저장하기
	// bakeAllLightProbe
	// 사면체 만들기
	// GetTetIndex - 오브젝트마다
	// getcoefficientTetrahedron

	class D3D11LightProbeManager
	{
	public:
		D3D11LightProbeManager();
		~D3D11LightProbeManager();

		void Initialize(std::shared_ptr<D3D11Device> device, std::shared_ptr<D3D11ResourceManager> resourceManager);

		/// Add Delete
		int AddCubeProbe(const DirectX::SimpleMath::Vector3& position);
		void DeleteCubeProbe(int index);

		int AddLightProbe(const DirectX::SimpleMath::Vector3& position);
		void DeleteLightProbe(int index);

		/// Get
		std::vector<CubeProbe*> GetCubeProbes() const;
		std::vector<LightProbe*> GetLightProbes() const;
		int GetLightProbesSize();
		DirectX::SimpleMath::Vector3 GetLightProbePosition(int lightProbeIndex);
		float* GetLightProbeCoefficient(int lightProbeIndex);
		// size 를 vector에서 가져가지 말고 꼭 이 함수를 써야 함 

		std::wstring SaveProbe1DirectionTexture(const int index, const std::wstring& direction); //return path Name
		void BakeAllLightProbeCoefficient();

		void MakeTetrahedron();

		int GetTetIndex(int& tetIndex, const DirectX::SimpleMath::Vector3& position, DirectX::SimpleMath::Vector4& weights);

		void GetCoefficientTetrahedronWeight(const DirectX::SimpleMath::Vector4& weights, int TetIndex, float* r, float* g, float* b/*(27)*/);

		/// Clear
		void ClearAllTetrahedron();
		void ClearAllLightProbe();

		/// SaveLoad
		void SaveLightProbes(const std::string& fileName);
		// position 정보만 담긴 probe 정보를 넘김
		bool LoadLightProbes(const std::string& fileName);

		/// IsUsed
		bool GetIsUsedLightProbe() const { return mIsUsedLightProbe; }
		void SetIsUsedLightProbe(bool val) { mIsUsedLightProbe = val; }

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
		// p는 오브젝트 pos, coords는 out 할 무게중심 좌표, vertices는 Tet의 vertices이므로 Tet을 보내면 될 거 같은디 
		void getBarycentriCoordinateForInnerTetrahedron(const DirectX::SimpleMath::Vector3& p,
			const Tetrahedron& tet, DirectX::SimpleMath::Vector4& coords);
		void getLightProbeInterpolationWeights(const std::vector<Tetrahedron*> tets, const DirectX::SimpleMath::Vector3& position, int tetIndex,
			DirectX::SimpleMath::Vector4& weights, int& steps);

		void lerpLightProbe(Tetrahedron* tet, const DirectX::SimpleMath::Vector4& weights, float* r, float* g, float* b/*(27)*/);

	private:
		std::shared_ptr<D3D11Device> mDevice;
		std::shared_ptr<D3D11ResourceManager> mResourceManager;

		bool mIsUsedLightProbe;

		int mCubeProbeIndex;
		int mLightProbeIndex; // 현재까지 만들어진 lightProbe 5개에서 1개 삭제돼도 5
		int mLightProbeSize; // 지금 가지고 있는 Lightprobe 개수 위의 경우 4
		std::vector<CubeProbe*> mCubeProbes;
		std::vector<LightProbe*> mLightProbes;
		std::unordered_map<int, int> mLightProbePair; // 게임 엔진에서 가지고 있는 라이트 프로브 인덱스를 실제 라이트 프로브 인덱스와 연결해주는 것
		std::vector<Tetrahedron*> mTetrahedrons;
	};
}

