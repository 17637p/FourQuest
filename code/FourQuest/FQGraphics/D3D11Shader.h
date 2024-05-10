#pragma once

#include <map>
#include <memory>
#include <string>
#include <wrl.h>
#include <d3d11.h>
#include <tuple>

#include "ResourceBase.h"
#include "D3D11ResourceType.h"

namespace fq::graphics
{
	class D3D11Device;

	class D3D11InputLayout
	{
	public:
		D3D11InputLayout(const std::shared_ptr<D3D11Device>& device, ID3DBlob* VSBytecode);

		void Bind(const std::shared_ptr<D3D11Device>& device);

	private:
		Microsoft::WRL::ComPtr<ID3D11InputLayout> mInputLayout;
	};

	class D3D11VertexShader : public ResourceBase
	{
	public:
		D3D11VertexShader(const std::shared_ptr<D3D11Device>& device,
			ED3D11VertexShader type);
		D3D11VertexShader(const std::shared_ptr<D3D11Device>& device,
			const std::wstring& path,
			const D3D_SHADER_MACRO* pDefines = nullptr,
			const std::string& entryPoint = "main",
			const std::string& shaderModel = "vs_5_0");

		static std::string GenerateRID(ED3D11VertexShader type);

		void Bind(const std::shared_ptr<D3D11Device>& device);

		inline const std::wstring& GetPath() const;
		inline Microsoft::WRL::ComPtr<ID3D10Blob> GetBlob() const;
		inline Microsoft::WRL::ComPtr<ID3D11VertexShader> GetShader() const;

	private:
		void create(const std::shared_ptr<D3D11Device>& device,
			const std::wstring& path,
			const D3D_SHADER_MACRO* pDefines = nullptr,
			const std::string& entryPoint = "main",
			const std::string& shaderModel = "vs_5_0");

	private:
		std::wstring mPath;
		Microsoft::WRL::ComPtr<ID3D10Blob> mBlob;
		Microsoft::WRL::ComPtr<ID3D11VertexShader> mShader;
	};

	inline const std::wstring& D3D11VertexShader::GetPath() const
	{
		return mPath;
	}
	inline Microsoft::WRL::ComPtr<ID3D10Blob> D3D11VertexShader::GetBlob() const
	{
		return mBlob;
	}
	inline Microsoft::WRL::ComPtr<ID3D11VertexShader> D3D11VertexShader::GetShader() const
	{
		return mShader;
	}

	class D3D11GeometryShader : public ResourceBase
	{
	public:
		D3D11GeometryShader(const std::shared_ptr<D3D11Device>& device,
			ED3D11GeometryShader type);
		D3D11GeometryShader(const std::shared_ptr<D3D11Device>& device,
			const std::wstring& path,
			const D3D_SHADER_MACRO* pDefines = nullptr,
			const std::string& entryPoint = "main",
			const std::string& shaderModel = "gs_5_0");

		static std::string GenerateRID(ED3D11GeometryShader type);

		void Bind(const std::shared_ptr<D3D11Device>& device);

		inline const std::wstring& GetPath() const;
		inline Microsoft::WRL::ComPtr<ID3D10Blob> GetBlob() const;
		inline Microsoft::WRL::ComPtr<ID3D11GeometryShader> GetShader() const;

	private:
		void create(const std::shared_ptr<D3D11Device>& device,
			const std::wstring& path,
			const D3D_SHADER_MACRO* pDefines,
			const std::string& entryPoint,
			const std::string& shaderModel);

	private:
		std::wstring mPath;
		Microsoft::WRL::ComPtr<ID3D10Blob> mBlob;
		Microsoft::WRL::ComPtr<ID3D11GeometryShader> mShader;
	};

	inline const std::wstring& D3D11GeometryShader::GetPath() const
	{
		return mPath;
	}
	inline Microsoft::WRL::ComPtr<ID3D10Blob> D3D11GeometryShader::GetBlob() const
	{
		return mBlob;
	}
	inline Microsoft::WRL::ComPtr<ID3D11GeometryShader> D3D11GeometryShader::GetShader() const
	{
		return mShader;
	}

	class D3D11PixelShader : public ResourceBase
	{
	public:
		D3D11PixelShader(const std::shared_ptr<D3D11Device>& device,
			ED3D11PixelShader type);
		D3D11PixelShader(const std::shared_ptr<D3D11Device>& device,
			const std::wstring& path,
			const D3D_SHADER_MACRO* pDefines = nullptr,
			const std::string& entryPoint = "main",
			const std::string& shaderModel = "ps_5_0");

		static std::string GenerateRID(ED3D11PixelShader type);

		void Bind(const std::shared_ptr<D3D11Device>& device);

		inline const std::wstring& GetPath() const;
		inline Microsoft::WRL::ComPtr<ID3D10Blob> GetBlob() const;
		inline Microsoft::WRL::ComPtr<ID3D11PixelShader> GetShader() const;

