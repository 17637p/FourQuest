#include "D3D11Buffer.h"
#include "ConstantBufferStructure.h"
#include "Define.h"

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

	D3D11ConstantBufferNT::D3D11ConstantBufferNT(const std::shared_ptr<D3D11Device>& device, ED3D11ConstantBufferNT type)
		: ResourceBase(ResourceType::ConstantBufferNT)
		, mSize(0u)
	{
		switch (type)
		{
		case fq::graphics::ED3D11ConstantBufferNT::cbModelTransform:
			mSize = sizeof(cbModelTransform);
			create(device, mSize);
			break;
		case fq::graphics::ED3D11ConstantBufferNT::cbSceneTransform:
			mSize = sizeof(cbSceneTransform);
			create(device, mSize);
			break;
		case fq::graphics::ED3D11ConstantBufferNT::cbBoneTransform:
			mSize = sizeof(cbBoneTransform);
			create(device, mSize);
			break;
		case fq::graphics::ED3D11ConstantBufferNT::cbShadowTransform:
			mSize = sizeof(cbShadowTransform);
			create(device, mSize);
			break;
		case fq::graphics::ED3D11ConstantBufferNT::cbShadowTransformCascaseEnd:
			mSize = sizeof(cbShadowTransformCascaseEnd);
			create(device, mSize);
			break;
		case fq::graphics::ED3D11ConstantBufferNT::cbAlpha:
			mSize = sizeof(cbAlpha);
			create(device, mSize);
			break;
		case fq::graphics::ED3D11ConstantBufferNT::ViewRotationProjectionMatrix:
			mSize = sizeof(ViewRotationProjectionMatrix);
			create(device, mSize);
			break;
		case fq::graphics::ED3D11ConstantBufferNT::cbModelTexture:
			mSize = sizeof(cbModelTexture);
			create(device, mSize);
			break;
		case fq::graphics::ED3D11ConstantBufferNT::cbLight:
			mSize = sizeof(cbLight);
			create(device, mSize);
			break;
		default:
			break;
		}
	}
	D3D11ConstantBufferNT::D3D11ConstantBufferNT(const std::shared_ptr<D3D11Device>& device, ED3D11ConstantBufferNT type, size_t size)
		: ResourceBase(ResourceType::ConstantBufferNT)
		, mSize(size)
	{
		create(device, size);
	}

	D3D11ConstantBufferNT::D3D11ConstantBufferNT(const std::shared_ptr<D3D11Device>& device, ED3D11ConstantBufferNT type, CD3D11_BUFFER_DESC desc)
		: ResourceBase(ResourceType::ConstantBufferNT)
	{
		mSize = desc.ByteWidth;
		HR(device->GetDevice()->CreateBuffer(&desc, nullptr, mBuffer.GetAddressOf()));
	}

	void D3D11ConstantBufferNT::create(const std::shared_ptr<D3D11Device>& device, size_t size)
	{
		D3D11_BUFFER_DESC desc = {};
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.MiscFlags = 0u;
		desc.ByteWidth = size;
		desc.StructureByteStride = 0u;
		HR(device->GetDevice()->CreateBuffer(&desc, nullptr, mBuffer.GetAddressOf()));
	}

	std::string D3D11ConstantBufferNT::GenerateRID(const ED3D11ConstantBufferNT type)
	{
		return typeid(D3D11ConstantBufferNT).name() + std::to_string(static_cast<int>(type));
	}

	void D3D11ConstantBufferNT::Bind(const std::shared_ptr<D3D11Device>& d3d11Device, const ED3D11ShaderType eShaderType, const UINT startSlot)
	{
		switch (eShaderType)
		{
		case ED3D11ShaderType::VertexShader:
		{
			d3d11Device->GetDeviceContext()->VSSetConstantBuffers(startSlot, 1, mBuffer.GetAddressOf());
			break;
		}
		case ED3D11ShaderType::GeometryShader:
		{
			d3d11Device->GetDeviceContext()->GSSetConstantBuffers(startSlot, 1, mBuffer.GetAddressOf());
			break;
		}
		case ED3D11ShaderType::Pixelshader:
		{
			d3d11Device->GetDeviceContext()->PSSetConstantBuffers(startSlot, 1, mBuffer.GetAddressOf());
			break;
		}
		default:
			break;
		}
	}

	void D3D11ConstantBufferNT::Update(const std::shared_ptr<D3D11Device>& device, const void* data, size_t size)
	{
		assert(mSize == size);
		device->GetDeviceContext()->UpdateSubresource(mBuffer.Get(), 0, 0, data, 0, 0);
	}
}