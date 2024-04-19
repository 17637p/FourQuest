#pragma once

#include <memory>

#include "../FQCommon/FQCommonLoader.h"

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

	class StaticMesh : public MeshBase
	{
	public:
		StaticMesh(const std::shared_ptr<D3D11Device>& device, const fq::common::Mesh& meshData);
		virtual ~StaticMesh() = default;

		void Bind(const std::shared_ptr<D3D11Device>& d3d11Device);
	};

	class SkinnedMesh : public MeshBase
	{
	public:
		SkinnedMesh(const std::shared_ptr<D3D11Device>& device, const fq::common::Mesh& meshData);
		virtual ~SkinnedMesh() = default;

		void Bind(const std::shared_ptr<D3D11Device>& d3d11Device);

	private:
		std::shared_ptr<D3D11VertexBuffer> mBoneVertexBuffer;
	};
}