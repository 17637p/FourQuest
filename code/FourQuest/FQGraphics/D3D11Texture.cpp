#include "D3D11Texture.h"

#include <directxtk/DDSTextureLoader.h>
#include <directxtk/WiCTextureLoader.h>
#include <filesystem>
#include <spdlog/spdlog.h>
#include <DirectXPackedVector.h>

#include <DirectXTex.h>

#include "D3D11Device.h"
#include "Define.h"

fq::graphics::D3D11Texture::D3D11Texture(const std::shared_ptr<D3D11Device>& d3d11Device, const std::wstring& texturePath)
	:ResourceBase(ResourceType::Texture)
{
	std::wstring fileExtension = texturePath.substr(texturePath.find_last_of(L".") + 1, texturePath.length() - texturePath.find_last_of(L".") + 1);

	if (!std::filesystem::exists(texturePath))
	{
		spdlog::warn("[D3D11Texture] \"{}\" not exist", std::filesystem::path(texturePath).string());
	}

	if (fileExtension == L"dds")
	{
		HR(DirectX::CreateDDSTextureFromFile(d3d11Device->GetDevice().Get(),
			d3d11Device->GetDeviceContext().Get(),
			texturePath.c_str(),
			nullptr, mTextureSRV.GetAddressOf()));
	}
	else if (fileExtension == L"jpg" || fileExtension == L"png" || fileExtension == L"tiff" || fileExtension == L"gif")
	{
		HR(DirectX::CreateWICTextureFromFile(d3d11Device->GetDevice().Get(),
			d3d11Device->GetDeviceContext().Get(),
			texturePath.c_str(),
			nullptr, mTextureSRV.GetAddressOf()));
	}
	else
	{
		MessageBox(NULL, L"텍스처를 생성할 수 없습니다. 텍스처의 파일 확장자가 dds, jpg, png, tiff, gif 외에 다른 파일입니다. 프로그래머한테 문의 주세요~", L"에러", MB_ICONERROR);
	}
}

fq::graphics::D3D11Texture::D3D11Texture(const std::shared_ptr<D3D11Device>& d3d11Device,
	const std::vector<float>& rawArray,
	const UINT width, const UINT height)
	:ResourceBase(ResourceType::Texture)
{
	D3D11_TEXTURE2D_DESC texDesc;
	texDesc.Width = width;
	texDesc.Height = height;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R16_FLOAT;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;

	std::vector<DirectX::PackedVector::HALF> hmap(rawArray.size());
	std::transform(rawArray.begin(), rawArray.end(), hmap.begin(), DirectX::PackedVector::XMConvertFloatToHalf);

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = &hmap[0];
	data.SysMemPitch = width * sizeof(DirectX::PackedVector::HALF);
	data.SysMemSlicePitch = 0;

	ID3D11Texture2D* hmapTex = 0;
	d3d11Device->GetDevice()->CreateTexture2D(&texDesc, &data, &hmapTex);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = texDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = -1;
	d3d11Device->GetDevice()->CreateShaderResourceView(hmapTex, &srvDesc, &mTextureSRV);

	hmapTex->Release();
}

