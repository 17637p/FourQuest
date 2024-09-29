#include <d3dcompiler.h>
#include <fstream>
#include "D3D11Shader.h"
#include "D3D11Util.h"
#include "D3D11Device.h"
#include "D3D11State.h"
#include "Define.h"

namespace fq::graphics
{
	std::wstring D3D11Shader::mBasePath;

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


	D3D11Shader::D3D11Shader(const std::shared_ptr<D3D11Device>& device,
		const std::wstring& path,
		const D3D_SHADER_MACRO* pDefines,
		const std::string& entryPoint,
		const std::string& shaderModel)
		: mPath(path)
		, mBlob(nullptr)
	{
		HRESULT hr = D3D11Util::CompileShaderFromFile((mBasePath + path).c_str(),
			pDefines,
			entryPoint.c_str(),
			shaderModel.c_str(),
			mBlob.GetAddressOf());
	}
	D3D11Shader::D3D11Shader(const std::shared_ptr<D3D11Device>& device, const std::wstring& binaryFilePath)
		: mPath(binaryFilePath)
		, mBlob(nullptr)
	{
		std::ifstream fin(mBasePath + binaryFilePath, std::ios::binary);
		fin.seekg(0, std::ios_base::end);
		int size = (int)fin.tellg();
		fin.seekg(0, std::ios_base::beg);
		std::vector<char> fileData(size);
		fin.read(&fileData[0], size);
		fin.close();

		D3DCreateBlob(size, mBlob.GetAddressOf());

		memcpy(mBlob->GetBufferPointer(), fileData.data(), size);
	}

	D3D11VertexShader::D3D11VertexShader(const std::shared_ptr<D3D11Device>& device,
		const std::wstring& path,
		const D3D_SHADER_MACRO* pDefines,
		const std::string& entryPoint,
		const std::string& shaderModel)
		: D3D11Shader(device, path, pDefines, entryPoint, shaderModel)
		, mShader(nullptr)
	{
		HR(device->GetDevice()->CreateVertexShader(
			mBlob->GetBufferPointer(),
			mBlob->GetBufferSize(),
			nullptr,
			mShader.GetAddressOf()));
	}
	D3D11VertexShader::D3D11VertexShader(const std::shared_ptr<D3D11Device>& device, const std::wstring& binaryFilePath)
		:D3D11Shader(device, binaryFilePath)
		, mShader(nullptr)
	{
		HR(device->GetDevice()->CreateVertexShader(
			mBlob->GetBufferPointer(),
			mBlob->GetBufferSize(),
			nullptr,
			mShader.GetAddressOf()));
	}
	void D3D11VertexShader::Bind(const std::shared_ptr<D3D11Device>& device)
	{
		device->GetDeviceContext()->VSSetShader(mShader.Get(), NULL, NULL);
	}

	D3D11GeometryShader::D3D11GeometryShader(const std::shared_ptr<D3D11Device>& device,
		const std::wstring& path,
		const D3D_SHADER_MACRO* pDefines,
		const std::string& entryPoint,
		const std::string& shaderModel)
		: D3D11Shader(device, path, pDefines, entryPoint, shaderModel)
		, mShader(nullptr)
	{
		HR(device->GetDevice()->CreateGeometryShader(
			mBlob->GetBufferPointer(),
			mBlob->GetBufferSize(),
			nullptr,
			mShader.GetAddressOf()));
	}

	D3D11GeometryShader::D3D11GeometryShader(const std::shared_ptr<D3D11Device>& device, const std::wstring& binaryFilePath)
		: D3D11Shader(device, binaryFilePath)
	{
		HR(device->GetDevice()->CreateGeometryShader(
			mBlob->GetBufferPointer(),
			mBlob->GetBufferSize(),
			nullptr,
			mShader.GetAddressOf()));
	}

	void D3D11GeometryShader::Bind(const std::shared_ptr<D3D11Device>& device)
	{
		device->GetDeviceContext()->GSSetShader(mShader.Get(), NULL, NULL);
	}

