#include "Mesh.h"
#include "D3D11Buffer.h"
#include "D3D11Device.h"

namespace fq::graphics
{
	MeshBase::MeshBase(const std::shared_ptr<D3D11Device>& device, const fq::common::Mesh& meshData)
		: mMeshData(meshData)
	{
		// // 어떤 인자가 존재하는지 체크해서 동적으로 상수버퍼 생성
		// unsigned int bufferSize = 0;
		// std::vector<unsigned int> offsets;
		// 
		// if (meshData.Vertices.empty())
		// {
		// 	bufferSize += sizeof(meshData.Vertices[0]);
		// }
		// if (meshData.BoneVertices.empty())
		// {
		// 	bufferSize += sizeof(meshData.BoneVertices[0]);
		// }
		// if (meshData.Tex1.empty())
		// {
		// 	bufferSize += sizeof(meshData.Tex1[0]);
		// }
		// for (const auto& [key, dynamicData] : meshData.DynamicInfos)
		// {
		// 	bufferSize += dynamicData.Size;
		// }
		// 
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

	void MeshBase::DrawInstanced(const std::shared_ptr<D3D11Device>& d3d11Device, size_t subsetIndex, size_t instanceCount)
	{
		const fq::common::Mesh::Subset& subset = mMeshData.Subsets[subsetIndex];
		d3d11Device->GetDeviceContext()->DrawIndexedInstanced(subset.IndexCount, instanceCount, subset.IndexStart, subset.VertexStart, 0);
	}

	StaticMesh::StaticMesh(std::shared_ptr<D3D11Device> device, const fq::common::Mesh& meshData)
		: MeshBase(device, meshData)
		, mDevice(device)
		, mStaticMeshType(EStaticMeshType::Default)
	{
		mIndexBuffer = std::make_shared<D3D11IndexBuffer>(device, meshData.Indices);

		auto find = meshData.DynamicInfos.find("Color0");

		if (!meshData.Tex1.empty())
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
			mStaticMeshType = EStaticMeshType::Static;
		}
		else if (find != meshData.DynamicInfos.end())
		{
			struct Vertex
			{
				DirectX::SimpleMath::Vector3 Pos;
				DirectX::SimpleMath::Vector3 Normal;
				DirectX::SimpleMath::Vector3 Tangent;
				DirectX::SimpleMath::Vector2 Tex;
				DirectX::SimpleMath::Vector4 Color;
			};

			std::vector<Vertex> vertices(meshData.Vertices.size());

			const DirectX::SimpleMath::Vector4* colorPtr = reinterpret_cast<const DirectX::SimpleMath::Vector4*>(find->second.Data.data());

			for (size_t i = 0; i < vertices.size(); ++i)
			{
				vertices[i].Pos = meshData.Vertices[i].Pos;
				vertices[i].Normal = meshData.Vertices[i].Normal;
				vertices[i].Tangent = meshData.Vertices[i].Tangent;
				vertices[i].Tex = meshData.Vertices[i].Tex;

				if (find->second.Count > i)
				{
					vertices[i].Color = colorPtr[i];
				}
			}

			mVertexBuffer = std::make_shared<D3D11VertexBuffer>(mDevice, vertices);
			mStaticMeshType = EStaticMeshType::VertexColor;
		}
		else
		{
			
			mVertexBuffer = std::make_shared<D3D11VertexBuffer>(mDevice, meshData.Vertices);
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

	SkinnedMesh::SkinnedMesh(std::shared_ptr<D3D11Device> device, const fq::common::Mesh& meshData)
		: MeshBase(device, meshData)
		, mDevice(device)
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