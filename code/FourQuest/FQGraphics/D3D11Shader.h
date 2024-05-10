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

		// ���÷�
		// ��� ���� ���ε������� �Ϲ�ȭ ���ѳ��� �� ���� ������ T Ÿ������ �޾ƿ��� �� �ٵ� �� ������ �̷����� �̻��� �� ������
		// ��� �Ŀ��� ó���ϴ� �� �̾ �ϸ� ���� ������ cbuffer�� ���̴� ���̶� CPU �� �� ���� ����ȭ�� �̷����� �Ǵ� �ǵ�
		// �̰� ������ �ٵ� ������Ʈ�Ϸ��� �˾ƾߵǴ� �Ŵϱ� �� �ǹ̰� ���� �� ���⵵ �ѵ�
		// �׷��� �����ܰ迡�� �ڵ�ȭ�Ǹ� ���� �� ���� �ϳ�

		// ������ ���ε� �Լ����� �ڵ�ȭ ���ѳ���,
		// ������� ����ؼ� ������Ʈ �ϴ°Ÿ� �ο��ϰ� ó���ϸ� ������ ����� �� ������
		// ���̴� ���ҽ��� �������� �޶����� �̸� ���ε��� ��찡 �ƴ϶�� ���̴��� ���� ���� ���� ���� ��������.
	};
}
