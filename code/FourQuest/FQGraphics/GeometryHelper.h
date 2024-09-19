#pragma once

#include <vector>
#include <directxtk\SimpleMath.h>

namespace fq::graphics
{
	class GeometryHelper
	{
	public:
		struct Vertex
		{
			Vertex() = default;
			Vertex(const Vertex&) = default;
			Vertex& operator=(const Vertex&) = default;
			Vertex(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& normal, const DirectX::SimpleMath::Vector3& tangent, const DirectX::SimpleMath::Vector2& texture);
			Vertex(float px, float py, float pz, float nx, float ny, float nz, float tanx, float tany, float tanz, float tx, float ty);

			DirectX::SimpleMath::Vector3 Position;
			DirectX::SimpleMath::Vector3 Normal;
			DirectX::SimpleMath::Vector3 TangentU;
			DirectX::SimpleMath::Vector2 TexC;
		};

		struct MeshData
		{
			std::vector<Vertex> Vertices;
			std::vector<unsigned int> Indices;
		};

	public:
		static void CreateBox(float width, float height, float depth, MeshData* outMeshData);
		static void CreateSphere(float radius, UINT sliceCount, UINT stackCount, MeshData* outMeshData);
		static void CreateGeosphere(float radius, UINT numSubdivisions, MeshData* outMeshData);
		static void CreateCylinder(float bottomRadius, float topRadius, float height, UINT sliceCount, UINT stackCount, MeshData* outMeshData);
		static void CreateGrid(float width, float depth, UINT m, UINT n, MeshData* outMeshData);
		static void CreateFullscreenQuad(MeshData* outMeshData);

	private:
		static void subdivide(MeshData* meshData);
		static void buildCylinderTopCap(float bottomRadius, float topRadius, float height, UINT sliceCount, UINT stackCount, MeshData* outMeshData);
		static void buildCylinderBottomCap(float bottomRadius, float topRadius, float height, UINT sliceCount, UINT stackCount, MeshData* outMeshData);
	};
}