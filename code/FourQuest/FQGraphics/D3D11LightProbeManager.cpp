#include "D3D11LightProbeManager.h"

#include <DirectXTex.h>
#include <directxtk/DDSTextureLoader.h>
#include <fstream>
#include <filesystem>

#include "d3d11resourceManager.h"
#include "D3D11View.h"
#include "D3D11Device.h"
#include "D3D11Texture.h"

using namespace fq::graphics;

fq::graphics::D3D11LightProbeManager::D3D11LightProbeManager()
	:mDevice(nullptr),
	mResourceManager(nullptr),
	mCubeProbeIndex(0),
	mLightProbeIndex(0),
	mLightProbeSize(0),
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
	
	mCubeProbes.push_back(newCubeProbe);

	mCubeProbeIndex++;

	return mCubeProbeIndex;
}

void fq::graphics::D3D11LightProbeManager::DeleteCubeProbe(int index)
{
	// 나중에 새로 만드는 친구의 인덱스가 빈 자리에 들어가도록 채워넣기
	mCubeProbes[index]->index = -1;
}

std::wstring fq::graphics::D3D11LightProbeManager::SaveProbe1DirectionTexture(const int index, const std::wstring& direction)
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

DirectX::SimpleMath::Vector3 D3D11LightProbeManager::GetLightProbePosition(int lightProbeIndex)
{
	return mLightProbes[mLightProbePair[lightProbeIndex]]->position;
}

bool D3D11LightProbeManager::LoadLightProbes(const std::string& fileName)
{
	if (mLightProbeSize != 0)
	{
		ClearAllLightProbe();
		ClearAllTetrahedron();
	}

	std::ifstream lightProbeFile("./resource/LightProbe/" + fileName + ".Ltp");

	if (lightProbeFile.is_open())
	{
		lightProbeFile >> mLightProbeSize;

		// Write LightProbe
		for (int i = 0; i < mLightProbeSize; i++)
		{
			LightProbe* newLightProbe = new LightProbe;

			lightProbeFile >> newLightProbe->position.x;
			lightProbeFile >> newLightProbe->position.y;
			lightProbeFile >> newLightProbe->position.z;

			for (int j = 0; j < 27; j++)
			{
				lightProbeFile >> newLightProbe->coefficient[j];
			}

			mLightProbes.push_back(newLightProbe);
		}

		// Tetrahedron Size
		int tetSize = -1;
		lightProbeFile >> tetSize;

		// Write Tetrahedron 
		for (int i = 0; i < tetSize; i++)
		{
			Tetrahedron* newTetrahedron = new Tetrahedron;
			// probe index
			for (int j = 0; j < 4; j++)
			{
				int probeIndex = -1;

				lightProbeFile >> probeIndex;
				newTetrahedron->probes[j] = mLightProbes[probeIndex];
				//newTetrahedron->probes[j]->index;
			}

			// neighbors
			for (int j = 0; j < 4; j++)
			{
				lightProbeFile >> newTetrahedron->neighbors[j];
			}

			// Matrix
			for (int j = 0; j < 3; j++)
			{
				for (int k = 0; k < 3; k++)
				{
					lightProbeFile >> newTetrahedron->matrix.m[j][k];
				}
			}

			mTetrahedrons.push_back(newTetrahedron);
		}

		lightProbeFile.close();

		return true;
	}
	else
	{
		// 읽지 못함
		//spdlog::error("NavMesh File Can't Load");

		return false;
	}
}

