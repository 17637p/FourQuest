#include "D3D11Buffer.h"

#include <d3dcompiler.h>

namespace fq::graphics
{
	D3D11VertexBuffer::D3D11VertexBuffer(const std::shared_ptr<D3D11Device>& device, unsigned int size, unsigned int count, void* ptr, bool bUseCPUWrite)
		: mStride(size)
		, mOffset(0u)
		, mbUseCPUWrite(false)
	{
		D3D11_BUFFER_DESC bd = {};
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.Usage = bUseCPUWrite ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_IMMUTABLE;
		bd.CPUAccessFlags = bUseCPUWrite ? D3D11_CPU_ACCESS_WRITE : 0u;
		bd.MiscFlags = 0u;
		bd.ByteWidth = (UINT)(size * count);
		bd.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA sd = {};
		sd.pSysMem = ptr;

		device->GetDevice().Get()->CreateBuffer(&bd, &sd, mVertexBuffer.GetAddressOf());
	}

	D3D11VertexBuffer::D3D11VertexBuffer(const std::shared_ptr<D3D11Device>& device, UINT size, UINT stride, UINT offset)
		: mStride(stride)
		, mOffset(offset)
		, mbUseCPUWrite(true)
	{
		D3D11_BUFFER_DESC bd = {};
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.Usage = D3D11_USAGE_DYNAMIC;
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bd.MiscFlags = 0u;
		bd.ByteWidth = size;
		bd.StructureByteStride = 0;

		device->GetDevice().Get()->CreateBuffer(&bd, nullptr, mVertexBuffer.GetAddressOf());
	}

	void D3D11VertexBuffer::Bind(const std::shared_ptr<D3D11Device>& device, const std::vector<std::shared_ptr<D3D11VertexBuffer>>& buffers, UINT startSlot)
	{
		std::vector<ID3D11Buffer*> VBs;
		VBs.reserve(buffers.size());

		std::vector<UINT> strides;
		strides.reserve(buffers.size());

		std::vector<UINT> offsets;
		offsets.reserve(buffers.size());

		for (const std::shared_ptr<D3D11VertexBuffer>& buffer : buffers)
		{
			VBs.push_back(buffer->mVertexBuffer.Get());
			strides.push_back(buffer->mStride);
			offsets.push_back(buffer->mOffset);
		}

		device->GetDeviceContext()->IASetVertexBuffers(
			startSlot,
			buffers.size(),
			VBs.data(),
			strides.data(),
			offsets.data()
		);
	}

	D3D11VertexBuffer D3D11VertexBuffer::CreateFullScreenVertexBuffer(const std::shared_ptr<D3D11Device>& device)
	{
		std::vector<DirectX::SimpleMath::Vector2> positions =
		{
			{ -1, 1 },
			{ 1, 1 },
			{ -1, -1 },
			{ 1, -1 }
		};

		return D3D11VertexBuffer(device, positions);
	}

	void D3D11VertexBuffer::Bind(const std::shared_ptr<D3D11Device>& device, UINT startSlot)
	{
		device->GetDeviceContext()->IASetVertexBuffers(
			startSlot,
			1,
			mVertexBuffer.GetAddressOf(),
			&mStride,
			&mOffset
		);
	}

	D3D11IndexBuffer::D3D11IndexBuffer(const std::shared_ptr<D3D11Device>& device, const std::vector<unsigned int>& indices)
		: mCount(indices.size())
		, mFormat(DXGI_FORMAT_R32_UINT)
	{
		D3D11_BUFFER_DESC bd = {};
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.Usage = D3D11_USAGE_IMMUTABLE;
		bd.CPUAccessFlags = 0u;
		bd.MiscFlags = 0u;
		bd.ByteWidth = (UINT)(sizeof(unsigned int) * indices.size());
		bd.StructureByteStride = 0u;

		D3D11_SUBRESOURCE_DATA sd = {};
		sd.pSysMem = indices.data();

		device->GetDevice().Get()->CreateBuffer(&bd, &sd, mIndexBuffer.GetAddressOf());
	}

	void D3D11IndexBuffer::Bind(const std::shared_ptr<D3D11Device>& device)
	{
		device->GetDeviceContext()->IASetIndexBuffer(
			mIndexBuffer.Get(),
			mFormat,
			0
		);
	}
}