#include "D3D11Buffer.h"

#include <d3dcompiler.h>

namespace fq::graphics
{
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