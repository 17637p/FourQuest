#include "D3D11Buffer.h"

#include <d3dcompiler.h>

namespace fq::graphics
{
	D3D11InputLayout::D3D11InputLayout(const std::shared_ptr<D3D11Device>& device, ID3DBlob* VSBytecode)
	{
		ID3D11ShaderReflection* pReflector = nullptr;

		D3DReflect(VSBytecode->GetBufferPointer(),
			VSBytecode->GetBufferSize(),
			IID_ID3D11ShaderReflection,
			(void**)&pReflector);

		D3D11_SHADER_DESC shaderDesc;
		pReflector->GetDesc(&shaderDesc);

		std::vector<D3D11_INPUT_ELEMENT_DESC> inputLayoutDesc;
		for (unsigned i = 0; i < shaderDesc.InputParameters; ++i)
		{
			D3D11_SIGNATURE_PARAMETER_DESC paramDesc;
			pReflector->GetInputParameterDesc(i, &paramDesc);

			D3D11_INPUT_ELEMENT_DESC elementDesc;
			elementDesc.SemanticName = paramDesc.SemanticName;
			elementDesc.SemanticIndex = paramDesc.SemanticIndex;
			elementDesc.InputSlot = 0;
			elementDesc.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
			elementDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			elementDesc.InstanceDataStepRate = 0;

			if (paramDesc.Mask == 1)
			{
				if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) elementDesc.Format = DXGI_FORMAT_R32_UINT;
				else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) elementDesc.Format = DXGI_FORMAT_R32_SINT;
				else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) elementDesc.Format = DXGI_FORMAT_R32_FLOAT;
			}
			else if (paramDesc.Mask <= 3)
			{
				if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) elementDesc.Format = DXGI_FORMAT_R32G32_UINT;
				else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) elementDesc.Format = DXGI_FORMAT_R32G32_SINT;
				else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) elementDesc.Format = DXGI_FORMAT_R32G32_FLOAT;
			}
			else if (paramDesc.Mask <= 7)
			{
				if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) elementDesc.Format = DXGI_FORMAT_R32G32B32_UINT;
				else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) elementDesc.Format = DXGI_FORMAT_R32G32B32_SINT;
				else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) elementDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
			}
			else if (paramDesc.Mask <= 15)
			{
				if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) elementDesc.Format = DXGI_FORMAT_R32G32B32A32_UINT;
				else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) elementDesc.Format = DXGI_FORMAT_R32G32B32A32_SINT;
				else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) elementDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			}

			inputLayoutDesc.push_back(elementDesc);
		}
	
		device->GetDevice()->CreateInputLayout(&inputLayoutDesc[0],
			(UINT)inputLayoutDesc.size(), 
			VSBytecode->GetBufferPointer(), 
			VSBytecode->GetBufferSize(), 
			mInputLayout.GetAddressOf());
	}

	void D3D11InputLayout::Bind(const std::shared_ptr<D3D11Device>& device)
	{
		device->GetDeviceContext()->IASetInputLayout(mInputLayout.Get());
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