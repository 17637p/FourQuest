#include "D3D11LightProbeManager.h"

#include <DirectXTex.h>

#include "d3d11resourceManager.h"
#include "D3D11View.h"
#include "D3D11Device.h"

using namespace fq::graphics;

fq::graphics::D3D11LightProbeManager::D3D11LightProbeManager()
	:mCubeProbes{}
{

}

fq::graphics::D3D11LightProbeManager::~D3D11LightProbeManager()
{

}

unsigned short fq::graphics::D3D11LightProbeManager::AddCubeProbe(const DirectX::SimpleMath::Vector3& position)
{
	unsigned short probeIndex = mCubeProbes.size();
	CubeProbe* newCubeProbe = new CubeProbe;
	newCubeProbe->position = position;
	mCubeProbes[probeIndex] = newCubeProbe;

	return probeIndex;
}

void fq::graphics::D3D11LightProbeManager::DeleteCubeProbe(unsigned short index)
{
	delete mCubeProbes[index];
	mCubeProbes.erase(index);
}

void fq::graphics::D3D11LightProbeManager::SaveCubeProbeTexture()
{
	HRESULT hr = S_OK;

	// Get the back buffer texture
	ID3D11Resource* pBackBuffer;
	std::shared_ptr<D3D11RenderTargetView> backBufferRTV =
		mResourceManager->Get<D3D11RenderTargetView>(ED3D11RenderTargetViewType::Offscreen);
	backBufferRTV->GetRTV()->GetResource(&pBackBuffer);

	// Create texture copy
	D3D11_TEXTURE2D_DESC desc;
	ID3D11Texture2D* pTexture;
	pBackBuffer->QueryInterface<ID3D11Texture2D>(&pTexture);
	pTexture->GetDesc(&desc);

	// Ensure the texture is set up for reading
	D3D11_TEXTURE2D_DESC descCopy = desc;
	descCopy.Usage = D3D11_USAGE_STAGING;
	descCopy.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	descCopy.BindFlags = 0;

	ID3D11Texture2D* pStagingTexture;
	hr = mDevice->GetDevice()->CreateTexture2D(&descCopy, nullptr, &pStagingTexture);
	//if (FAILED(hr))
	//	return hr;

	// Copy texture data from GPU to CPU
	mDevice->GetDeviceContext()->CopyResource(pStagingTexture, pTexture);

	// Save texture to DDS file
	DirectX::TexMetadata metadata;
	DirectX::ScratchImage image;
	hr = CaptureTexture(mDevice->GetDevice().Get(), mDevice->GetDeviceContext().Get(), pStagingTexture, image);
	//if (FAILED(hr))
	//	return hr;

	static int index = 0;

	std::wstring fileName = L"test" + std::to_wstring(index) + L".DDS";
	//const wchar_t* filename = L"test.DDS";
	hr = SaveToDDSFile(*image.GetImage(0, 0, 0), DirectX::DDS_FLAGS_NONE, fileName.c_str());
	//if (FAILED(hr))
	//	return hr;

	index++;
}

std::unordered_map<unsigned short, CubeProbe*> fq::graphics::D3D11LightProbeManager::GetCubeProbes() const
{
	return mCubeProbes;
}

void D3D11LightProbeManager::Initialize(std::shared_ptr<D3D11Device> device, std::shared_ptr<D3D11ResourceManager> resourceManager)
{
	mDevice = device;
	mResourceManager = resourceManager;
}

