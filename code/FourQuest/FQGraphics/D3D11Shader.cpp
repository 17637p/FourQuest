#include <cassert>
#include <d3dcompiler.h>
#include "D3D11Shader.h"
#include "D3D11Util.h"
#include "D3D11Device.h"
#include "D3D11Buffer.h"
#include "D3D11State.h"
#include "D3D11ResourceManager.h"

namespace fq::graphics
{
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

	D3D11VertexShader::D3D11VertexShader(const std::shared_ptr<D3D11Device>& device,
		ED3D11VertexShader type)
		: ResourceBase(ResourceType::VertexShader)
		, mBlob(nullptr)
		, mShader(nullptr)
	{
		// 쉐이더 .cso로 변환한 다음에 스트링 테이블을 만들어주는 편이 더 깔끔할 거 같다
		switch (type)
		{
		case fq::graphics::ED3D11VertexShader::None:
			return;
			break;
		case fq::graphics::ED3D11VertexShader::StaticModel:
			create(device, L"./resource/internal/shader/ModelVS.hlsl");
			break;
		case fq::graphics::ED3D11VertexShader::SkinnedModel:
		{
			D3D_SHADER_MACRO macroSkinning[] =
			{
				{"SKINNING", ""},
				{ NULL, NULL}
			};

			create(device, L"./resource/internal/shader/ModelVS.hlsl", macroSkinning);
			break;
		}
		default:
			assert(false);
			break;
		}
	}

	D3D11VertexShader::D3D11VertexShader(const std::shared_ptr<D3D11Device>& device,
		const std::wstring& path,
		const D3D_SHADER_MACRO* pDefines,
		const std::string& entryPoint,
		const std::string& shaderModel)
		: ResourceBase(ResourceType::VertexShader)
		, mPath(path)
		, mBlob(nullptr)
		, mShader(nullptr)
	{
		create(device, path, pDefines, entryPoint, shaderModel);
	}

	void D3D11VertexShader::create(const std::shared_ptr<D3D11Device>& device,
		const std::wstring& path,
		const D3D_SHADER_MACRO* pDefines,
		const std::string& entryPoint,
		const std::string& shaderModel)
	{
		HRESULT hr = D3D11Util::CompileShaderFromFile(path.c_str(),
			pDefines,
			entryPoint.c_str(),
			shaderModel.c_str(),
			mBlob.GetAddressOf());
		assert(SUCCEEDED(hr));

		hr = device->GetDevice()->CreateVertexShader(
			mBlob->GetBufferPointer(),
			mBlob->GetBufferSize(),
			nullptr,
			mShader.GetAddressOf());
		assert(SUCCEEDED(hr));
	}

	std::string D3D11VertexShader::GenerateRID(ED3D11VertexShader type)
	{
		return typeid(D3D11VertexShader).name() + std::to_string(static_cast<int>(type));
	}

	void D3D11VertexShader::Bind(const std::shared_ptr<D3D11Device>& device)
	{
		device->GetDeviceContext()->VSSetShader(mShader == nullptr ? nullptr : mShader.Get(), NULL, NULL);
	}

	D3D11GeometryShader::D3D11GeometryShader(const std::shared_ptr<D3D11Device>& device,
		ED3D11GeometryShader type)
		: ResourceBase(ResourceType::GeometryShader)
	{
		switch (type)
		{
		case fq::graphics::ED3D11GeometryShader::None:
			return;
			break;
		default:
			assert(false);
			break;
		}
	}

	D3D11GeometryShader::D3D11GeometryShader(const std::shared_ptr<D3D11Device>& device,
		const std::wstring& path,
		const D3D_SHADER_MACRO* pDefines,
		const std::string& entryPoint,
		const std::string& shaderModel)
		: ResourceBase(ResourceType::GeometryShader)
	{
		create(device, path, pDefines, entryPoint, shaderModel);
	}

