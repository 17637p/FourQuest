#pragma once

#include "ResourceBase.h"

#include <memory>
#include <string>
#include <wrl.h>
#include <string>
#include <vector>
#include <d3d11.h>
#include <directxtk/BufferHelpers.h>
#include <deque>
#include <directxtk/SimpleMath.h>
#include <directxtk\SimpleMath.h>

#include "D3D11Device.h"
#include "D3D11ResourceType.h"
#include "Define.h"

namespace fq::graphics
{
	class D3D11Device;

	/*=============================================================================
		Vertex Buffer
	=============================================================================*/
	class D3D11VertexBuffer
	{
	public:
		template<typename T>
		D3D11VertexBuffer(const std::shared_ptr<D3D11Device>& device, const std::vector<T>& vertices, bool bUseCPUWrite = false);
		D3D11VertexBuffer(const std::shared_ptr<D3D11Device>& device, unsigned int size, unsigned int count, void* ptr, bool bUseCPUWrite = false);
		D3D11VertexBuffer(const std::shared_ptr<D3D11Device>& device, UINT size, UINT stride, UINT offset);

		static void Bind(const std::shared_ptr<D3D11Device>& device, const std::vector<std::shared_ptr<D3D11VertexBuffer>>& buffers, UINT startSlot = 0);
		static D3D11VertexBuffer CreateFullScreenVertexBuffer(const std::shared_ptr<D3D11Device>& device);

		void Bind(const std::shared_ptr<D3D11Device>& device, UINT startSlot = 0);
		template <typename Vertex>
		void Update(std::shared_ptr<D3D11Device> device, const std::deque<Vertex>& vertices);
		template <typename Vertex>
		void Update(std::shared_ptr<D3D11Device> device, const std::vector<Vertex>& vertices);

		Microsoft::WRL::ComPtr<ID3D11Buffer> GetVertexBuffer() const { return mVertexBuffer; }

		UINT GetStride() const { return mStride; }

	private:
		Microsoft::WRL::ComPtr<ID3D11Buffer> mVertexBuffer;
		UINT mStride;
		UINT mOffset;
		bool mbUseCPUWrite;
	};

	template<typename T>
	D3D11VertexBuffer::D3D11VertexBuffer(const std::shared_ptr<D3D11Device>& device, const std::vector<T>& vertices, bool bUseCPUWrite)
		: mStride(sizeof(T))
		, mOffset(0)
		, mbUseCPUWrite(bUseCPUWrite)
	{
		D3D11_BUFFER_DESC bd = {};
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.Usage = bUseCPUWrite ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_IMMUTABLE;
		bd.CPUAccessFlags = bUseCPUWrite ? D3D11_CPU_ACCESS_WRITE : 0u;
		bd.MiscFlags = 0u;
		bd.ByteWidth = (UINT)(sizeof(T) * vertices.size());
		bd.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA sd = {};
		sd.pSysMem = vertices.data();

		device->GetDevice().Get()->CreateBuffer(&bd, &sd, mVertexBuffer.GetAddressOf());
	}

	template <typename Vertex>
	void D3D11VertexBuffer::Update(std::shared_ptr<D3D11Device> device, const std::deque<Vertex>& vertices)
	{
		assert(mbUseCPUWrite);

		D3D11_MAPPED_SUBRESOURCE mappedData;
		device->GetDeviceContext()->Map(mVertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);

		Vertex* vertexPtr = reinterpret_cast<Vertex*>(mappedData.pData);

		for (UINT i = 0; i < vertices.size(); ++i)
		{
			vertexPtr[i] = vertices[i];
		}

		device->GetDeviceContext()->Unmap(mVertexBuffer.Get(), 0);
	}

	template<typename Vertex>
	inline void D3D11VertexBuffer::Update(std::shared_ptr<D3D11Device> device, const std::vector<Vertex>& vertices)
	{
		assert(mbUseCPUWrite);

		D3D11_MAPPED_SUBRESOURCE mappedData;
		device->GetDeviceContext()->Map(mVertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);

		Vertex* vertexPtr = reinterpret_cast<Vertex*>(mappedData.pData);

		memcpy(vertexPtr, vertices.data(), sizeof(Vertex) * vertices.size());

		device->GetDeviceContext()->Unmap(mVertexBuffer.Get(), 0);
	}

