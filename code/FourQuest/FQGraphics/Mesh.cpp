#include "Mesh.h"
#include "D3D11Buffer.h"
#include "D3D11Device.h"

namespace fq::graphics
{
	MeshBase::MeshBase(const std::shared_ptr<D3D11Device>& device, const fq::common::Mesh& meshData)
		: mMeshData(meshData)
	{
		mIndexBuffer = std::make_shared<D3D11IndexBuffer>(device, meshData.Indices);
	}
	void MeshBase::Bind(const std::shared_ptr<D3D11Device>& d3d11Device)
	{
		mVertexBuffer->Bind(d3d11Device);
		mIndexBuffer->Bind(d3d11Device);
	}
	void MeshBase::Draw(const std::shared_ptr<D3D11Device>& d3d11Device, size_t subsetIndex)
	{
		const fq::common::Mesh::Subset& subset = mMeshData.Subsets[subsetIndex];
		d3d11Device->GetDeviceContext()->DrawIndexed(subset.IndexCount, subset.IndexStart, subset.VertexStart);
	}

	StaticMesh::StaticMesh(const std::shared_ptr<D3D11Device>& device, const fq::common::Mesh& meshData)
		: MeshBase(device, meshData)
	{
		mVertexBuffer = std::make_shared<D3D11VertexBuffer>(device, meshData.Vertices);
	}

	SkinnedMesh::SkinnedMesh(const std::shared_ptr<D3D11Device>& device, const fq::common::Mesh& meshData)
		: MeshBase(device, meshData)
	{
		struct Vertex
		{
			DirectX::SimpleMath::Vector3 Pos;
			DirectX::SimpleMath::Vector3 Normal;
			DirectX::SimpleMath::Vector3 Tangent;
			DirectX::SimpleMath::Vector2 Tex;
			int BoneIndices[4] = { 0, };
			float BoneWeights[4] = { 0.f, };
		};

		std::vector<Vertex> vertices;
		vertices.reserve(meshData.Vertices.size());

		for (size_t i = 0; i < meshData.Vertices.size(); ++i)
		{
			Vertex vertex;

			vertex.Pos = meshData.Vertices[i].Pos;
			vertex.Normal = meshData.Vertices[i].Normal;
			vertex.Tangent = meshData.Vertices[i].Tangent;
			vertex.Tex = meshData.Vertices[i].Tex;

			if (meshData.BoneVertices.size() > i)
			{
				memcpy(vertex.BoneIndices, meshData.BoneVertices[i].BoneIndices, sizeof(vertex.BoneIndices));
				memcpy(vertex.BoneWeights, meshData.BoneVertices[i].BoneWeights, sizeof(vertex.BoneWeights));
			}

			vertices.push_back(std::move(vertex));
		}

		mVertexBuffer = std::make_shared<D3D11VertexBuffer>(device, vertices);
	}

	TerrainMesh::TerrainMesh(const std::shared_ptr<D3D11Device>& device, const fq::common::Mesh& meshData)
		:MeshBase(device, meshData)
	{
		struct Vertex
		{
			DirectX::SimpleMath::Vector3 Pos;
			DirectX::SimpleMath::Vector3 Normal;
			DirectX::SimpleMath::Vector3 Tangent;
			DirectX::SimpleMath::Vector2 Tex;
			DirectX::SimpleMath::Vector2 BoundsY;
		};

		std::vector<Vertex> vertices;
		vertices.reserve(meshData.Vertices.size());

		for (size_t i = 0; i < meshData.Vertices.size(); ++i)
		{
			Vertex vertex;

			vertex.Pos = meshData.Vertices[i].Pos;
			vertex.Normal = meshData.Vertices[i].Normal;
			vertex.Tangent = meshData.Vertices[i].Tangent;
			vertex.Tex = meshData.Vertices[i].Tex;
			vertex.BoundsY = meshData.BoundsYVertices[i].BoundsY;

			vertices.push_back(std::move(vertex));
		}

		mVertexBuffer = std::make_shared<D3D11VertexBuffer>(device, vertices);

		// Picking 용 인덱스 버퍼 따로 만들기 
		std::vector<unsigned int> terrainTriIndexBuffer;

		for (UINT i = 0; i < meshData.Indices.size(); i += 4)
		{
			terrainTriIndexBuffer.push_back(meshData.Indices[i + 0]);
			terrainTriIndexBuffer.push_back(meshData.Indices[i + 3]);
			terrainTriIndexBuffer.push_back(meshData.Indices[i + 2]);

			terrainTriIndexBuffer.push_back(meshData.Indices[i + 0]);
			terrainTriIndexBuffer.push_back(meshData.Indices[i + 1]);
			terrainTriIndexBuffer.push_back(meshData.Indices[i + 3]);
		}

		mTriIndexBuffer = std::make_shared<D3D11IndexBuffer>(device, terrainTriIndexBuffer);
	}

	void TerrainMesh::BindForPicking(const std::shared_ptr<D3D11Device>& d3d11Device)
	{
		mVertexBuffer->Bind(d3d11Device);
		mTriIndexBuffer->Bind(d3d11Device);
	}
}