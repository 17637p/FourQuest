#include <d3d11.h>
#include "D3D11PostProcessingManager.h"
#include "D3D11Common.h"

namespace fq::graphics
{
	void D3D11PostProcessingManager::Initialize(std::shared_ptr<D3D11Device> device)
	{
		mFullScreenVB = std::make_shared<D3D11VertexBuffer>(D3D11VertexBuffer::CreateFullScreenVertexBuffer(device));
		mFullScreenIB = std::make_shared<D3D11IndexBuffer>(D3D11IndexBuffer::CreateFullScreenIndexBuffer(device));

		auto fullScreenVS = std::make_shared<D3D11VertexShader>(device, L"FullScreenVS.cso");
		auto toneMappingPS = std::make_shared<D3D11PixelShader>(device, L"ToneMappingPS.cso");
		auto pipelieState = std::make_shared<PipelineState>(nullptr, nullptr, nullptr);
		mToneMapping = std::make_unique<ShaderProgram>(device, fullScreenVS, nullptr, toneMappingPS, pipelieState);
	}

	void D3D11PostProcessingManager::Excute(std::shared_ptr<class D3D11RenderTargetView> rtv, std::shared_ptr<class D3D11ShaderResourceView> srv, EPostEffectFlag flag)
	{


	}

	void D3D11PostProcessingManager::Excute(class D3D11RenderTargetView* rtv, class ID3D11ShaderResourceView* srv, EPostEffectFlag flag)
	{

	}
}