#pragma once

#include <windows.h>
#include <d3dcommon.h>
#include <directxtk/SimpleMath.h>
#include <d3d11.h>

namespace fq::graphics
{
	class D3D11Util
	{
	public:
		static HRESULT CompileShaderFromFile(const WCHAR* szFileName, const D3D_SHADER_MACRO* pDefines, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);
		static DirectX::SimpleMath::Matrix InverseTranspose(const DirectX::SimpleMath::Matrix& matrix);

		static float RandF();
		static float RandF(float a, float b);
		static void CreateRandomTexture1DSRV(ID3D11Device* device, ID3D11ShaderResourceView** ppSRV);
		static void CreateRandomTexture2DSRV(ID3D11Device* device, ID3D11ShaderResourceView** ppSRV);
		static void CreateCurveTexture(ID3D11Device* device, ID3D11ShaderResourceView** ppSRV, const std::vector<DirectX::SimpleMath::Vector2>& points);
	};
}