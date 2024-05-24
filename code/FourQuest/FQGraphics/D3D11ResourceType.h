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
		GeometryShader,
		ComputeShader,
	};

	/*=============================================================================
			View
	=============================================================================*/
	enum class ED3D11RenderTargetViewType
	{
		Default,
		Offscreen,
		OffscreenHDR,
		OffscreenGrayscale,

		Picking,

		ColorAcuumulation,
		PixeldRevealageThreshold,

		SingleColor, // OutLine Ã¹´Ü°è 
		OutLine,
		OutLineBlur,
		BlurVertical,

		// Deferred
		Albedo,
		Metalness,
		Roughness,
		Normal,
		Emissive,
		PositionWClipZ,
	};

	enum class ED3D11ShaderResourceViewType
	{
		Default,
	};

	enum class ED3D11DepthStencilViewType
	{
		None,
		Default,
		ShaderInputDepthStencil,
		CascadeShadow1,
		CascadeShadow2,
		CascadeShadow3,
		PointLightShadow,
		Picking,
	};

	/*=============================================================================
			State
	=============================================================================*/
	enum class ED3D11SamplerState
	{
		Default,
		AnisotropicWrap,
		PointClamp,
		Shadow,
		LinearWrap,
	};

	enum class ED3D11RasterizerState
	{
		Default,
		Shadow,
		CullFront,
		CullOff,
	};

	enum class ED3D11DepthStencilState
	{
		Default,
		DisableDepthWirte,
		LessEqual,
		DepthOff,
	};

	enum class ED3D11BlendState
	{
		Default,
		OITRender,
		OITComposite,
		Additive,
	};

	/*=============================================================================
			Buffer
	=============================================================================*/
	enum class ED3D11ConstantBuffer
	{
		Transform,
		Light,
		ViewRotationProj,
		TerrainTexture,
		DirectionalShadowInfo
	};
}