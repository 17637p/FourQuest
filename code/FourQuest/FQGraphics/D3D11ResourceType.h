#pragma once

namespace fq::graphics
{
	/*=============================================================================
			Shader Type
	=============================================================================*/
	enum class ED3D11ShaderType
	{
		PixelShader,
		VertexShader,
		GeometryShader,
		ComputeShader,
		DomainShader,
		HullShader
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

		SingleColor, // OutLine 첫 단계 
		OutLine,
		OutLineBlur,
		BlurVertical,

		SSAODepth, // SSAO 첫 단계
		SSAO,

		// Deferred
		Albedo,
		Metalness,
		Roughness,
		Normal,
		Emissive,
		PositionWClipZ,
		SourceNormal,
		SourceTangent,
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
		AnisotropicClamp,
		Shadow,
		LinearWrap,
		PointClamp
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
		DisableDepthTest,
		LessEqualStencilWriteReplace,
		StencilComparisonEqual
	};

	enum class ED3D11BlendState
	{
		Default,
		OITRender,
		OITComposite,
		Additive,
		Subtractive,
		Modulate,
		AlphaBlend,
		DecalBlend,
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