	void D3D11GeometryShader::create(const std::shared_ptr<D3D11Device>& device,
		const std::wstring& path,
		const D3D_SHADER_MACRO* pDefines,
		const std::string& entryPoint,
		const std::string& shaderModel)
	{
		HRESULT hr = D3D11Util::CompileShaderFromFile(path.c_str(),
			pDefines,
			entryPoint.c_str(),
			shaderModel.c_str(),
			mBlob.GetAddressOf());

		hr = device->GetDevice()->CreateGeometryShader(
			mBlob->GetBufferPointer(),
			mBlob->GetBufferSize(),
			nullptr,
			mShader.GetAddressOf());
	}

	std::string D3D11GeometryShader::GenerateRID(ED3D11GeometryShader type)
	{
		return typeid(D3D11GeometryShader).name() + std::to_string(static_cast<int>(type));
	}

	void D3D11GeometryShader::Bind(const std::shared_ptr<D3D11Device>& device)
	{
		device->GetDeviceContext()->GSSetShader(mShader == nullptr ? nullptr : mShader.Get(), NULL, NULL);
	}

	D3D11PixelShader::D3D11PixelShader(const std::shared_ptr<D3D11Device>& device,
		ED3D11PixelShader type)
		: ResourceBase(ResourceType::PixelShader)
	{
		switch (type)
		{
		case fq::graphics::ED3D11PixelShader::None:
			return;
			break;
		case fq::graphics::ED3D11PixelShader::Model:
			create(device, L"./resource/internal/shader/ModelPS.hlsl");
			break;
		default:
			assert(false);
			break;
		}
	}

	D3D11PixelShader::D3D11PixelShader(const std::shared_ptr<D3D11Device>& device,
		const std::wstring& path,
		const D3D_SHADER_MACRO* pDefines,
		const std::string& entryPoint,
		const std::string& shaderModel)
		: ResourceBase(ResourceType::PixelShader)
	{
		create(device, path, pDefines, entryPoint, shaderModel);
	}

	void D3D11PixelShader::create(const std::shared_ptr<D3D11Device>& device,
		const std::wstring& path,
		const D3D_SHADER_MACRO* pDefines,
		const std::string& entryPoint,
		const std::string& shaderModel)
	{
		HRESULT hr = D3D11Util::CompileShaderFromFile(path.c_str(),
			pDefines,
			entryPoint.c_str(),
			shaderModel.c_str(),
			mBlob.GetAddressOf());

		hr = device->GetDevice()->CreatePixelShader(
			mBlob->GetBufferPointer(),
			mBlob->GetBufferSize(),
			nullptr,
			mShader.GetAddressOf());
	}

	std::string D3D11PixelShader::GenerateRID(ED3D11PixelShader type)
	{
		return typeid(D3D11PixelShader).name() + std::to_string(static_cast<int>(type));
	}

	void D3D11PixelShader::Bind(const std::shared_ptr<D3D11Device>& device)
	{
		device->GetDeviceContext()->PSSetShader(mShader == nullptr ? nullptr : mShader.Get(), NULL, NULL);
	}

	std::map<std::string, ED3D11SamplerState> D3D11ShaderProgram::mSamplerStateMap;
	std::map<std::string, ED3D11ConstantBufferNT> D3D11ShaderProgram::mConstnatBufferMap;

	D3D11ShaderProgram::D3D11ShaderProgram(const std::shared_ptr<D3D11Device>& device,
		std::shared_ptr<D3D11ResourceManager> resourceManager,
		std::shared_ptr<D3D11VertexShader> vertexShader,
		std::shared_ptr<D3D11GeometryShader> geometryShader,
		std::shared_ptr<D3D11PixelShader> pixelShader,
		std::shared_ptr<PipelineState> pipelineState)
		: mVertexShader(vertexShader)
		, mGeometryShader(geometryShader)
		, mPixelShader(pixelShader)
		, mPipelineState(pipelineState)
	{
		mInputLayout = std::make_shared<D3D11InputLayout>(device, mVertexShader->GetBlob().Get());
		reflect(device, ED3D11ShaderType::VertexShader, mVertexShader->GetBlob(), resourceManager);
		reflect(device, ED3D11ShaderType::GeometryShader, mGeometryShader->GetBlob(), resourceManager);
		reflect(device, ED3D11ShaderType::Pixelshader, mPixelShader->GetBlob(), resourceManager);
	}

