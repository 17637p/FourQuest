#include "D3D11Texture.h"

#include <directxtk/DDSTextureLoader.h>
#include <directxtk/WiCTextureLoader.h>

#include "D3D11Device.h"
#include "Define.h"

fq::graphics::D3D11Texture::D3D11Texture(const std::shared_ptr<D3D11ResourceManager>& resourceManager, const std::shared_ptr<D3D11Device>& d3d11Device, const std::wstring& texturePath)
	:ResourceBase(resourceManager, ResourceType::Texture)
{
	std::wstring fileExtension = texturePath.substr(texturePath.find_last_of(L".") + 1, texturePath.length() - texturePath.find_last_of(L".") + 1);

	if (fileExtension == L"dds")
	{
		HR(DirectX::CreateDDSTextureFromFile(d3d11Device->GetDevice().Get(),
			d3d11Device->GetDeviceContext().Get(),
			texturePath.c_str(),
			nullptr, mTextureSRV.GetAddressOf()));
	}
	else if(fileExtension == L"jpg" || fileExtension == L"png" || fileExtension == L"tiff")
	{
		HR(DirectX::CreateWICTextureFromFile(d3d11Device->GetDevice().Get(),
			d3d11Device->GetDeviceContext().Get(),
			texturePath.c_str(),
			nullptr, mTextureSRV.GetAddressOf()));
	}
	else
	{
		MessageBox(NULL, L"텍스처를 생성할 수 없습니다. 텍스처의 파일 확장자가 dds, jpg, png, tiff 외에 다른 파일입니다. 프로그래머한테 문의 주세요~", L"에러", MB_ICONERROR);
	}
}

std::string fq::graphics::D3D11Texture::GenerateRID(const std::wstring& texturePath)
{
	std::string temp;
	temp.assign(texturePath.begin(), texturePath.end());

	return typeid(D3D11Texture).name() + temp;
}

void fq::graphics::D3D11Texture::Bind(const std::shared_ptr<D3D11Device>& d3d11Device, const UINT startSlot, const ED3D11ShaderType eShaderType)
{
	switch (eShaderType)
	{
		case ED3D11ShaderType::VertexShader:
		{
			d3d11Device->GetDeviceContext()->VSSetShaderResources(startSlot, 1, mTextureSRV.GetAddressOf());
			break;
		}
		case ED3D11ShaderType::Pixelshader:
		{
			d3d11Device->GetDeviceContext()->PSSetShaderResources(startSlot, 1, mTextureSRV.GetAddressOf());
			break;
		}
		case ED3D11ShaderType::GeometryShader:
		{
			d3d11Device->GetDeviceContext()->GSSetShaderResources(startSlot, 1, mTextureSRV.GetAddressOf());
			break;
		}
		default:
			break;
	}
}
