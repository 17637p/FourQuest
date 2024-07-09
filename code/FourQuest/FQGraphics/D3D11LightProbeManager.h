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
		int index; // �����ߴٰ� �ҷ��� �� �ʿ�
		DirectX::SimpleMath::Vector3 position;

		float coefficient[27]; // 0~8 Red, 9~17 Green, 18~26 Blue
	};

	struct Tetrahedron
	{
		LightProbe* probes[4]; // vertice ������ �ʿ���!
		int neighbors[4];

		DirectX::SimpleMath::Matrix matrix; // 4x3�� �´�
	};

	// ��ġ�ϱ�
	// ť��� �����ϱ�
	// bakeAllLightProbe
	// ���ü �����
	// GetTetIndex - ������Ʈ����
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
		// size �� vector���� �������� ���� �� �� �Լ��� ��� �� 

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
		// position ������ ��� probe ������ �ѱ�
		bool LoadLightProbes(const std::string& fileName);

		/// IsUsed
		bool GetIsUsedLightProbe() const { return mIsUsedLightProbe; }
		void SetIsUsedLightProbe(bool val) { mIsUsedLightProbe = val; }

		//void GetBarycentricCoordinates(const std::vector<DirectX::SimpleMath::Vector3> vertices, const std::vector<DirectX::SimpleMath::Vector3>& hullRays,
		//	const DirectX::SimpleMath::Vector3& p, const Tetrahedron& tet, DirectX::SimpleMath::Vector4& coords, float& t);
		/// Outer Cells�� ���õ� �Ŷ� �ϴ� ���� 
		/// �׳� ����ó���� �ƹ��͵� ���ϸ� �ȵǳ�?
		//void GetBarycentricCoordinatesForOuterCell(const std::vector<DirectX::SimpleMath::Vector3>& vertices,
		//	const std::vector<DirectX::SimpleMath::Vector3>& hullRays,
		//	const DirectX::SimpleMath::Vector3& p,
		//	const Tetrahedron& tet,
		//	DirectX::SimpleMath::Vector4& coord,
		//	float& t);
		// 
		//void CalculateOuterCellsMatrices(LightProbeCloudData& data, int innerTetrahedraCount);

	private:
		// p�� ������Ʈ pos, coords�� out �� �����߽� ��ǥ, vertices�� Tet�� vertices�̹Ƿ� Tet�� ������ �� �� ������ 
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
		int mLightProbeIndex; // ������� ������� lightProbe 5������ 1�� �����ŵ� 5
		int mLightProbeSize; // ���� ������ �ִ� Lightprobe ���� ���� ��� 4
		std::vector<CubeProbe*> mCubeProbes;
		std::vector<LightProbe*> mLightProbes;
		std::unordered_map<int, int> mLightProbePair; // ���� �������� ������ �ִ� ����Ʈ ���κ� �ε����� ���� ����Ʈ ���κ� �ε����� �������ִ� ��
		std::vector<Tetrahedron*> mTetrahedrons;
	};
}

