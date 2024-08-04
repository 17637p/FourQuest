#include "Mesh.h"
#include "D3D11Buffer.h"
#include "D3D11Device.h"

namespace fq::graphics
{
	MeshBase::MeshBase(const std::shared_ptr<D3D11Device>& device, const fq::common::Mesh& meshData)
		: mMeshData(meshData)
	{
		// � ���ڰ� �����ϴ��� üũ�ؼ� �������� ������� ����
		unsigned int bufferSize = 0;
		std::vector<unsigned int> offsets;

		if (meshData.Vertices.empty())
		{
			bufferSize += sizeof(meshData.Vertices[0]);
		}
		if (meshData.BoneVertices.empty())
		{
			bufferSize += sizeof(meshData.BoneVertices[0]);
		}
		if (meshData.Tex1.empty())
		{
			bufferSize += sizeof(meshData.Tex1[0]);
		}
		for (const auto& [key, dynamicData] : meshData.DynamicInfos)
		{
			bufferSize += dynamicData.Size;
		}



		mIndexBuffer = std::make_shared<D3D11IndexBuffer>(device, meshData.Indices);
	}
	void MeshBase::Bind(const std::shared_ptr<D3D11Device>& d3d11Device)
	{
		mVertexBuffer->Bind(d3d11Device);
		mIndexBuffer->Bind(d3d11Device);
	}
	void MeshBase::Bind(const std::shared_ptr<D3D11Device>& d3d11Device, std::shared_ptr<D3D11VertexBuffer> uvBuffer)
	{
		D3D11VertexBuffer::Bind(d3d11Device, { mVertexBuffer, uvBuffer });
		mIndexBuffer->Bind(d3d11Device);
	}
	void MeshBase::Draw(const std::shared_ptr<D3D11Device>& d3d11Device, size_t subsetIndex)
	{
		const fq::common::Mesh::Subset& subset = mMeshData.Subsets[subsetIndex];
		d3d11Device->GetDeviceContext()->DrawIndexed(subset.IndexCount, subset.IndexStart, subset.VertexStart);
	}

	StaticMesh::StaticMesh(std::shared_ptr<D3D11Device> device, const fq::common::Mesh& meshData)
		: MeshBase()
		, mDevice(device)
	{
		if (meshData.Tex1.empty())
		{
			mVertexBuffer = std::make_shared<D3D11VertexBuffer>(mDevice, meshData.Vertices);
		}
		else
		{
			struct Vertex
			{
				DirectX::SimpleMath::Vector3 Pos;
				DirectX::SimpleMath::Vector3 Normal;
				DirectX::SimpleMath::Vector3 Tangent;
				DirectX::SimpleMath::Vector2 Tex;
				DirectX::SimpleMath::Vector2 Tex1;
			};

			std::vector<Vertex> vertices(meshData.Vertices.size());

			for (size_t i = 0; i < vertices.size(); ++i)
			{
				vertices[i].Pos = meshData.Vertices[i].Pos;
				vertices[i].Normal = meshData.Vertices[i].Normal;
				vertices[i].Tangent = meshData.Vertices[i].Tangent;
				vertices[i].Tex = meshData.Vertices[i].Tex;
				vertices[i].Tex1 = meshData.Tex1[i];
			}

			mVertexBuffer = std::make_shared<D3D11VertexBuffer>(mDevice, vertices);
		}
	}
	void* StaticMesh::GetVertexBuffer()
	{
		auto vertexBuffer = mVertexBuffer->GetVertexBuffer();

		if (vertexBuffer == nullptr)
		{
			return nullptr;
		}

		return vertexBuffer.Get();
	}
	void* StaticMesh::GetIndexBuffer()
	{
		auto indexBuffer = mIndexBuffer->GetIndexBuffer();

		if (indexBuffer == nullptr)
		{
			return nullptr;
		}

		return indexBuffer.Get();
	}

	SkinnedMesh::SkinnedMesh(std::shared_ptr<D3D11Device> device)
		: MeshBase()
		, mDevice(device)
	{
	}

	SkinnedMesh::SkinnedMesh(std::shared_ptr<D3D11Device> device, const fq::common::Mesh& meshData)
		: MeshBase()
		, mDevice(device)
	{
		Create(meshData);
	}

	void SkinnedMesh::Create(const fq::common::Mesh& meshData)
	{
		mMeshData = meshData;

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

		mIndexBuffer = std::make_shared<D3D11IndexBuffer>(mDevice, meshData.Indices);
		mVertexBuffer = std::make_shared<D3D11VertexBuffer>(mDevice, vertices);
	}
	void* SkinnedMesh::GetVertexBuffer()
	{
		auto vertexBuffer = mVertexBuffer->GetVertexBuffer();

		if (vertexBuffer == nullptr)
		{
			return nullptr;
		}

		return vertexBuffer.Get();
	}
	void* SkinnedMesh::GetIndexBuffer()
	{
		auto indexBuffer = mIndexBuffer->GetIndexBuffer();

		if (indexBuffer == nullptr)
		{
			return nullptr;
		}

		return indexBuffer.Get();
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

		// Picking �� �ε��� ���� ���� ����� 
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