	void D3D11ShaderProgram::SetSamplerStateMap(std::map<std::string, ED3D11SamplerState> samplerStateMap)
	{
		mSamplerStateMap = samplerStateMap;
	}

	void D3D11ShaderProgram::SetConstantBufferMap(std::map<std::string, ED3D11ConstantBufferNT> constnatBufferMap)
	{
		mConstnatBufferMap = constnatBufferMap;
	}

	void D3D11ShaderProgram::Bind(const std::shared_ptr<D3D11Device>& device)
	{
		mInputLayout->Bind(device);
		mVertexShader->Bind(device);
		mGeometryShader->Bind(device);
		mPixelShader->Bind(device);
		mPipelineState->Bind(device);

		for (const auto& [key, data] : mConstantBuffers)
		{
			data.first->Bind(device, key.second, data.second);
		}
		for (const auto& [key, data] : mSamplers)
		{
			data.first->Bind(device, data.second, key.second);
		}
	}

	void D3D11ShaderProgram::reflect(const std::shared_ptr<D3D11Device>& device, ED3D11ShaderType shaderType, Microsoft::WRL::ComPtr<ID3D10Blob> blob, std::shared_ptr<D3D11ResourceManager> resourceManager)
	{
		if (blob == nullptr)
		{
			return;
		}

		ID3D11ShaderReflection* pReflector = nullptr;

		D3DReflect(blob->GetBufferPointer(),
			blob->GetBufferSize(),
			IID_ID3D11ShaderReflection,
			(void**)&pReflector);

		D3D11_SHADER_DESC shaderDesc;
		pReflector->GetDesc(&shaderDesc);

		for (unsigned int cbindex = 0; cbindex < shaderDesc.ConstantBuffers; cbindex++)
		{
			ID3D11ShaderReflectionConstantBuffer* cBuffer = pReflector->GetConstantBufferByIndex(cbindex);
			D3D11_SHADER_BUFFER_DESC bufferDesc;

			if (SUCCEEDED(cBuffer->GetDesc(&bufferDesc)))
			{
				CD3D11_BUFFER_DESC cBufferDesc(bufferDesc.Size, D3D11_BIND_CONSTANT_BUFFER);

				D3D11_SHADER_INPUT_BIND_DESC bindDesc;
				pReflector->GetResourceBindingDescByName(bufferDesc.Name, &bindDesc);

				UINT registerSlot = bindDesc.BindPoint;

				auto find = mConstnatBufferMap.find(bufferDesc.Name);
				assert(find != mConstnatBufferMap.end());
				ED3D11ConstantBufferNT type = find->second;
				std::shared_ptr<D3D11ConstantBufferNT> cb = resourceManager->Create<D3D11ConstantBufferNT>(type, cBufferDesc);

				mConstantBuffers.insert({ { find->second, shaderType }, {cb, bindDesc.BindPoint} });
				nConstnatBufferNTs.insert({ bufferDesc.Name, { shaderType, bindDesc.BindPoint, cb } });
			}
		}

		for (unsigned int rsindex = 0; rsindex < shaderDesc.BoundResources; rsindex++)
		{
			D3D11_SHADER_INPUT_BIND_DESC bindDesc;
			pReflector->GetResourceBindingDesc(rsindex, &bindDesc);

			// 현재 리소스가 Texture라면 리스트에 삽입..
			switch (bindDesc.Type)
			{
			case D3D_SIT_SAMPLER:
			{
				auto find = mSamplerStateMap.find(bindDesc.Name);
				assert(find != mSamplerStateMap.end());

				ED3D11SamplerState samplerType = find->second;
				std::shared_ptr<D3D11SamplerState> samplerState = resourceManager->Create<D3D11SamplerState>(samplerType);
				mSamplers.insert({ { samplerType, shaderType}, { samplerState, bindDesc.BindPoint} });

				break;
			}
			case D3D_SIT_TEXTURE:
				break;
			case D3D_SIT_UAV_RWTYPED:
				break;
			default:
				break;
			}
		}
	}
}

