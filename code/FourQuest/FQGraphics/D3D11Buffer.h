#pragma once

#include "ResourceBase.h"

#include <memory>
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
	class D3D11ConstantBuffer
	{
	public:
		D3D11ConstantBuffer(const std::shared_ptr<D3D11Device>& device);

		void Update(const std::shared_ptr<D3D11Device>& d3d11Device, const ConstantType& value);

		void Bind(const std::shared_ptr<D3D11Device>& d3d11Device, const ED3D11ShaderType eShaderType);

	private:
		std::shared_ptr<DirectX::ConstantBuffer<ConstantType> > mConstantBuffer;
	};

	template<typename ConstantType>
	void fq::graphics::D3D11ConstantBuffer<ConstantType>::Update(const std::shared_ptr<D3D11Device>& d3d11Device, const ConstantType& value)
	{
		mConstantBuffer->SetData(d3d11Device->GetDeviceContext(), value);
	}

	template<typename ConstantType>
	void fq::graphics::D3D11ConstantBuffer<ConstantType>::Bind(const std::shared_ptr<D3D11Device>& d3d11Device, const ED3D11ShaderType eShaderType)
	{
		switch (eShaderType)
		{
			case ED3D11ShaderType::VertexShader:
			{
				//d3d11Device->GetDeviceContext()->VSSetConstantBuffers(bufferOffset, 1, &_meshConstantBuffer);
				break;
			}
			case ED3D11ShaderType::GeometryShader:
			{
				break;
			}
			case ED3D11ShaderType::Pixelshader:
			{
				break;
			}
			default:
				break;
		}
	}

	template<typename ConstantType>
	fq::graphics::D3D11ConstantBuffer<ConstantType>::D3D11ConstantBuffer(const std::shared_ptr<D3D11Device>& device)
	{
		mConstantBuffer->Create(device->GetDevice());
	}

	/*class D3D11ConstantBuffer : public ResourceBase
	{
	public:
		D3D11ConstantBuffer(const std::shared_ptr<D3D11ResourceManager> resourceManager,
			const std::shared_ptr<D3D11Device>& device);

	private:
		Microsoft::WRL::ComPtr<ID3D11Buffer> mConstantBuffer;
	};*/
}

