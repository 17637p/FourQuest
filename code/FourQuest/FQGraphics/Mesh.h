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
		MeshBase() = default;
		MeshBase(const std::shared_ptr<D3D11Device>& device, const fq::common::Mesh& meshData);
		virtual ~MeshBase() = 0 {};

		void Bind(const std::shared_ptr<D3D11Device>& d3d11Device);
		void Bind(const std::shared_ptr<D3D11Device>& d3d11Device, std::shared_ptr<D3D11VertexBuffer> uvBuffer);
		void Draw(const std::shared_ptr<D3D11Device>& d3d11Device, size_t subsetIndex);

		inline const fq::common::Mesh& GetMeshData() const;

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
		StaticMesh(std::shared_ptr<D3D11Device> device);
		StaticMesh(std::shared_ptr<D3D11Device> device, const fq::common::Mesh& meshData);
		virtual ~StaticMesh() = default;

		virtual void Create(const fq::common::Mesh& meshData);

		virtual void* GetVertexBuffer() override;
		virtual void* GetIndexBuffer() override;

		virtual const fq::common::Mesh& GetMeshData() const override { return mMeshData; }

		std::shared_ptr<D3D11VertexBuffer> GetUV1Buffer() { return mUV1VertexBuffer; }

	private:
		std::shared_ptr<D3D11Device> mDevice;
		std::shared_ptr<D3D11VertexBuffer> mUV1VertexBuffer;
	};

	class SkinnedMesh : public ISkinnedMesh, public MeshBase
	{
	public:
		SkinnedMesh(std::shared_ptr<D3D11Device> device);
		SkinnedMesh(std::shared_ptr<D3D11Device> device, const fq::common::Mesh& meshData);
		virtual ~SkinnedMesh() = default;

		virtual void Create(const fq::common::Mesh& meshData);

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