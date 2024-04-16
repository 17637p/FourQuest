#pragma once

#include "ResourceBase.h"

#include <memory>
#include <wrl.h>
#include <d3d11.h>

#include "D3D11ResourceType.h"

namespace fq_graphics
{
	class D3D11Device;

	/*=============================================================================
		Input Layout
	=============================================================================*/
	class D3D11InputLayout : public ResourceBase
	{
	public:
		D3D11InputLayout(const std::shared_ptr<D3D11ResourceManager> resourceManager,
			const std::shared_ptr<D3D11Device>& device);

	private:
		Microsoft::WRL::ComPtr<ID3D11InputLayout> mInputLayout;
	};

	/*=============================================================================
		Vertex Buffer
	=============================================================================*/
	class D3D11VertexBuffer : public ResourceBase
	{
	public:
		D3D11VertexBuffer(const std::shared_ptr<D3D11ResourceManager> resourceManager,
			const std::shared_ptr<D3D11Device>& device);

	private:
		Microsoft::WRL::ComPtr<ID3D11Buffer> mVertexBuffer;
	};

	/*=============================================================================
		Index Buffer
	=============================================================================*/
	class D3D11IndexBuffer : public ResourceBase
	{
	public:
		D3D11IndexBuffer(const std::shared_ptr<D3D11ResourceManager> resourceManager,
			const std::shared_ptr<D3D11Device>& device);

	private:
		Microsoft::WRL::ComPtr<ID3D11Buffer> mIndexBuffer;
	};

	/*=============================================================================
		Constant Buffer
	=============================================================================*/
	class D3D11ConstantBuffer : public ResourceBase
	{
	public:
		D3D11ConstantBuffer(const std::shared_ptr<D3D11ResourceManager> resourceManager,
			const std::shared_ptr<D3D11Device>& device);

	private:
		Microsoft::WRL::ComPtr<ID3D11Buffer> mConstantBuffer;
	};
}

