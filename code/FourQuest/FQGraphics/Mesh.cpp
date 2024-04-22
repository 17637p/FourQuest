#include "Mesh.h"
#include "D3D11Buffer.h"
#include "D3D11Device.h"

namespace fq::graphics
{
	MeshBase::MeshBase(const std::shared_ptr<D3D11Device>& device, const fq::common::Mesh& meshData)
		: mMeshData(meshData)
	{
		mVertexBuffer = std::make_shared<D3D11VertexBuffer>(device, meshData.Vertices);
		mIndexBuffer = std::make_shared<D3D11IndexBuffer>(device, meshData.Indices);
	}
	void MeshBase::Draw(const std::shared_ptr<D3D11Device>& d3d11Device, size_t subsetIndex)
	{
		const fq::common::Mesh::Subset& subset = mMeshData.Subsets[subsetIndex];
		d3d11Device->GetDeviceContext()->DrawIndexed(subset.IndexCount, subset.IndexStart, subset.VertexStart);
	}

	StaticMesh::StaticMesh(const std::shared_ptr<D3D11Device>& device, const fq::common::Mesh& meshData)
		: MeshBase(device, meshData)
	{
	}
	void StaticMesh::Bind(const std::shared_ptr<D3D11Device>& d3d11Device)
	{
		mVertexBuffer->Bind(d3d11Device);
		mIndexBuffer->Bind(d3d11Device);
	}

	SkinnedMesh::SkinnedMesh(const std::shared_ptr<D3D11Device>& device, const fq::common::Mesh& meshData)
		: MeshBase(device, meshData)
	{
		mBoneVertexBuffer = std::make_shared<D3D11VertexBuffer>(device, meshData.BoneVertices);
	}
	void SkinnedMesh::Bind(const std::shared_ptr<D3D11Device>& d3d11Device)
	{
		std::vector<std::shared_ptr<D3D11VertexBuffer>> buffers = { mVertexBuffer, mBoneVertexBuffer };
		D3D11VertexBuffer::Bind(d3d11Device, buffers);
		mIndexBuffer->Bind(d3d11Device);
	}
}