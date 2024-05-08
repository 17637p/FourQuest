#include "D3D11Shader.h"
#include "D3D11Util.h"
#include "D3D11Device.h"

namespace fq::graphics
{
	D3D11VertexShader::D3D11VertexShader(const std::shared_ptr<D3D11Device>& device,
		const std::wstring& path,
		const D3D_SHADER_MACRO* pDefines,
		const std::string& entryPoint,
		const std::string& shaderModel)
		: mPath(path)
		, mBlob(nullptr)
		, mShader(nullptr)
	{
		HRESULT hr = D3D11Util::CompileShaderFromFile(path.c_str(),
			pDefines,
			entryPoint.c_str(),
			shaderModel.c_str(),
			mBlob.GetAddressOf());

		hr = device->GetDevice()->CreateVertexShader(
			mBlob->GetBufferPointer(),
			mBlob->GetBufferSize(),
			nullptr,
			mShader.GetAddressOf());
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

	void D3D11GeometryShader::Bind(const std::shared_ptr<D3D11Device>& device)
	{
		device->GetDeviceContext()->GSSetShader(mShader.Get(), NULL, NULL);
	}

	D3D11PixelShader::D3D11PixelShader(const std::shared_ptr<D3D11Device>& device,
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
	void D3D11PixelShader::Bind(const std::shared_ptr<D3D11Device>& device)
	{
		device->GetDeviceContext()->PSSetShader(mShader.Get(), NULL, NULL);
	}
}