fq::graphics::D3D11Texture::D3D11Texture(const std::shared_ptr<D3D11Device>& d3d11Device, const std::vector<std::wstring>& texturePaths)
	:ResourceBase(ResourceType::Texture)
{
	ID3D11Texture2D* cubeTexture = NULL;
	ID3D11ShaderResourceView* shaderResourceView = NULL;

	//Description of each face
	D3D11_TEXTURE2D_DESC texDesc = {};

	D3D11_TEXTURE2D_DESC cubeTexDesc = {};
	//The Shader Resource view description
	D3D11_SHADER_RESOURCE_VIEW_DESC SMViewDesc = {};

	ID3D11Texture2D* tex[6] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
	for (int i = 0; i < 6; i++)
	{
		HRESULT hr = DirectX::CreateDDSTextureFromFileEx(d3d11Device->GetDevice().Get(), texturePaths[i].c_str(), 0, D3D11_USAGE_STAGING, 0, D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE, 0,
			DirectX::DDS_LOADER_FLAGS::DDS_LOADER_DEFAULT,
			(ID3D11Resource**)&tex[i], 0);
		assert(SUCCEEDED(hr));
	}

	tex[0]->GetDesc(&cubeTexDesc);

	texDesc.Width = cubeTexDesc.Width;
	texDesc.Height = cubeTexDesc.Height;
	texDesc.MipLevels = cubeTexDesc.MipLevels;
	texDesc.ArraySize = 6;
	texDesc.Format = cubeTexDesc.Format;
	texDesc.CPUAccessFlags = 0;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

	SMViewDesc.Format = texDesc.Format;
	SMViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	SMViewDesc.TextureCube.MipLevels = texDesc.MipLevels;
	SMViewDesc.TextureCube.MostDetailedMip = 0;

	d3d11Device->GetDevice()->CreateTexture2D(&texDesc, NULL, &cubeTexture);
	for (int i = 0; i < 6; i++)
	{
		for (UINT mipLevel = 0; mipLevel < texDesc.MipLevels; ++mipLevel)
		{
			D3D11_MAPPED_SUBRESOURCE mappedTex2D;
			HRESULT hr = (d3d11Device->GetDeviceContext()->Map(tex[i], mipLevel, D3D11_MAP_READ, 0, &mappedTex2D));
			assert(SUCCEEDED(hr));
			d3d11Device->GetDeviceContext()->UpdateSubresource(cubeTexture,
				D3D11CalcSubresource(mipLevel, i, texDesc.MipLevels),
				0, mappedTex2D.pData, mappedTex2D.RowPitch, mappedTex2D.DepthPitch);

			d3d11Device->GetDeviceContext()->Unmap(tex[i], mipLevel);
		}
	}

	for (int i = 0; i < 6; i++)
	{
		tex[i]->Release();
	}

	d3d11Device->GetDevice()->CreateShaderResourceView(cubeTexture, &SMViewDesc, mTextureSRV.GetAddressOf());

	//--------------------------------------------------------------
	ID3D11Resource* resource;
	mTextureSRV->GetResource(&resource);

	// Query the resource for the ID3D11Texture2D interface
	ID3D11Texture2D* cubeMapTexture;
	//if (FAILED(hr))
	//	return hr;
	mTextureSRV->GetResource(&resource);

	// Get the description of the texture
	D3D11_TEXTURE2D_DESC desc;
	resource->QueryInterface<ID3D11Texture2D>(&cubeMapTexture);
	cubeMapTexture->GetDesc(&desc);

	// Ensure the texture is indeed a cube map
	if (!(desc.MiscFlags & D3D11_RESOURCE_MISC_TEXTURECUBE))
	{
		// 경고
	}
	//	return E_FAIL;

	// Capture the texture into a ScratchImage
	DirectX::ScratchImage scratchImage;
	CaptureTexture(d3d11Device->GetDevice().Get(), d3d11Device->GetDeviceContext().Get(), cubeMapTexture, scratchImage);
	//if (FAILED(hr))
	//	return hr;

	// Save the ScratchImage to a DDS file
	SaveToDDSFile(scratchImage.GetImages(), scratchImage.GetImageCount(), scratchImage.GetMetadata(), DirectX::DDS_FLAGS_NONE, L"TestTest.dds");
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
		case ED3D11ShaderType::PixelShader:
		{
			d3d11Device->GetDeviceContext()->PSSetShaderResources(startSlot, 1, mTextureSRV.GetAddressOf());
			break;
		}
		case ED3D11ShaderType::GeometryShader:
		{
			d3d11Device->GetDeviceContext()->GSSetShaderResources(startSlot, 1, mTextureSRV.GetAddressOf());
			break;
		}
		case ED3D11ShaderType::DomainShader:
		{
			d3d11Device->GetDeviceContext()->DSSetShaderResources(startSlot, 1, mTextureSRV.GetAddressOf());
			break;
		}
		default:
			break;
	}
}

void fq::graphics::D3D11Texture::Bind(const std::shared_ptr<D3D11Device>& d3d11Device, std::vector<std::shared_ptr<D3D11Texture>> textures, const UINT startSlot, const ED3D11ShaderType eShaderType)
{
	unsigned short numOfTexture = textures.size();
	std::vector<ID3D11ShaderResourceView*> textureSRVs;
	textureSRVs.reserve(numOfTexture);
	for (const auto& texture : textures)
	{
		textureSRVs.push_back(texture->mTextureSRV.Get());
	}

	switch (eShaderType)
	{
		case ED3D11ShaderType::VertexShader:
		{
			d3d11Device->GetDeviceContext()->VSSetShaderResources(startSlot, numOfTexture, textureSRVs.data());
			break;
		}
		case ED3D11ShaderType::PixelShader:
		{
			d3d11Device->GetDeviceContext()->PSSetShaderResources(startSlot, numOfTexture, textureSRVs.data());
			break;
		}
		case ED3D11ShaderType::GeometryShader:
		{
			d3d11Device->GetDeviceContext()->GSSetShaderResources(startSlot, numOfTexture, textureSRVs.data());
			break;
		}
		default:
			break;
	}
}
