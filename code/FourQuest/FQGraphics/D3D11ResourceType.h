#pragma once

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
};

enum class ED3D11ShaderResourceViewType
{
	Default,
};

enum class ED3D11DepthStencilViewType
{
	Default,
};

/*=============================================================================
		State
=============================================================================*/
enum class ED3D11SamplerState
{
	Default,
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