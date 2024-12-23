#pragma once

#include <memory>

#include "../FQCommon/FQCommonLoader.h"
#include "../FQCommon/IFQRenderResource.h"

namespace fq::graphics
{
	class D3D11Device;
	class D3D11VertexBuffer;
	class D3D11IndexBuffer;

	class MeshBase
	{
	public:
		MeshBase(const std::shared_ptr<D3D11Device>& device, const fq::common::Mesh& meshData);
		virtual ~MeshBase() = 0 {};

		void Bind(const std::shared_ptr<D3D11Device>& d3d11Device);
		void Bind(const std::shared_ptr<D3D11Device>& d3d11Device, std::shared_ptr<D3D11VertexBuffer> uvBuffer);
		void Draw(const std::shared_ptr<D3D11Device>& d3d11Device, size_t subsetIndex);
		void DrawInstanced(const std::shared_ptr<D3D11Device>& d3d11Device, size_t subsetIndex, size_t instanceCount);

		inline const fq::common::Mesh& GetMeshData() const;

		std::shared_ptr<D3D11VertexBuffer> GetSharedVertexBuffer() const { return mVertexBuffer; }
		std::shared_ptr<D3D11IndexBuffer> GetSharedIndexBuffer() const { return mIndexBuffer; }

	protected:
		fq::common::Mesh mMeshData;
		std::shared_ptr<D3D11VertexBuffer> mVertexBuffer;
		std::shared_ptr<D3D11IndexBuffer> mIndexBuffer;
	};

	inline const fq::common::Mesh& MeshBase::GetMeshData() const
	{
		return mMeshData;
	}

	class StaticMesh : public IStaticMesh, public MeshBase
	{
	public:
		StaticMesh(std::shared_ptr<D3D11Device> device, const fq::common::Mesh& meshData);
		virtual ~StaticMesh() = default;

		virtual void* GetVertexBuffer() override;
		virtual void* GetIndexBuffer() override;

		virtual const fq::common::Mesh& GetMeshData() const override { return mMeshData; }

		virtual EStaticMeshType GetStaticMeshType() const override { return mStaticMeshType; }

		virtual UINT GetVertexSize() const override;

	private:
		std::shared_ptr<D3D11Device> mDevice;
		EStaticMeshType mStaticMeshType;
	};

	class SkinnedMesh : public ISkinnedMesh, public MeshBase
	{
	public:
		SkinnedMesh(std::shared_ptr<D3D11Device> device, const fq::common::Mesh& meshData);
		virtual ~SkinnedMesh() = default;

		virtual void* GetVertexBuffer() override;
		virtual void* GetIndexBuffer() override;

		virtual const fq::common::Mesh& GetMeshData() const override { return mMeshData; }

	private:
		std::shared_ptr<D3D11Device> mDevice;
	};

	class TerrainMesh : public MeshBase
	{
	public:
		TerrainMesh(const std::shared_ptr<D3D11Device>& device, const fq::common::Mesh& meshData);
		virtual ~TerrainMesh() = default;

		void BindForPicking(const std::shared_ptr<D3D11Device>& d3d11Device);

	private:
		std::shared_ptr<D3D11IndexBuffer> mTriIndexBuffer;
	};
}