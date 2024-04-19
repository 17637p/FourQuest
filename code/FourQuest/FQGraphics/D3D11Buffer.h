#pragma once

#include "ResourceBase.h"

#include <memory>
#include <string>
#include <wrl.h>
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
	class D3D11InputLayout : public ResourceBase
	{
	public:
		D3D11InputLayout(const std::shared_ptr<D3D11Device>& device);

	private:
		Microsoft::WRL::ComPtr<ID3D11InputLayout> mInputLayout;
	};

	/*=============================================================================
		Vertex Buffer
	=============================================================================*/
	class D3D11VertexBuffer : public ResourceBase
	{
	public:
		D3D11VertexBuffer(const std::shared_ptr<D3D11Device>& device);

	private:
		Microsoft::WRL::ComPtr<ID3D11Buffer> mVertexBuffer;
	};

	/*=============================================================================
		Index Buffer
	=============================================================================*/
	class D3D11IndexBuffer : public ResourceBase
	{
	public:
		D3D11IndexBuffer(const std::shared_ptr<D3D11Device>& device);

	private:
		Microsoft::WRL::ComPtr<ID3D11Buffer> mIndexBuffer;
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
		std::shared_ptr<DirectX::ConstantBuffer<ConstantType> > mConstantBuffer;
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