	private:
		void create(const std::shared_ptr<D3D11Device>& device,
			const std::wstring& path,
			const D3D_SHADER_MACRO* pDefines = nullptr,
			const std::string& entryPoint = "main",
			const std::string& shaderModel = "ps_5_0");

	private:
		std::wstring mPath;
		Microsoft::WRL::ComPtr<ID3D10Blob> mBlob;
		Microsoft::WRL::ComPtr<ID3D11PixelShader> mShader;
	};

	inline const std::wstring& D3D11PixelShader::GetPath() const
	{
		return mPath;
	}
	inline Microsoft::WRL::ComPtr<ID3D10Blob> D3D11PixelShader::GetBlob() const
	{
		return mBlob;
	}
	inline Microsoft::WRL::ComPtr<ID3D11PixelShader> D3D11PixelShader::GetShader() const
	{
		return mShader;
	}

	class D3D11SamplerState;
	class D3D11ConstantBufferNT;
	class PipelineState;
	class D3D11ResourceManager;

	class D3D11ShaderProgram
	{
	public:
		D3D11ShaderProgram(const std::shared_ptr<D3D11Device>& device,
			std::shared_ptr<D3D11ResourceManager> resourceManager,
			std::shared_ptr<D3D11VertexShader> vertexShader,
			std::shared_ptr<D3D11GeometryShader> geometryShader,
			std::shared_ptr<D3D11PixelShader> pixelShader,
			std::shared_ptr<PipelineState> pipelineState);
		~D3D11ShaderProgram() = default;

		static void SetSamplerStateMap(std::map<std::string, ED3D11SamplerState> samplerStateMap);
		static void SetConstantBufferMap(std::map<std::string, ED3D11ConstantBufferNT> constnatBufferMap);

		void Bind(const std::shared_ptr<D3D11Device>& device);

		template <typename T>
		std::shared_ptr<D3D11ConstantBufferNT> GetConstantBufferNT()
		{
			std::string key = T::GetName();
			auto find = nConstnatBufferNTs.find(key);
			assert(find != nConstnatBufferNTs.end());
			std::tuple<ED3D11ShaderType, UINT, std::shared_ptr<D3D11ConstantBufferNT>>& data = find->second;

			return std::get<2>(data);
		}
		template <typename T>
		void UpdateConstantBuffer(const std::shared_ptr<D3D11Device>& device, const T& data)
		{
			auto cb = GetConstantBufferNT<T>();
			cb->Update(device, &data, sizeof(data));
		}

	private:
		void reflect(const std::shared_ptr<D3D11Device>& device, ED3D11ShaderType shaderType, Microsoft::WRL::ComPtr<ID3D10Blob> blob, std::shared_ptr<D3D11ResourceManager> resourceManager);

	private:
		static std::map<std::string, ED3D11SamplerState> mSamplerStateMap;
		static std::map<std::string, ED3D11ConstantBufferNT> mConstnatBufferMap;

		std::shared_ptr<D3D11InputLayout> mInputLayout;
		std::shared_ptr<D3D11VertexShader> mVertexShader;
		std::shared_ptr<D3D11GeometryShader> mGeometryShader;
		std::shared_ptr<D3D11PixelShader> mPixelShader;
		std::shared_ptr<PipelineState> mPipelineState;

		std::map<std::string, std::tuple<ED3D11ShaderType, UINT, std::shared_ptr<D3D11ConstantBufferNT>>> nConstnatBufferNTs;

		std::map<std::pair<ED3D11ConstantBufferNT, ED3D11ShaderType>, std::pair<std::shared_ptr<D3D11ConstantBufferNT>, UINT>> mConstantBuffers;
		std::map<std::pair<ED3D11SamplerState, ED3D11ShaderType>, std::pair<std::shared_ptr<D3D11SamplerState>, UINT>> mSamplers;

		// 샘플러
		// 상수 버퍼 바인딩까지는 일반화 시켜놓을 수 있지 않을까 T 타입으로 받아오는 게 근데 매 프레임 이뤄지면 이상할 거 같은데
		// 취득 후에는 처리하는 거 이어서 하면 되지 않을까 cbuffer가 쉐이더 쪽이랑 CPU 쪽 한 번은 동기화가 이뤄져야 되는 건데
		// 이거 어차피 근데 업데이트하려면 알아야되는 거니까 별 의미가 없을 거 같기도 한데
		// 그래도 생성단계에서 자동화되면 좋을 거 같긴 하네

		// 생성과 바인딩 함수까지 자동화 시켜놓고,
		// 상수버퍼 취득해서 업데이트 하는거만 로우하게 처리하면 굉장히 깔끔할 거 같은데
		// 쉐이더 리소스는 재질마다 달라지니 미리 바인딩할 경우가 아니라면 쉐이더에 넣지 않은 편이 좋게 느껴진다.
	};
}
