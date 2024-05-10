#pragma once

#include <memory>
#include <string>
#include <wrl.h>
#include <d3d11.h>

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

	class D3D11GeometryShader
	{
	public:
		D3D11GeometryShader(const std::shared_ptr<D3D11Device>& device,
			const std::wstring& path,
			const D3D_SHADER_MACRO* pDefines = nullptr,
			const std::string& entryPoint = "main",
			const std::string& shaderModel = "gs_5_0");

		void Bind(const std::shared_ptr<D3D11Device>& device);

		inline const std::wstring& GetPath() const;
		inline Microsoft::WRL::ComPtr<ID3D10Blob> GetBlob() const;
		inline Microsoft::WRL::ComPtr<ID3D11GeometryShader> GetShader() const;

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

	class D3D11PixelShader
	{
	public:
		D3D11PixelShader(const std::shared_ptr<D3D11Device>& device,
			const std::wstring& path,
			const D3D_SHADER_MACRO* pDefines = nullptr,
			const std::string& entryPoint = "main",
			const std::string& shaderModel = "ps_5_0");

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

	class PipelineState;

	class ShaderProgram
	{
	public:
		ShaderProgram(const std::shared_ptr<D3D11Device>& device, 
			std::shared_ptr<D3D11VertexShader> vsOrNull, 
			std::shared_ptr<D3D11GeometryShader> gsOrNull, 
			std::shared_ptr<D3D11PixelShader> psOrNull, 
			std::shared_ptr<PipelineState> pipelineState);

		void Bind(const std::shared_ptr<D3D11Device>& device);

	private:
		std::shared_ptr<D3D11VertexShader> mVSOrNull;
		std::shared_ptr<D3D11InputLayout> mInputLayoutOrNull;
		std::shared_ptr<D3D11GeometryShader> mGSOrNull;
		std::shared_ptr<D3D11PixelShader> mPSOrNull;
		std::shared_ptr<PipelineState> mPipelineState;
	};
}

