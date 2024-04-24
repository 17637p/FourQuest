#pragma once

#include <windows.h>
#include <d3dcommon.h>
#include <directxtk/SimpleMath.h>

namespace fq::graphics
{
	class D3D11Util
	{
	public:
		static HRESULT CompileShaderFromFile(const WCHAR* szFileName, const D3D_SHADER_MACRO* pDefines, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);
		static DirectX::SimpleMath::Matrix InverseTranspose(const DirectX::SimpleMath::Matrix& matrix);
	};
}