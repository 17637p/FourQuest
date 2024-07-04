#pragma once

namespace fq::graphics
{
	class D3D11Device;
	class D3D11InputLayout;
	class D3D11VertexShader;
	class D3D11PixelShader;
	class D3D11RenderTargetView;
	class D3D11ShaderResourceView;
	class D3D11DepthStencilView;
	template <typename T>
	class D3D11ConstantBuffer;
	class D3D11VertexBuffer;
	class D3D11IndexBuffer;
	class D3D11ResourceManager;
	class D3D11CameraManager;
	class D3D11LightManager;
	class D3D11LightProbeManager;
	class D3D11JobManager;
	class D3D11ParticleManager;
	class D3D11DebugDrawManager;
	class D3D11RasterizerState;
	class D3D11GeometryShader;
	class D3D11BlendState;
	class D3D11DepthStencilState;
	class D3D11SamplerState;
	class D3D11Texture;
	class D3D11UnorderedAccessView;
	class D3D11StructuredBuffer;

	class Pass
	{
	public:
		virtual void Finalize() = 0;
		virtual void OnResize(unsigned short width, unsigned short height) = 0;
		virtual void Render() = 0;
	};
}