void D3D11LightProbeManager::SaveLightProbes(const std::string& fileName)
{
	if (mLightProbeSize < 3)
	{
		//spdlog::error("Not Exist Navigation Mesh");
		return;
	}


	std::filesystem::path dirPath = "./resource/LightProbe/";
	if (!std::filesystem::exists(dirPath))
	{
		std::filesystem::create_directory(dirPath);
	}

	std::string line;
	std::ofstream lightProbeFile(dirPath.string() + fileName + ".Ltp");
	if (lightProbeFile.is_open())
	{
		// LightProbeSize
		lightProbeFile << mLightProbeSize << "\n";

		// Write LightProbe
		for (int i = 0; i < mLightProbeSize; i++)
		{
			lightProbeFile << mLightProbes[i]->position.x << " ";
			lightProbeFile << mLightProbes[i]->position.y << " ";
			lightProbeFile << mLightProbes[i]->position.z << "\n";

			for (int j = 0; j < 27; j++)
			{
				lightProbeFile << mLightProbes[i]->coefficient[j] << " ";
			}
			lightProbeFile << "\n";
		}

		// Tetrahedron Size
		lightProbeFile << mTetrahedrons.size() << "\n";

		// Write Tetrahedron 
		for (int i = 0; i < mTetrahedrons.size(); i++)
		{
			// probe index
			for (int j = 0; j < 4; j++)
			{
				lightProbeFile << mTetrahedrons[i]->probes[j]->index << " ";
			}
			lightProbeFile << "\n";

			// neighbors
			for (int j = 0; j < 4; j++)
			{
				lightProbeFile << mTetrahedrons[i]->neighbors[j] << " ";
			}
			lightProbeFile << "\n";

			// Matrix
			for (int j = 0; j < 3; j++)
			{
				for (int k = 0; k < 3; k++)
				{
					lightProbeFile << mTetrahedrons[i]->matrix.m[j][k] << " ";
				}
			}
			lightProbeFile << "\n";
		}

		lightProbeFile.close();
	}
	else
	{
		// 파일 오픈 불가
		//spdlog::error("NavMesh File Can't write");
	}

	// 성공
	//spdlog::info("Navigation Mesh Save Successful");
}

void D3D11LightProbeManager::ClearAllTetrahedron()
{
	for (int i = 0; i < mTetrahedrons.size(); i++)
	{
		delete mTetrahedrons[i];
	}
	mTetrahedrons.clear();
}

void D3D11LightProbeManager::ClearAllLightProbe()
{
	for (int i = 0; i < mLightProbeSize; i++)
	{
		delete mLightProbes[i];
	}
	mLightProbes.clear();
	mLightProbeIndex = 0;
	mLightProbeSize = 0;
}

int D3D11LightProbeManager::GetLightProbesSize()
{
	return mLightProbeSize;
}

void D3D11LightProbeManager::lerpLightProbe(Tetrahedron* tet, const DirectX::SimpleMath::Vector4& weights, float* r, float* g, float* b)
{
	for (int i = 0; i < 9; i++)
	{
		r[i] = tet->probes[0]->coefficient[i] * weights.x;
		r[i] += tet->probes[1]->coefficient[i] * weights.y;
		r[i] += tet->probes[2]->coefficient[i] * weights.z;
		r[i] += tet->probes[3]->coefficient[i] * weights.w;
	}

	for (int i = 0; i < 9; i++)
	{
		g[i] = tet->probes[0]->coefficient[i + 9] * weights.x;
		g[i] += tet->probes[1]->coefficient[i + 9] * weights.y;
		g[i] += tet->probes[2]->coefficient[i + 9] * weights.z;
		g[i] += tet->probes[3]->coefficient[i + 9] * weights.w;
	}

	for (int i = 0; i < 9; i++)
	{
		b[i] = tet->probes[0]->coefficient[i + 18] * weights.x;
		b[i] += tet->probes[1]->coefficient[i + 18] * weights.y;
		b[i] += tet->probes[2]->coefficient[i + 18] * weights.z;
		b[i] += tet->probes[3]->coefficient[i + 18] * weights.w;
	}
}

void D3D11LightProbeManager::GetCoefficientTetrahedronWeight(const DirectX::SimpleMath::Vector4& weights, int TetIndex, float* r, float* g, float* b)
{
	lerpLightProbe(mTetrahedrons[TetIndex], weights, r, g, b);
}

int D3D11LightProbeManager::GetTetIndex(int& tetIndex, const DirectX::SimpleMath::Vector3& position,
	DirectX::SimpleMath::Vector4& weights)
{
	int step;

	getLightProbeInterpolationWeights(mTetrahedrons, position, tetIndex, weights, step);

	return tetIndex;
}

