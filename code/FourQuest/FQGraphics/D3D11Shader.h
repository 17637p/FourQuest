#pragma once

#include <memory>
#include <string>

#include <wrl.h>
#include <d3d11.h>

namespace fq::graphics
{
	class D3D11Device;

	class D3D11VertexShader
	{
	public:
		D3D11VertexShader(const std::shared_ptr<D3D11Device>& device,
			const std::wstring& path,
			const D3D_SHADER_MACRO* pDefines = nullptr,
			const std::string& entryPoint = "main",
			const std::string& shaderModel = "vs_5_0");

		void Bind(const std::shared_ptr<D3D11Device>& device);

		inline const std::wstring& GetPath() const;
		inline Microsoft::WRL::ComPtr<ID3D10Blob> GetBlob() const;
		inline Microsoft::WRL::ComPtr<ID3D11VertexShader> GetShader() const;

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

	class D3D11PixelShader
	{
	public:
		D3D11PixelShader(const std::shared_ptr<D3D11Device>& device,
			const std::wstring& path,
			const D3D_SHADER_MACRO* pDefines = nullptr,
			const std::string& entryPoint = "main",
			const std::string & shaderModel = "ps_5_0");

		void Bind(const std::shared_ptr<D3D11Device>& device);

		inline const std::wstring& GetPath() const;
		inline Microsoft::WRL::ComPtr<ID3D10Blob> GetBlob() const;
		inline Microsoft::WRL::ComPtr<ID3D11PixelShader> GetShader() const;

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
}

