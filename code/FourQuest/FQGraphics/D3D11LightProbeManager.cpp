#include "D3D11LightProbeManager.h"

#include <DirectXTex.h>
#include <directxtk/DDSTextureLoader.h>

#include "d3d11resourceManager.h"
#include "D3D11View.h"
#include "D3D11Device.h"

using namespace fq::graphics;

fq::graphics::D3D11LightProbeManager::D3D11LightProbeManager()
	:mDevice(nullptr),
	mResourceManager(nullptr),
	mCubeProbeIndex(0),
	mLightProbeIndex(0),
	mCubeProbes{},
	mLightProbes{},
	mTetrahedrons{}
{

}

fq::graphics::D3D11LightProbeManager::~D3D11LightProbeManager()
{

}

int fq::graphics::D3D11LightProbeManager::AddCubeProbe(const DirectX::SimpleMath::Vector3& position)
{
	CubeProbe* newCubeProbe = new CubeProbe;

	newCubeProbe->position = position;
	newCubeProbe->index = mCubeProbeIndex;

	mCubeProbes[mCubeProbeIndex] = newCubeProbe;

	mCubeProbeIndex++;

	return mCubeProbeIndex;
}

void fq::graphics::D3D11LightProbeManager::DeleteCubeProbe(int index)
{
	// 나중에 새로 만드는 친구의 인덱스가 빈 자리에 들어가도록 채워넣기
	mCubeProbes[index]->index = -1;
}

std::wstring fq::graphics::D3D11LightProbeManager::SaveCubeProbeTexture(const int index, const std::wstring& direction)
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

	std::wstring fileName = L"CubeTexture" + std::to_wstring(index) + direction + L".dds";
	//const wchar_t* filename = L"test.DDS";
	hr = SaveToDDSFile(*image.GetImage(0, 0, 0), DirectX::DDS_FLAGS_NONE, fileName.c_str());
	//if (FAILED(hr))
	//	return hr;

	return fileName;
}

std::vector<CubeProbe*> fq::graphics::D3D11LightProbeManager::GetCubeProbes() const
{
	return mCubeProbes;
}

std::vector<LightProbe*> D3D11LightProbeManager::GetLightProbes() const
{
	return mLightProbes;
}

void D3D11LightProbeManager::DeleteLightProbe(int index)
{
	mLightProbes[index]->index = -1;
}

int D3D11LightProbeManager::AddLightProbe(const DirectX::SimpleMath::Vector3& position)
{
	LightProbe* newCubeProbe = new LightProbe;

	newCubeProbe->position = position;
	newCubeProbe->index = mLightProbeIndex;

	mLightProbes[mLightProbeIndex] = newCubeProbe;

	mLightProbeIndex++;

	return mLightProbeIndex;
}

void D3D11LightProbeManager::MakeTetrahedron()
{
	tetgenio in, out;
	// 삭제한 것 처리하고 해야됨
	in.numberofpoints = mLightProbes.size();

	in.pointlist = new REAL[in.numberofpoints * 3];
	for (int i = 0; i < in.numberofpoints; i++)
	{
		in.pointlist[i * 3 + 0] = mLightProbes[i]->position.x;
		in.pointlist[i * 3 + 1] = mLightProbes[i]->position.y;
		in.pointlist[i * 3 + 2] = mLightProbes[i]->position.z;
	}

	tetgenbehavior tetOption;
	tetOption.parse_commandline((char*)"n");  // 원하는 옵션 설정 n은 neighbors 만들기

	tetrahedralize(&tetOption, &in, &out);

	for (int i = 0; i < out.numberoftetrahedra; i++)
	{
		Tetrahedron* newTet = new Tetrahedron;

		for (int j = 0; j < 4; j++)
		{
			int point_index = out.tetrahedronlist[i * 4 + j];
			int neighbor_index = out.tetrahedronlist[i * 4 + j];
			
			newTet->probes[j] = mLightProbes[point_index];
			newTet->neighbors[j] = neighbor_index;
		}
		newTet->matrix = DirectX::SimpleMath::Matrix{
			newTet->probes[0]->position,
			newTet->probes[1]->position,
			newTet->probes[2]->position
		};
		newTet->matrix._41 = newTet->probes[3]->position.x;
		newTet->matrix._42 = newTet->probes[3]->position.y;
		newTet->matrix._43 = newTet->probes[3]->position.z;

		mTetrahedrons.push_back(newTet);
	}
}

void D3D11LightProbeManager::GetLightProbeInterpolationWeights(const std::vector<Tetrahedron> tets, const DirectX::SimpleMath::Vector3& position, int tetIndex, DirectX::SimpleMath::Vector4& weights, float& t, int& steps)
{
	const int tetCount = tets.size();
	if (tetIndex < 0 || tetIndex >= tetCount)
	{
		tetIndex = 0;
	}
	steps = 0;

	for (; steps < tetCount; steps++)
	{
		// Check if we're in the current "best guess" tetrahedron
		// 현재 "최선의 추측" 사면체인지 확인합니다.
		const Tetrahedron& tet = tets[tetIndex];
		GetBarycentriCoordinateForInnerTetrahedron(position, tet, weights);
		if (weights.x >= 0.0f && weights.y >= 0.0f && weights.z >= 0.0f && weights.w >= 0.0f)
		{
			// Success!
			return;
		}

		// Otherwise find the smallest barycentric coord and move in that direction
		// 그렇지 않으면 가장 작은 이심좌표를 찾아 그 방향으로 이동합니다.
		if (weights.x < weights.y && weights.x < weights.z && weights.x < weights.w)
		{
			tetIndex = tet.neighbors[0];
		}
		else if (weights.y < weights.z && weights.y < weights.w)
		{
			tetIndex = tet.neighbors[1];
		}
		else if (weights.z < weights.w)
		{
			tetIndex = tet.neighbors[2];
		}
		else
		{
			tetIndex = tet.neighbors[3];
		}

		// There's a chance the position lies "between" two tetrahedra, i.e. both return a slightly negative weight
		// due to numerical errors and we ping-pong between them. We could be detecting if the next tet index
		// is the one we came from. But we can also let it reach the max steps count and see if that ever happens in practice.
	}
}

void D3D11LightProbeManager::GetBarycentriCoordinateForInnerTetrahedron(const DirectX::SimpleMath::Vector3& p, const Tetrahedron& tet, DirectX::SimpleMath::Vector4& coords)
{
	DirectX::SimpleMath::Vector3 mult = DirectX::SimpleMath::Vector3::Transform(p - tet.probes[3]->position, tet.matrix);
	coords.x = mult.x;
	coords.y = mult.y;
	coords.z = mult.z;
	coords.w = 1.0f - mult.x - mult.y - mult.z;
}

void D3D11LightProbeManager::Initialize(std::shared_ptr<D3D11Device> device, std::shared_ptr<D3D11ResourceManager> resourceManager)
{
	mDevice = device;
	mResourceManager = resourceManager;
}

