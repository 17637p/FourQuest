#include "D3D11Texture.h"

#include <directxtk/DDSTextureLoader.h>
#include <directxtk/WiCTextureLoader.h>
#include <filesystem>
#include <spdlog/spdlog.h>
#include <DirectXPackedVector.h>

#include <DirectXTex.h>
#include <../FQCommon/StringUtil.h>

#ifdef max
#undef max
#undef min

#define IMATH_HALF_NO_LOOKUP_TABLE
#include <Imath/half.h>
#include <OpenEXR/ImfRgbaFile.h>
#include <OpenEXR/ImfArray.h>
#include <Imath/ImathBox.h>

#endif // max

#include "D3D11Device.h"
#include "Define.h"

#include "SHMath/DirectXSH.h"

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

	type = TextureType::Default;
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

	type = TextureType::Default;
}

fq::graphics::D3D11Texture::D3D11Texture(const std::shared_ptr<D3D11Device>& d3d11Device, const std::vector<std::wstring>& texturePaths)
	:ResourceBase(ResourceType::Texture)
{
	ID3D11Texture2D* cubeTexture = NULL;

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

	d3d11Device->GetDevice()->CreateShaderResourceView(cubeTexture, &SMViewDesc, mSRV.GetAddressOf());

	cubeTexture->Release();

	type = TextureType::CubeMap;
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
	else if (fileExtension == L"exr")
	{

	}
	else
	{
		MessageBox(NULL, L"텍스처를 생성할 수 없습니다. 텍스처의 파일 확장자가 dds, jpg, png, tiff, gif 외에 다른 파일입니다. 프로그래머한테 문의 주세요~", L"에러", MB_ICONERROR);
	}

	if (mTexture != nullptr)
	{
		D3D11_TEXTURE2D_DESC desc;
		mTexture->GetDesc(&desc);

		mWidth = desc.Width;
		mHeight = desc.Height;
		mLevels = desc.MipLevels;
	}
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

void fq::graphics::D3D11Texture::Save(const std::shared_ptr<D3D11Device>& d3d11Device, const std::wstring& savePath)
{
	switch (type)
	{
	case fq::graphics::D3D11Texture::TextureType::Default:
		break;
	case fq::graphics::D3D11Texture::TextureType::CubeMap:
	{
		ID3D11Resource* resource;
		mSRV->GetResource(&resource);

		// Query the resource for the ID3D11Texture2D interface
		ID3D11Texture2D* cubeMapTexture;
		//if (FAILED(hr))
		//	return hr;
		//mTextureSRV->GetResource(&resource);

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
		if (scratchImage.GetMetadata().format != DXGI_FORMAT_R16G16B16A16_FLOAT)
		{
			DirectX::ScratchImage float16Image;
			Convert(scratchImage.GetImages(), scratchImage.GetImageCount(), scratchImage.GetMetadata(), DXGI_FORMAT_R16G16B16A16_FLOAT, DirectX::TEX_FILTER_DEFAULT, DirectX::TEX_THRESHOLD_DEFAULT, float16Image);
			SaveToDDSFile(float16Image.GetImages(), float16Image.GetImageCount(), float16Image.GetMetadata(), DirectX::DDS_FLAGS_NONE, savePath.c_str());

			float16Image.Release();
		}
		else
		{
			SaveToDDSFile(scratchImage.GetImages(), scratchImage.GetImageCount(), scratchImage.GetMetadata(), DirectX::DDS_FLAGS_NONE, savePath.c_str());
		}
		//if (FAILED(hr))
		//	return hr;

		// Save the ScratchImage to a DDS file

		resource->Release();
		cubeMapTexture->Release();
		scratchImage.Release();
	}
	break;
	default:
		break;
	}
}

void fq::graphics::D3D11Texture::GetSHCoefficientRGB(const std::shared_ptr<D3D11Device>& d3d11Device, float* R, float* G, float* B)
{
	//if (type == TextureType::Default)
	//{
	//	return;
	//}

	ID3D11Resource* resource = nullptr;
	mSRV->GetResource(&resource);
	assert(resource);

	// Query the resource for the ID3D11Texture2D interface
	ID3D11Texture2D* cubeMapTexture = nullptr;
	resource->QueryInterface<ID3D11Texture2D>(&cubeMapTexture);
	assert(cubeMapTexture);

	DirectX::SHProjectCubeMap(d3d11Device->GetDeviceContext().Get(), 3, cubeMapTexture, R, G, B);

	resource->Release();
	cubeMapTexture->Release();

	return;
}

fq::graphics::D3D11TextureArray::D3D11TextureArray(const std::shared_ptr<D3D11Device>& d3d11Device, const std::vector<std::filesystem::path>& texturePaths)
{
	const UINT TEXTURE_COUNT = texturePaths.size();
	std::vector<ID3D11Texture2D*> srcTextures(TEXTURE_COUNT);

	for (UINT i = 0; i < texturePaths.size(); ++i) {
		const std::wstring& texturePath = texturePaths[i];
		std::wstring fileExtension = texturePath.substr(texturePath.find_last_of(L".") + 1, texturePath.length() - texturePath.find_last_of(L".") + 1);

		if (!std::filesystem::exists(texturePath))
		{
			spdlog::warn("[D3D11Texture] \"{}\" not exist", std::filesystem::path(texturePath).string());
		}

		if (fileExtension == L"dds")
		{
			HR(DirectX::CreateDDSTextureFromFileEx(
				d3d11Device->GetDevice().Get(),
				texturePath.c_str(),
				0,
				D3D11_USAGE_STAGING,
				0,
				D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ,
				0,
				DirectX::DDS_LOADER_DEFAULT,
				reinterpret_cast<ID3D11Resource**>(&srcTextures[i]),
				nullptr
			));
		}
		else if (fileExtension == L"jpg" || fileExtension == L"png" || fileExtension == L"tiff" || fileExtension == L"gif")
		{
			HR(DirectX::CreateWICTextureFromFileEx(
				d3d11Device->GetDevice().Get(),
				texturePath.c_str(),
				0,
				D3D11_USAGE_STAGING,
				0,
				D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ,
				0,
				DirectX::DX11::WIC_LOADER_DEFAULT,
				reinterpret_cast<ID3D11Resource**>(&srcTextures[i]),
				nullptr
			));
		}
		else if (fileExtension == L"exr")
		{
			int width;
			int height;
			std::vector<float> rawArray = loadEXR(texturePath, &width, &height);

			D3D11_TEXTURE2D_DESC texDesc;
			texDesc.Width = width;
			texDesc.Height = height;
			texDesc.MipLevels = 1;
			texDesc.ArraySize = 1;
			texDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
			texDesc.SampleDesc.Count = 1;
			texDesc.SampleDesc.Quality = 0;
			texDesc.Usage = D3D11_USAGE_STAGING;
			texDesc.BindFlags = 0;
			texDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
			texDesc.MiscFlags = 0;

			std::vector<DirectX::PackedVector::HALF> hmap(rawArray.size());
			std::transform(rawArray.begin(), rawArray.end(), hmap.begin(), DirectX::PackedVector::XMConvertFloatToHalf);

			D3D11_SUBRESOURCE_DATA data;
			data.pSysMem = &hmap[0];
			data.SysMemPitch = width * 4 * sizeof(DirectX::PackedVector::HALF);
			data.SysMemSlicePitch = 0;

			ID3D11Texture2D* hmapTex = 0;
			HR(d3d11Device->GetDevice()->CreateTexture2D(&texDesc, &data, &srcTextures[i]));
		}
		else
		{
			MessageBox(NULL, L"텍스처를 생성할 수 없습니다. 텍스처의 파일 확장자가 dds, jpg, png, tiff, gif 외에 다른 파일입니다. 프로그래머한테 문의 주세요~", L"에러", MB_ICONERROR);
		}
	}

	D3D11_TEXTURE2D_DESC textureDesc;
	srcTextures[0]->GetDesc(&textureDesc);

	D3D11_TEXTURE2D_DESC texArrayDesc = {};
	texArrayDesc.Width = textureDesc.Width;
	texArrayDesc.Height = textureDesc.Height;
	texArrayDesc.MipLevels = textureDesc.MipLevels;
	texArrayDesc.ArraySize = TEXTURE_COUNT;
	texArrayDesc.Format = textureDesc.Format;
	texArrayDesc.SampleDesc.Count = 1;
	texArrayDesc.Usage = D3D11_USAGE_DEFAULT;
	texArrayDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	ID3D11Texture2D* texArray = nullptr;
	HR(d3d11Device->GetDevice()->CreateTexture2D(&texArrayDesc, nullptr, &texArray));

	for (UINT i = 0; i < TEXTURE_COUNT; ++i) {
		for (UINT mipLevel = 0; mipLevel < textureDesc.MipLevels; ++mipLevel) {
			D3D11_MAPPED_SUBRESOURCE mappedResource;
			HR(d3d11Device->GetDeviceContext()->Map(srcTextures[i], mipLevel, D3D11_MAP_READ, 0, &mappedResource));

			d3d11Device->GetDeviceContext()->UpdateSubresource(
				texArray,
				D3D11CalcSubresource(mipLevel, i, textureDesc.MipLevels),
				nullptr,
				mappedResource.pData,
				mappedResource.RowPitch,
				mappedResource.DepthPitch
			);

			d3d11Device->GetDeviceContext()->Unmap(srcTextures[i], mipLevel);
		}
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = texArrayDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	srvDesc.Texture2DArray.MostDetailedMip = 0;
	srvDesc.Texture2DArray.MipLevels = texArrayDesc.MipLevels;
	srvDesc.Texture2DArray.FirstArraySlice = 0;
	srvDesc.Texture2DArray.ArraySize = TEXTURE_COUNT;

	assert(texArray != nullptr);
	HR(d3d11Device->GetDevice()->CreateShaderResourceView(texArray, &srvDesc, mSRV.GetAddressOf()));

	texArray->Release();

	for (UINT i = 0; i < TEXTURE_COUNT; ++i) {
		srcTextures[i]->Release();
	}
}

void fq::graphics::D3D11TextureArray::Bind(const std::shared_ptr<D3D11Device>& d3d11Device, const UINT startSlot, const ED3D11ShaderType eShaderType)
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

std::vector<float> fq::graphics::D3D11TextureArray::loadEXR(const std::filesystem::path& texturePath, int* outWidth, int* outHeight)
{
	std::string filename = fq::common::StringUtil::ToMultiByte(texturePath.wstring());
	Imf::RgbaInputFile file(filename.c_str());
	Imath::Box2i dw = file.dataWindow();

	const int& width = *outWidth = dw.max.x - dw.min.x + 1;
	const int& height = *outHeight = dw.max.y - dw.min.y + 1;

	Imf::Array2D<Imf::Rgba> pixels(height, width);
	file.setFrameBuffer(&pixels[0][0], 1, width);
	file.readPixels(dw.min.y, dw.max.y);

	std::vector<float> result(width * height * 4);

	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			Imf::Rgba& pixel = pixels[y][x];
			result[4 * (y * width + x) + 0] = pixel.r;
			result[4 * (y * width + x) + 1] = pixel.g;
			result[4 * (y * width + x) + 2] = pixel.b;
			result[4 * (y * width + x) + 3] = pixel.a;
		}
	}

	return result;
}
