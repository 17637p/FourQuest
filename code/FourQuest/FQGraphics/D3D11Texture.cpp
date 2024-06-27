#include "D3D11Texture.h"

#include <directxtk/DDSTextureLoader.h>
#include <directxtk/WiCTextureLoader.h>
#include <filesystem>
#include <spdlog/spdlog.h>
#include <DirectXPackedVector.h>

#include "D3D11Device.h"
#include "Define.h"

fq::graphics::D3D11Texture::D3D11Texture(const std::shared_ptr<D3D11Device>& d3d11Device, const std::wstring& texturePath)
	:ResourceBase(ResourceType::Texture)
	, mTexture(nullptr)
	, mSRV(nullptr)
	, mUAV(nullptr)
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
			nullptr, mSRV.GetAddressOf()));
	}
	else if (fileExtension == L"jpg" || fileExtension == L"png" || fileExtension == L"tiff" || fileExtension == L"gif")
	{
		HR(DirectX::CreateWICTextureFromFile(d3d11Device->GetDevice().Get(),
			d3d11Device->GetDeviceContext().Get(),
			texturePath.c_str(),
			nullptr, mSRV.GetAddressOf()));
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
	, mTexture(nullptr)
	, mSRV(nullptr)
	, mUAV(nullptr)
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
	d3d11Device->GetDevice()->CreateTexture2D(&texDesc, &data, mTexture.GetAddressOf());

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = texDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = -1;
	d3d11Device->GetDevice()->CreateShaderResourceView(mTexture.Get(), &srvDesc, mSRV.GetAddressOf());
}

fq::graphics::D3D11Texture::D3D11Texture(const std::shared_ptr<D3D11Device>& d3d11Device, UINT width, UINT height, DXGI_FORMAT format, UINT levels)
	: ResourceBase(ResourceType::Texture)
	, mTexture(nullptr)
	, mSRV(nullptr)
	, mUAV(nullptr)
	, mWidth(width)
	, mHeight(height)
	, mLevels(levels)
{
	if (levels == 0)
	{
		levels = 1;
		while ((width | height) >> levels) {
			++levels;
		}
	}

	D3D11_TEXTURE2D_DESC desc = {};
	desc.Width = width;
	desc.Height = height;
	desc.MipLevels = levels;
	desc.ArraySize = 1;
	desc.Format = format;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	if (levels == 0) {
		desc.BindFlags |= D3D11_BIND_RENDER_TARGET;
		desc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
	}

	HR(d3d11Device->GetDevice()->CreateTexture2D(&desc, nullptr, mTexture.GetAddressOf()));

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = desc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = -1;
	HR(d3d11Device->GetDevice()->CreateShaderResourceView(mTexture.Get(), &srvDesc, mSRV.GetAddressOf()));
}

std::string fq::graphics::D3D11Texture::GenerateRID(const std::wstring& texturePath)
{
	std::string temp;
	temp.assign(texturePath.begin(), texturePath.end());

	return typeid(D3D11Texture).name() + temp;
}

void fq::graphics::D3D11Texture::CreateUAV(const std::shared_ptr<D3D11Device>& d3d11Device, UINT mipSlice)
{
	assert(mTexture != nullptr);

	D3D11_TEXTURE2D_DESC desc;
	mTexture->GetDesc(&desc);

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
	uavDesc.Format = desc.Format;
	if (desc.ArraySize == 1) {
		uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
		uavDesc.Texture2D.MipSlice = mipSlice;
	}
	else {
		uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2DARRAY;
		uavDesc.Texture2DArray.MipSlice = mipSlice;
		uavDesc.Texture2DArray.FirstArraySlice = 0;
		uavDesc.Texture2DArray.ArraySize = desc.ArraySize;
	}

	HR(d3d11Device->GetDevice()->CreateUnorderedAccessView(mTexture.Get(), &uavDesc, mUAV.GetAddressOf()));
}