void D3D11LightProbeManager::BakeAllLightProbeCoefficient()
{
	float* r = new float[9];
	float* g = new float[9];
	float* b = new float[9];

	for (int i = 0; i < mLightProbeSize; i++)
	{
		auto cubeMap = mResourceManager->Create<D3D11Texture>(L"LightProbe" + std::to_wstring(i) + L".dds");

		cubeMap->GetSHCoefficientRGB(mDevice, r, g, b);

		for (int j = 0; j < 9; j++)
		{
			mLightProbes[i]->coefficient[j] = r[j];
		}
		for (int j = 0; j < 9; j++)
		{
			mLightProbes[i]->coefficient[j + 9] = g[j];
		}
		for (int j = 0; j < 9; j++)
		{
			mLightProbes[i]->coefficient[j + 18] = b[j];
		}
	}

	delete[] r;
	delete[] g;
	delete[] b;
}

std::vector<LightProbe*> D3D11LightProbeManager::GetLightProbes() const
{
	return mLightProbes;
}

void D3D11LightProbeManager::DeleteLightProbe(int index)
{
	// 삭제하기
	int deleteIndex = mLightProbePair[index];
	delete mLightProbes[deleteIndex];
	mLightProbeSize--;

	// 삭제하고 맨 뒤에 있던 라이트 프로브를 그 자리에 넣어주기
	mLightProbes[deleteIndex] = mLightProbes[mLightProbeSize];
	mLightProbePair[mLightProbeSize] = deleteIndex;
	mLightProbes[deleteIndex]->index = deleteIndex;
}

int D3D11LightProbeManager::AddLightProbe(const DirectX::SimpleMath::Vector3& position)
{
	LightProbe* newLightProbe = new LightProbe;

	newLightProbe->position = position;
	newLightProbe->index = mLightProbeSize;

	mLightProbePair[mLightProbeIndex] = mLightProbeSize;
	mLightProbes.push_back(newLightProbe);

	mLightProbeIndex++;
	mLightProbeSize++;

	return mLightProbeIndex;
}

void D3D11LightProbeManager::MakeTetrahedron()
{
	ClearAllTetrahedron();

	tetgenio in, out;
	// 삭제한 것 처리하고 해야됨
	in.numberofpoints = mLightProbeSize;

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
			int neighbor_index = out.neighborlist[i * 4 + j];
			
			newTet->probes[j] = mLightProbes[point_index];
			newTet->neighbors[j] = neighbor_index;
		}
		newTet->matrix = DirectX::SimpleMath::Matrix{
			newTet->probes[0]->position.x - newTet->probes[3]->position.x, newTet->probes[1]->position.x - newTet->probes[3]->position.x, newTet->probes[2]->position.x - newTet->probes[3]->position.x, 0,
			newTet->probes[0]->position.y - newTet->probes[3]->position.y, newTet->probes[1]->position.y - newTet->probes[3]->position.y, newTet->probes[2]->position.y - newTet->probes[3]->position.y, 0,
			newTet->probes[0]->position.z - newTet->probes[3]->position.z, newTet->probes[1]->position.z - newTet->probes[3]->position.z, newTet->probes[2]->position.z - newTet->probes[3]->position.z, 0,
			0, 0, 0, 1
		};
		newTet->matrix = newTet->matrix.Transpose();
		newTet->matrix = newTet->matrix.Invert();

		mTetrahedrons.push_back(newTet);
	}

	out.save_elements("output");
	out.save_nodes("output");
}

void D3D11LightProbeManager::getLightProbeInterpolationWeights(const std::vector<Tetrahedron*> tets, const DirectX::SimpleMath::Vector3& position, int tetIndex, DirectX::SimpleMath::Vector4& weights, int& steps)
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
		// temp
		if (tetIndex < 0)
		{
			break;
		}

		const Tetrahedron& tet = *tets[tetIndex];
		getBarycentriCoordinateForInnerTetrahedron(position, tet, weights);
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

void D3D11LightProbeManager::getBarycentriCoordinateForInnerTetrahedron(const DirectX::SimpleMath::Vector3& p, const Tetrahedron& tet, DirectX::SimpleMath::Vector4& coords)
{
	DirectX::SimpleMath::Vector3 mult = DirectX::SimpleMath::Vector3::TransformNormal(p - tet.probes[3]->position, tet.matrix);
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

