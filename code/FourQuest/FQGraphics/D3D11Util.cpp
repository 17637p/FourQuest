#include <d3dcompiler.h>
#include <wrl.h>

#include "D3D11Util.h"
#include "Define.h"

namespace fq::graphics
{
	HRESULT D3D11Util::CompileShaderFromFile(const WCHAR* szFileName, const D3D_SHADER_MACRO* pDefines, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
	{
		HRESULT hr = S_OK;

		DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
		// Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
		// Setting this flag improves the shader debugging experience, but still allows 
		// the shaders to be optimized and to run exactly the way they will run in 
		// the release configuration of this program.
		dwShaderFlags |= D3DCOMPILE_DEBUG;

		// Disable optimizations to further improve shader debugging
		dwShaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

		ID3DBlob* pErrorBlob = nullptr;
		hr = D3DCompileFromFile(szFileName, pDefines, D3D_COMPILE_STANDARD_FILE_INCLUDE, szEntryPoint, szShaderModel,
			dwShaderFlags, 0, ppBlobOut, &pErrorBlob);
		if (FAILED(hr))
		{
			if (pErrorBlob)
			{
				MessageBoxA(NULL, (char*)pErrorBlob->GetBufferPointer(), "CompileShaderFromFile", MB_OK);
				pErrorBlob->Release();
			}
			return hr;
		}
		if (pErrorBlob) pErrorBlob->Release();

		return S_OK;
	}

	DirectX::SimpleMath::Matrix D3D11Util::InverseTranspose(const DirectX::SimpleMath::Matrix& matrix)
	{
		DirectX::SimpleMath::Matrix result = matrix;
		result._41 = 0.f;
		result._42 = 0.f;
		result._43 = 0.f;
		result._44 = 1.f;

		DirectX::SimpleMath::Matrix invResult;
		result.Invert(invResult);

		return invResult.Transpose();
	}

	float D3D11Util::RandF()
	{
		return (float)(rand()) / (float)RAND_MAX;
	}

	float D3D11Util::RandF(float a, float b)
	{
		return a + RandF() * (b - a);
	}

	void D3D11Util::CreateRandomTexture1DSRV(ID3D11Device* device, ID3D11ShaderResourceView** ppSRV)
	{
		using namespace DirectX::SimpleMath;

		// 랜덤 백터 생성
		const size_t ELEMENT_COUNT = 1024u;

		std::vector<Vector4> randomValues(ELEMENT_COUNT);

		for (size_t i = 0; i < ELEMENT_COUNT; ++i)
		{
			randomValues[i].x = RandF(-1.0f, 1.0f);
			randomValues[i].y = RandF(-1.0f, 1.0f);
			randomValues[i].z = RandF(-1.0f, 1.0f);
			randomValues[i].w = RandF(-1.0f, 1.0f);
		}

		// 텍스처 생성
		D3D11_TEXTURE1D_DESC texDesc;
		texDesc.Width = ELEMENT_COUNT;
		texDesc.MipLevels = 1;
		texDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		texDesc.Usage = D3D11_USAGE_IMMUTABLE;
		texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		texDesc.CPUAccessFlags = 0;
		texDesc.MiscFlags = 0;
		texDesc.ArraySize = 1;

		D3D11_SUBRESOURCE_DATA initData;
		initData.pSysMem = &randomValues[0];
		initData.SysMemPitch = ELEMENT_COUNT * sizeof(Vector4);
		initData.SysMemSlicePitch = 0;

		Microsoft::WRL::ComPtr<ID3D11Texture1D> randomTex;
		HR(device->CreateTexture1D(&texDesc, &initData, randomTex.GetAddressOf()));

		// 쉐이더 리소스 뷰 생성
		D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
		viewDesc.Format = texDesc.Format;
		viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1D; // 1D 텍스처
		viewDesc.Texture1D.MipLevels = texDesc.MipLevels;
		viewDesc.Texture1D.MostDetailedMip = 0;

		assert(randomTex != nullptr);
		HR(device->CreateShaderResourceView(randomTex.Get(), &viewDesc, ppSRV));
	}

	void D3D11Util::CreateRandomTexture2DSRV(ID3D11Device* device, ID3D11ShaderResourceView** ppSRV)
	{
		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Width = 1024;
		desc.Height = 1024;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		desc.Usage = D3D11_USAGE_IMMUTABLE;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.MipLevels = 1;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;

		float* values = new float[desc.Width * desc.Height * 4];
		float* ptr = values;
		for (UINT i = 0; i < desc.Width * desc.Height; i++)
		{
			ptr[0] = RandF(-1.0f, 1.0f);
			ptr[1] = RandF(-1.0f, 1.0f);
			ptr[2] = RandF(-1.0f, 1.0f);
			ptr[3] = RandF(-1.0f, 1.0f);
			ptr += 4;
		}

		D3D11_SUBRESOURCE_DATA data;
		data.pSysMem = values;
		data.SysMemPitch = desc.Width * 16;
		data.SysMemSlicePitch = 0;

		Microsoft::WRL::ComPtr<ID3D11Texture2D> randomTex;
		device->CreateTexture2D(&desc, &data, randomTex.GetAddressOf());

		delete[] values;

		D3D11_SHADER_RESOURCE_VIEW_DESC srv;
		srv.Format = desc.Format;
		srv.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srv.Texture2D.MipLevels = 1;
		srv.Texture2D.MostDetailedMip = 0;

		assert(randomTex != nullptr);
		device->CreateShaderResourceView(randomTex.Get(), &srv, ppSRV);
	}
	void D3D11Util::CreateCurveTexture(ID3D11Device* device, ID3D11ShaderResourceView** ppSRV, const std::vector<DirectX::SimpleMath::Vector2>& points)
	{
		auto evaluateCurve = [&points](float t)
			{
				{
					if (points.empty()) return t;
					if (points.size() == 1) return points[0].y;

					for (size_t i = 1; i < points.size(); ++i)
					{
						if (t < points[i].x)
						{
							const auto& p0 = points[i - 1];
							const auto& p1 = points[i];
							float factor = (t - p0.x) / (p1.x - p0.x);
							return p0.y + factor * (p1.y - p0.y);
						}
					}

					return points.back().y;
				}
			};

		std::vector<float> data(256);
		for (int i = 0; i < 256; ++i)
		{
			float t = static_cast<float>(i) / 255.0f;
			data[i] = evaluateCurve(t);
		}

		D3D11_TEXTURE1D_DESC desc = {};
		desc.Width = 256;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_R32_FLOAT;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA initData = {};
		initData.pSysMem = data.data();
		initData.SysMemPitch = static_cast<UINT>(256 * sizeof(float));

		Microsoft::WRL::ComPtr<ID3D11Texture1D> texture;
		HR(device->CreateTexture1D(&desc, &initData, texture.GetAddressOf()));
	
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = desc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1D;
		srvDesc.Texture1D.MostDetailedMip = 0;
		srvDesc.Texture1D.MipLevels = 1;

		HR(device->CreateShaderResourceView(texture.Get(), &srvDesc, ppSRV));
	}
}