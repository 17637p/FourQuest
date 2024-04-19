#pragma once

#include <windows.h>
#include <d3dcommon.h>

namespace fq::graphics
{
	class D3D11Util
	{
	public:
		static HRESULT CompileShaderFromFile(const WCHAR* szFileName, const D3D_SHADER_MACRO* pDefines, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);
	};
}