	D3D11PixelShader::D3D11PixelShader(const std::shared_ptr<D3D11Device>& device,
		const std::wstring& path,
		const D3D_SHADER_MACRO* pDefines,
		const std::string& entryPoint,
		const std::string& shaderModel)
		: D3D11Shader(device, path, pDefines, entryPoint, shaderModel)
		, mShader(nullptr)
	{
		HR(device->GetDevice()->CreatePixelShader(
			mBlob->GetBufferPointer(),
			mBlob->GetBufferSize(),
			nullptr,
			mShader.GetAddressOf()));
	}
	D3D11PixelShader::D3D11PixelShader(const std::shared_ptr<D3D11Device>& device, const std::wstring& binaryFilePath)
		: D3D11Shader(device, binaryFilePath)
	{
		HR(device->GetDevice()->CreatePixelShader(
			mBlob->GetBufferPointer(),
			mBlob->GetBufferSize(),
			nullptr,
			mShader.GetAddressOf()));
	}
	void D3D11PixelShader::Bind(const std::shared_ptr<D3D11Device>& device)
	{
		device->GetDeviceContext()->PSSetShader(mShader.Get(), NULL, NULL);
	}

	D3D11ComputeShader::D3D11ComputeShader(const std::shared_ptr<D3D11Device>& device,
		const std::wstring& path,
		const D3D_SHADER_MACRO* pDefines,
		const std::string& entryPoint,
		const std::string& shaderModel)
		: D3D11Shader(device, path, pDefines, entryPoint, shaderModel)
		, mShader(nullptr)
	{
		HR(device->GetDevice()->CreateComputeShader(
			mBlob->GetBufferPointer(),
			mBlob->GetBufferSize(),
			nullptr,
			mShader.GetAddressOf()));
	}
	D3D11ComputeShader::D3D11ComputeShader(const std::shared_ptr<D3D11Device>& device, const std::wstring& binaryFilePath)
		:D3D11Shader(device, binaryFilePath)
	{
		HR(device->GetDevice()->CreateComputeShader(
			mBlob->GetBufferPointer(),
			mBlob->GetBufferSize(),
			nullptr,
			mShader.GetAddressOf()));
	}
	void D3D11ComputeShader::Bind(const std::shared_ptr<D3D11Device>& device)
	{
		device->GetDeviceContext()->CSSetShader(mShader.Get(), NULL, NULL);
	}

	ShaderProgram::ShaderProgram(const std::shared_ptr<D3D11Device>& device,
		std::shared_ptr<D3D11VertexShader> vsOrNull,
		std::shared_ptr<D3D11GeometryShader> gsOrNull,
		std::shared_ptr<D3D11PixelShader> psOrNull,
		std::shared_ptr<PipelineState> pipelineState)
		: ShaderProgram(device, pipelineState, vsOrNull, psOrNull, gsOrNull, nullptr)
	{
	}

	ShaderProgram::ShaderProgram(const std::shared_ptr<D3D11Device>& device,
		std::shared_ptr<PipelineState> pipelineState,
		std::shared_ptr<D3D11VertexShader> vsOrNull,
		std::shared_ptr<D3D11PixelShader> psOrNull,
		std::shared_ptr<D3D11GeometryShader> gsOrNull,
		std::shared_ptr<D3D11ComputeShader> csOrNull)
		: mPipelineState(pipelineState)
		, mVSOrNull(vsOrNull)
		, mPSOrNull(psOrNull)
		, mGSOrNull(gsOrNull)
		, mCSOrNull(csOrNull)
	{
		assert(pipelineState != nullptr);

		if (mVSOrNull != nullptr)
		{
			assert(mVSOrNull->GetBlob() != nullptr);
			mInputLayoutOrNull = std::make_shared<D3D11InputLayout>(device, mVSOrNull->GetBlob().Get());
		}
	}

