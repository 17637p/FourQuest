#pragma once

namespace fq::graphics
{
	/*=============================================================================
			Shader Type
	=============================================================================*/
	enum class ED3D11ShaderType
	{
		Pixelshader,
		VertexShader,
		GeometryShader
	};

	/*=============================================================================
			View
	=============================================================================*/
	enum class ED3D11RenderTargetViewType
	{
		Default,
		Offscreen,
		OffscreenHDR,
		OffscreenGrayscale
	};

	enum class ED3D11ShaderResourceViewType
	{
		Default,
	};

	enum class ED3D11DepthStencilViewType
	{
		None,
		Default,
		ShaderInputDepthStencil
	};

	/*=============================================================================
			State
	=============================================================================*/
	enum class ED3D11SamplerState
	{
		Default,
		AnisotropicWrap,
		PointClamp
	};

	enum class ED3D11RasterizerState
	{
		Default,
	};

	enum class ED3D11DepthStencilState
	{
		Default,
	};

	enum class ED3D11BlendState
	{
		Default,
	};

	/*=============================================================================
			Buffer
	=============================================================================*/
	enum class ED3D11ConstantBuffer
	{
		Transform,
	};
}