void fq::graphics::D3D11Texture::Bind(const std::shared_ptr<D3D11Device>& d3d11Device, const UINT startSlot, const ED3D11ShaderType eShaderType)
{
	switch (eShaderType)
	{
	case ED3D11ShaderType::VertexShader:
	{
		d3d11Device->GetDeviceContext()->VSSetShaderResources(startSlot, 1, mSRV.GetAddressOf());
		break;
	}
	case ED3D11ShaderType::PixelShader:
	{
		d3d11Device->GetDeviceContext()->PSSetShaderResources(startSlot, 1, mSRV.GetAddressOf());
		break;
	}
	case ED3D11ShaderType::GeometryShader:
	{
		d3d11Device->GetDeviceContext()->GSSetShaderResources(startSlot, 1, mSRV.GetAddressOf());
		break;
	}
	case ED3D11ShaderType::DomainShader:
	{
		d3d11Device->GetDeviceContext()->DSSetShaderResources(startSlot, 1, mSRV.GetAddressOf());
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
		textureSRVs.push_back(texture->mSRV.Get());
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

fq::graphics::D3D11CubeTexture::D3D11CubeTexture(const std::shared_ptr<D3D11Device>& d3d11Device, const std::wstring& texturePath)
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
			(ID3D11Resource**)mTexture.GetAddressOf(),
			mSRV.GetAddressOf()));
	}
	else if (fileExtension == L"jpg" || fileExtension == L"png" || fileExtension == L"tiff" || fileExtension == L"gif")
	{
		HR(DirectX::CreateWICTextureFromFile(d3d11Device->GetDevice().Get(),
			d3d11Device->GetDeviceContext().Get(),
			texturePath.c_str(),
			(ID3D11Resource**)mTexture.GetAddressOf(),
			mSRV.GetAddressOf()));
	}
	else
	{
		MessageBox(NULL, L"텍스처를 생성할 수 없습니다. 텍스처의 파일 확장자가 dds, jpg, png, tiff, gif 외에 다른 파일입니다. 프로그래머한테 문의 주세요~", L"에러", MB_ICONERROR);
	}

	D3D11_TEXTURE2D_DESC desc;
	mTexture->GetDesc(&desc);

	mWidth = desc.Width;
	mHeight = desc.Height;
	mLevels = desc.MipLevels;
}

fq::graphics::D3D11CubeTexture::D3D11CubeTexture(const std::shared_ptr<D3D11Device>& d3d11Device, UINT width, UINT height, DXGI_FORMAT format, UINT levels)
	:ResourceBase(ResourceType::Texture)
	, mWidth(width)
	, mHeight(height)
	, mLevels(-1)
	, mTexture(nullptr)
	, mUAV(nullptr)
	, mSRV(nullptr)
{
	if (levels == 0)
	{
		levels = 1;
		while ((width | height) >> levels) {
			++levels;
		}
	}

	mLevels = levels;

	D3D11_TEXTURE2D_DESC desc = {};
	desc.Width = width;
	desc.Height = height;
	desc.MipLevels = levels;
	desc.ArraySize = 6;
	desc.Format = format;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	desc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
	if (levels == 0) {
		desc.BindFlags |= D3D11_BIND_RENDER_TARGET;
		desc.MiscFlags |= D3D11_RESOURCE_MISC_GENERATE_MIPS;
	}

	HR(d3d11Device->GetDevice()->CreateTexture2D(&desc, nullptr, mTexture.GetAddressOf()));
	
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = desc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	srvDesc.TextureCube.MostDetailedMip = 0;
	srvDesc.TextureCube.MipLevels = -1;

	HR(d3d11Device->GetDevice()->CreateShaderResourceView(mTexture.Get(), &srvDesc, mSRV.GetAddressOf()));
}

std::string fq::graphics::D3D11CubeTexture::GenerateRID(const std::wstring& texturePath)
{
	std::string temp;
	temp.assign(texturePath.begin(), texturePath.end());

	return typeid(D3D11Texture).name() + temp;
}

void fq::graphics::D3D11CubeTexture::CreateUAV(const std::shared_ptr<D3D11Device>& d3d11Device, UINT mipSlice)
{
	assert(mTexture != nullptr);

	mUAV.Reset();

	D3D11_TEXTURE2D_DESC desc;
	mTexture->GetDesc(&desc);

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
	uavDesc.Format = desc.Format;

	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2DARRAY;
	uavDesc.Texture2DArray.MipSlice = mipSlice;
	uavDesc.Texture2DArray.FirstArraySlice = 0;
	uavDesc.Texture2DArray.ArraySize = desc.ArraySize;

	HR(d3d11Device->GetDevice()->CreateUnorderedAccessView(mTexture.Get(), &uavDesc, mUAV.GetAddressOf()));
}
