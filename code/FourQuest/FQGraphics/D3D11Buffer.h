#pragma once

#include "ResourceBase.h"

#include <memory>
#include <string>
#include <wrl.h>
#include <string>
#include <vector>
#include <d3d11.h>
#include <directxtk/BufferHelpers.h>

#include "D3D11Device.h"
#include "D3D11ResourceType.h"

namespace fq::graphics
{
	class D3D11Device;

	/*=============================================================================
		Input Layout
	=============================================================================*/
	class D3D11InputLayout
	{
	public:
		D3D11InputLayout(const std::shared_ptr<D3D11Device>& device, ID3DBlob* VSBytecode);

		void Bind(const std::shared_ptr<D3D11Device>& device);

	private:
		Microsoft::WRL::ComPtr<ID3D11InputLayout> mInputLayout;
	};

	/*=============================================================================
		Vertex Buffer
	=============================================================================*/
	class D3D11VertexBuffer
	{
	public:
		template<typename T>
		D3D11VertexBuffer(const std::shared_ptr<D3D11Device>& device, const std::vector<T>& vertices);

		void Bind(const std::shared_ptr<D3D11Device>& device, UINT startSlot = 0);

	private:
		Microsoft::WRL::ComPtr<ID3D11Buffer> mVertexBuffer;
		UINT mStride;
		UINT mOffset;
	};

	template<typename T>
	D3D11VertexBuffer::D3D11VertexBuffer(const std::shared_ptr<D3D11Device>& device, const std::vector<T>& vertices)
		: mStride(sizeof(T))
		, mOffset(0)
	{
		D3D11_BUFFER_DESC bd = {};
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.Usage = D3D11_USAGE_IMMUTABLE;
		bd.CPUAccessFlags = 0u;
		bd.MiscFlags = 0u;
		bd.ByteWidth = (UINT)(sizeof(T) * vertices.size());
		bd.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA sd = {};
		sd.pSysMem = vertices.data();

		device->GetDevice().Get()->CreateBuffer(&bd, &sd, mVertexBuffer.GetAddressOf());
	}

	/*=============================================================================
		Index Buffer
	=============================================================================*/
	class D3D11IndexBuffer
	{
	public:
		D3D11IndexBuffer(const std::shared_ptr<D3D11Device>& device, const std::vector<unsigned int>& indices);

		void Bind(const std::shared_ptr<D3D11Device>& device);

	private:
		Microsoft::WRL::ComPtr<ID3D11Buffer> mIndexBuffer;
		DXGI_FORMAT mFormat;
		std::string mTag;
		size_t mCount;
	};

	/*=============================================================================
		Constant Buffer
	=============================================================================*/
	template<typename ConstantType>
	class D3D11ConstantBuffer : public ResourceBase
	{
	public:
		D3D11ConstantBuffer(const std::shared_ptr<D3D11Device>& device, 
			const ED3D11ConstantBuffer eConstantBuffer);

		static std::string GenerateRID(const ED3D11ConstantBuffer eConstantBuffer);

		void Update(const std::shared_ptr<D3D11Device>& d3d11Device, const ConstantType& value);

		void Bind(const std::shared_ptr<D3D11Device>& d3d11Device, const ED3D11ShaderType eShaderType, const UINT startSlot = 0);

	private:
		std::shared_ptr<DirectX::ConstantBuffer<ConstantType>> mConstantBuffer;
	};

	template<typename ConstantType>
	std::string fq::graphics::D3D11ConstantBuffer<ConstantType>::GenerateRID(const ED3D11ConstantBuffer eConstantBuffer)
	{
		return typeid(D3D11ConstantBuffer<ConstantType>).name() + std::to_string(static_cast<int>(eConstantBuffer));
	}

	template<typename ConstantType>
	void fq::graphics::D3D11ConstantBuffer<ConstantType>::Update(const std::shared_ptr<D3D11Device>& d3d11Device, const ConstantType& value)
	{
		mConstantBuffer->SetData(d3d11Device->GetDeviceContext().Get(), value);
	}

	template<typename ConstantType>
	void fq::graphics::D3D11ConstantBuffer<ConstantType>::Bind(const std::shared_ptr<D3D11Device>& d3d11Device, const ED3D11ShaderType eShaderType, const UINT startSlot /*0*/)
	{
		ID3D11Buffer* constantBuffer = mConstantBuffer->GetBuffer();

		switch (eShaderType)
		{
			case ED3D11ShaderType::VertexShader:
			{
				d3d11Device->GetDeviceContext()->VSSetConstantBuffers(startSlot, 1, &constantBuffer);
				break;
			}
			case ED3D11ShaderType::GeometryShader:
			{
				d3d11Device->GetDeviceContext()->GSSetConstantBuffers(startSlot, 1, &constantBuffer);
				break;
			}
			case ED3D11ShaderType::Pixelshader:
			{
				d3d11Device->GetDeviceContext()->PSSetConstantBuffers(startSlot, 1, &constantBuffer);
				break;
			}
			default:
				break;
		}
	}

	template<typename ConstantType>
	fq::graphics::D3D11ConstantBuffer<ConstantType>::D3D11ConstantBuffer(const std::shared_ptr<D3D11Device>& device,
		const ED3D11ConstantBuffer eConstantBuffer)
		:ResourceBase(ResourceType::ConstantBuffer),
		mConstantBuffer{}
	{
		mConstantBuffer = std::make_shared<DirectX::ConstantBuffer<ConstantType>>(device->GetDevice().Get());
	}
}