	/*=============================================================================
		Index Buffer
	=============================================================================*/
	class D3D11IndexBuffer
	{
	public:
		D3D11IndexBuffer(const std::shared_ptr<D3D11Device>& device, const std::vector<unsigned int>& indices);

		static D3D11IndexBuffer CreateFullScreenIndexBuffer(const std::shared_ptr<D3D11Device>& device)
		{
			std::vector<unsigned int> indices =
			{
				0,1,2,
				1,3,2
			};

			return D3D11IndexBuffer(device, indices);
		}

		void Bind(const std::shared_ptr<D3D11Device>& device);

		Microsoft::WRL::ComPtr<ID3D11Buffer> GetIndexBuffer() const { return mIndexBuffer; }

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
		static_assert(sizeof(ConstantType) % 16 == 0, "constant buffer must be aligned by 16 bytes");

	public:
		D3D11ConstantBuffer(const std::shared_ptr<D3D11Device>& device,
			const ED3D11ConstantBuffer eConstantBuffer);

		static std::string GenerateRID(const ED3D11ConstantBuffer eConstantBuffer);

		void Update(const std::shared_ptr<D3D11Device>& d3d11Device, const ConstantType& value);

		void Bind(const std::shared_ptr<D3D11Device>& d3d11Device, const ED3D11ShaderType eShaderType, const UINT startSlot = 0);

		ID3D11Buffer* GetBuffer() { return mConstantBuffer->GetBuffer(); }

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
		case ED3D11ShaderType::PixelShader:
		{
			d3d11Device->GetDeviceContext()->PSSetConstantBuffers(startSlot, 1, &constantBuffer);
			break;
		}
		case ED3D11ShaderType::ComputeShader:
		{
			d3d11Device->GetDeviceContext()->CSSetConstantBuffers(startSlot, 1, &constantBuffer);
			break;
		}
		case ED3D11ShaderType::DomainShader:
		{
			d3d11Device->GetDeviceContext()->DSSetConstantBuffers(startSlot, 1, &constantBuffer);
			break;
		}
		case ED3D11ShaderType::HullShader:
		{
			d3d11Device->GetDeviceContext()->HSSetConstantBuffers(startSlot, 1, &constantBuffer);
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

	class D3D11StructuredBuffer
	{
	public:
		D3D11StructuredBuffer(const std::shared_ptr<D3D11Device>& device, size_t typeSize, size_t maxCount)
			: mBuffer(nullptr)
			, mTypeSize(typeSize)
			, mMaxCount(maxCount)
		{
			D3D11_BUFFER_DESC desc;
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.ByteWidth = typeSize * maxCount;
			desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
			desc.CPUAccessFlags = 0;
			desc.StructureByteStride = typeSize;
			desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

			HR(device->GetDevice()->CreateBuffer(&desc, nullptr, mBuffer.GetAddressOf()));

			desc.Usage = D3D11_USAGE_STAGING;
			desc.BindFlags = 0;
			desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

			HR(device->GetDevice()->CreateBuffer(&desc, nullptr, mDebugBuffer.GetAddressOf()));
		}
		~D3D11StructuredBuffer() = default;

		Microsoft::WRL::ComPtr<ID3D11Buffer> GetBuffer() const
		{
			return mBuffer;
		}

		Microsoft::WRL::ComPtr<ID3D11Buffer> GetDebugBuffer() const
		{
			return mDebugBuffer;
		}

		size_t GetTypeSize() const
		{
			return mTypeSize;
		}

		size_t GetMaxCount() const
		{
			return mMaxCount;
		}

		size_t GetByteWidth() const
		{
			return GetTypeSize() * GetMaxCount();
		}


	private:
		Microsoft::WRL::ComPtr<ID3D11Buffer> mBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer> mDebugBuffer;
		size_t mTypeSize;
		size_t mMaxCount;
	};
}