	void ShaderProgram::Bind(const std::shared_ptr<D3D11Device>& device)
	{
		if (mVSOrNull != nullptr)
		{
			mVSOrNull->Bind(device);
			mInputLayoutOrNull->Bind(device);
		}
		else
		{
			device->GetDeviceContext()->VSSetShader(NULL, NULL, NULL);
		}

		if (mGSOrNull != nullptr)
		{
			mGSOrNull->Bind(device);
		}
		else
		{
			device->GetDeviceContext()->GSSetShader(NULL, NULL, NULL);
		}

		if (mPSOrNull != nullptr)
		{
			mPSOrNull->Bind(device);
		}
		else
		{
			device->GetDeviceContext()->PSSetShader(NULL, NULL, NULL);
		}

		if (mCSOrNull != nullptr)
		{
			mCSOrNull->Bind(device);
		}
		else
		{
			device->GetDeviceContext()->CSSetShader(NULL, NULL, NULL);
		}

		mPipelineState->Bind(device);
	}

	void ShaderProgram::Unbind(const std::shared_ptr<D3D11Device>& device)
	{
		device->GetDeviceContext()->VSSetShader(NULL, NULL, NULL);
		device->GetDeviceContext()->GSSetShader(NULL, NULL, NULL);
		device->GetDeviceContext()->PSSetShader(NULL, NULL, NULL);
		device->GetDeviceContext()->CSSetShader(NULL, NULL, NULL);
	}

	void D3D11HullShader::Bind(const std::shared_ptr<D3D11Device>& device)
	{
		device->GetDeviceContext()->HSSetShader(mShader.Get(), NULL, NULL);
	}

	Microsoft::WRL::ComPtr<ID3D11HullShader> D3D11HullShader::GetShader() const
	{
		return mShader;
	}

	D3D11HullShader::D3D11HullShader(const std::shared_ptr<D3D11Device>& device, const std::wstring& path, const D3D_SHADER_MACRO* pDefines /*= nullptr*/, const std::string& entryPoint /*= "main"*/, const std::string& shaderModel /*= "hs_5_0"*/)
		:D3D11Shader(device, path, pDefines, entryPoint, shaderModel)
		, mShader(nullptr)
	{
		HR(device->GetDevice()->CreateHullShader(
			mBlob->GetBufferPointer(),
			mBlob->GetBufferSize(),
			nullptr,
			mShader.GetAddressOf()));
	}

	D3D11HullShader::D3D11HullShader(const std::shared_ptr<D3D11Device>& device, const std::wstring& binaryFilePath)
		: D3D11Shader(device, binaryFilePath)
	{
		HR(device->GetDevice()->CreateHullShader(
			mBlob->GetBufferPointer(),
			mBlob->GetBufferSize(),
			nullptr,
			mShader.GetAddressOf()));
	}

	D3D11DomainShader::D3D11DomainShader(const std::shared_ptr<D3D11Device>& device, const std::wstring& path, const D3D_SHADER_MACRO* pDefines /*= nullptr*/, const std::string& entryPoint /*= "main"*/, const std::string& shaderModel /*= "hs_5_0"*/)
		:D3D11Shader(device, path, pDefines, entryPoint, shaderModel)
		, mShader(nullptr)
	{
		HR(device->GetDevice()->CreateDomainShader(
			mBlob->GetBufferPointer(),
			mBlob->GetBufferSize(),
			nullptr,
			mShader.GetAddressOf()));
	}

	D3D11DomainShader::D3D11DomainShader(const std::shared_ptr<D3D11Device>& device, const std::wstring& binaryFilePath)
		:D3D11Shader(device, binaryFilePath)
	{
		HR(device->GetDevice()->CreateDomainShader(
			mBlob->GetBufferPointer(),
			mBlob->GetBufferSize(),
			nullptr,
			mShader.GetAddressOf()));
	}

	void D3D11DomainShader::Bind(const std::shared_ptr<D3D11Device>& device)
	{
		device->GetDeviceContext()->DSSetShader(mShader.Get(), NULL, NULL);
	}

}

