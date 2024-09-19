#include "DeferredDecalPass.h"
#include "ManagementCommon.h"
#include "D3D11Common.h"
#include "Material.h"
#include "DecalObject.h"

namespace fq::graphics
{
	void DeferredDecalPass::Initialize(std::shared_ptr<D3D11Device> device,
		std::shared_ptr<D3D11ResourceManager> resourceManager,
		std::shared_ptr<D3D11CameraManager> cameraManager,
		std::shared_ptr<D3D11ObjectManager> objectManager,
		std::shared_ptr<D3D11DebugDrawManager> debugDrawManager,
		unsigned short width,
		unsigned short height)
	{
		mDevice = device;
		mResourceManager = resourceManager;
		mCameraManager = cameraManager;
		mObjectManager = objectManager;
		mDebugDrawManager = debugDrawManager;

		mAlbedoRTV = mResourceManager->Get<D3D11RenderTargetView>(ED3D11RenderTargetViewType::Albedo);
		mNormalRTV = mResourceManager->Get<D3D11RenderTargetView>(ED3D11RenderTargetViewType::Normal);
		mEmissiveRTV = mResourceManager->Get<D3D11RenderTargetView>(ED3D11RenderTargetViewType::Emissive);

		auto blendState = mResourceManager->Create<D3D11BlendState>(ED3D11BlendState::DecalBlend);
		mDefualtDSV = mResourceManager->Get<D3D11DepthStencilView>(ED3D11DepthStencilViewType::Default);

		mStencilComparisionEqual = mResourceManager->Create<D3D11DepthStencilState>(ED3D11DepthStencilState::StencilComparisonEqual);
		auto pipelieState = std::make_shared<PipelineState>(nullptr, mStencilComparisionEqual, blendState);
		auto DeferredDecalVS = std::make_shared<D3D11VertexShader>(mDevice, L"DeferredDecalVS.cso");
		auto decalPS = std::make_shared<D3D11PixelShader>(mDevice, L"DeferredDecalPS.cso");
		mDecalProgram = std::make_shared<ShaderProgram>(mDevice, DeferredDecalVS, nullptr, decalPS, pipelieState);
		mDecalMaterialCB = std::make_shared< D3D11ConstantBuffer<CBDecalMaterial>>(mDevice, ED3D11ConstantBuffer::Transform);
		mDecalObjectCB = std::make_shared< D3D11ConstantBuffer<CBDecalObject>>(mDevice, ED3D11ConstantBuffer::Transform);

		mLinearClampSamplerState = resourceManager->Create<D3D11SamplerState>(ED3D11SamplerState::Default);
		mPointClampSamplerState = resourceManager->Create<D3D11SamplerState>(ED3D11SamplerState::AnisotropicClamp);
		mAnisotropicWrapSamplerState = resourceManager->Create<D3D11SamplerState>(ED3D11SamplerState::AnisotropicWrap);

		OnResize(width, height);

		struct Vertex
		{
			DirectX::SimpleMath::Vector3 Pos;
			DirectX::SimpleMath::Vector2 UV;
		};

		std::vector<Vertex> positions =
		{
			{ { -0.5f, -0.5f, -0.5f}, { 0.0f, 1.0f }},
			{ { 0.5f, -0.5f, -0.5f}, { 1.0f, 1.0f }},
			{ { 0.5f,  0.5f, -0.5f}, { 1.0f, 0.0f }},
			{ { -0.5f,  0.5f, -0.5f}, { 0.0f, 0.0f }},
			{ { -0.5f, -0.5f,  0.5f}, { 1.0f, 1.0f }},
			{ { 0.5f, -0.5f,  0.5f}, { 0.0f, 1.0f }},
			{ { 0.5f,  0.5f,  0.5f}, { 0.0f, 0.0f }},
			{ { -0.5f,  0.5f,  0.5f}, { 1.0f, 0.0f }},
		};

		std::vector<unsigned int> indices =
		{
			0, 1, 2, 0, 2, 3,
			4, 6, 5, 4, 7, 6,
			4, 5, 1, 4, 1, 0,
			3, 2, 6, 3, 6, 7,
			1, 5, 6, 1, 6, 2,
			4, 0, 3, 4, 3, 7,
		};

		mBoxVB = std::make_shared<D3D11VertexBuffer>(device, positions);
		mBoxIB = std::make_shared<D3D11IndexBuffer>(device, indices);

		InitBlendState();
	}
	void DeferredDecalPass::Finalize()
	{
		mDevice = nullptr;
		mResourceManager = nullptr;
		mCameraManager = nullptr;
		mObjectManager = nullptr;
		mDebugDrawManager = nullptr;

		mAlbedoRTV = nullptr;
		mNormalRTV = nullptr;
		mEmissiveRTV = nullptr;

		mDefualtDSV = nullptr;

		mPositionWSRV = nullptr;
		mSourceNormalSRV = nullptr;
		mSourceTangentSRV = nullptr;

		mStencilComparisionEqual = nullptr;
		mDecalProgram = nullptr;
		mAnisotropicWrapSamplerState = nullptr;
		mLinearClampSamplerState = nullptr;
		mPointClampSamplerState = nullptr;

		mDecalMaterialCB = nullptr;
		mDecalObjectCB = nullptr;

		mBoxVB = nullptr;
		mBoxIB = nullptr;

		for (auto& blendState : mBlendStates)
		{
			unsigned long refCount = blendState.Reset();
			assert(refCount == 0);
		}
	}
	void DeferredDecalPass::OnResize(unsigned short width, unsigned short height)
	{
		mViewport.Width = (float)width;
		mViewport.Height = (float)height;
		mViewport.MinDepth = 0.f;
		mViewport.MaxDepth = 1.f;
		mViewport.TopLeftX = 0.f;
		mViewport.TopLeftY = 0.f;

		auto positionWRTV = mResourceManager->Get<D3D11RenderTargetView>(ED3D11RenderTargetViewType::PositionWClipZ);
		mPositionWSRV = std::make_shared<D3D11ShaderResourceView>(mDevice, positionWRTV);
		auto sourceNormalRTV = mResourceManager->Get<D3D11RenderTargetView>(ED3D11RenderTargetViewType::SourceNormal);
		mSourceNormalSRV = std::make_shared<D3D11ShaderResourceView>(mDevice, sourceNormalRTV);
		auto sourceTangentRTV = mResourceManager->Get<D3D11RenderTargetView>(ED3D11RenderTargetViewType::SourceTangent);
		mSourceTangentSRV = std::make_shared<D3D11ShaderResourceView>(mDevice, sourceTangentRTV);
	}
	void DeferredDecalPass::Render()
	{
		// bind
		{
			mDevice->GetDeviceContext()->RSSetViewports(1, &mViewport);
			mDevice->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			mDecalProgram->Bind(mDevice);

			mDecalObjectCB->Bind(mDevice, ED3D11ShaderType::VertexShader);
			mDecalObjectCB->Bind(mDevice, ED3D11ShaderType::PixelShader);
			mDecalMaterialCB->Bind(mDevice, ED3D11ShaderType::PixelShader, 1);
			mLinearClampSamplerState->Bind(mDevice, 0, ED3D11ShaderType::PixelShader);
			mPointClampSamplerState->Bind(mDevice, 1, ED3D11ShaderType::PixelShader);
			mAnisotropicWrapSamplerState->Bind(mDevice, 2, ED3D11ShaderType::PixelShader);

			mBoxVB->Bind(mDevice);
			mBoxIB->Bind(mDevice);

			std::vector<std::shared_ptr<D3D11RenderTargetView>> renderTargetViews;
			renderTargetViews.reserve(5u);
			renderTargetViews.push_back(mAlbedoRTV);
			renderTargetViews.push_back(mNormalRTV);
			renderTargetViews.push_back(mEmissiveRTV);
			D3D11RenderTargetView::Bind(mDevice, renderTargetViews, mDefualtDSV);

			mPositionWSRV->Bind(mDevice, 5, ED3D11ShaderType::PixelShader);
			mSourceNormalSRV->Bind(mDevice, 7, ED3D11ShaderType::PixelShader);
			mSourceTangentSRV->Bind(mDevice, 8, ED3D11ShaderType::PixelShader);
		}

		// cb update
		const std::set<IDecalObject*>& decalObjects = mObjectManager->GetDecalObjects();

		for (IDecalObject* decalObjectInterface : decalObjects)
		{
			DecalObject* decalObject = static_cast<DecalObject*>(decalObjectInterface);
			std::shared_ptr<DecalMaterial> material = std::static_pointer_cast<DecalMaterial>(decalObjectInterface->GetDecalMaterial());
			auto transform = decalObject->GetTransform();
			const auto& decalInfo = decalObject->GetDecalInfo();

			DirectX::SimpleMath::Vector3 translation;
			DirectX::SimpleMath::Vector3 scale;
			DirectX::SimpleMath::Quaternion rotation;
			transform.Decompose(scale, rotation, translation);

			auto calcTransform =
				DirectX::SimpleMath::Matrix::CreateScale(decalInfo.Width, decalInfo.Depth, decalInfo.Height)
				* DirectX::SimpleMath::Matrix::CreateTranslation(decalInfo.Pivot.x, decalInfo.Pivot.z, decalInfo.Pivot.y)
				* DirectX::SimpleMath::Matrix::CreateFromQuaternion(rotation)
				* DirectX::SimpleMath::Matrix::CreateTranslation(translation);

			material->Bind(mDevice);

			CBDecalObject decalObjectCB;
			decalObjectCB.Deproject.x = mCameraManager->GetProjectionMatrix(ECameraType::Player)._11;
			decalObjectCB.Deproject.y = mCameraManager->GetProjectionMatrix(ECameraType::Player)._22;
			decalObjectCB.TexTransform = (DirectX::SimpleMath::Matrix::CreateScale(decalInfo.Tiling.x, decalInfo.Tiling.y, 1) * DirectX::SimpleMath::Matrix::CreateTranslation(decalInfo.Offset.x, decalInfo.Offset.y, 0)).Transpose();
			decalObjectCB.World = calcTransform.Transpose();
			decalObjectCB.View = mCameraManager->GetViewMatrix(ECameraType::Player).Transpose();
			decalObjectCB.Proj = mCameraManager->GetProjectionMatrix(ECameraType::Player).Transpose();
			decalObjectCB.InvWV = (calcTransform * mCameraManager->GetViewMatrix(ECameraType::Player)).Invert().Transpose();
			decalObjectCB.NormalThresholdInRadian = decalInfo.NormalThresholdInDegree * 3.14f / 180.f;
			mDecalObjectCB->Update(mDevice, decalObjectCB);

			const auto& materialInfo = material->GetInfo();
			CBDecalMaterial decalMaterialCB;
			decalMaterialCB.BaseColor = materialInfo.BaseColor;
			decalMaterialCB.EmissiveColor = materialInfo.EmissiveColor;
			decalMaterialCB.bUseBaseColor = materialInfo.bUseBaseColor && material->GetHasBaseColor();
			decalMaterialCB.bUseNormalness = materialInfo.bUseNormalness && material->GetHasNormal();
			decalMaterialCB.bIsUsedEmissive = materialInfo.bIsUsedEmissive && material->GetHasEmissive();
			decalMaterialCB.NormalBlend = materialInfo.NormalBlend;
			decalMaterialCB.AlphaCutoff = materialInfo.AlphaCutoff;
			mDecalMaterialCB->Update(mDevice, decalMaterialCB);

			int index = decalMaterialCB.bIsUsedEmissive << 2 | decalMaterialCB.bUseNormalness << 1 | decalMaterialCB.bUseBaseColor << 0;
			mDevice->GetDeviceContext()->OMSetBlendState(mBlendStates[index].Get(), nullptr, 0xFFFFFFFF);

			mDevice->GetDeviceContext()->DrawIndexed(36, 0, 0);

			if (decalInfo.bIsRenderDebug)
			{
				debug::OBBInfo obbInfo;
				obbInfo.OBB.Extents = { 0.5f, 0.5f, 0.5f };
				obbInfo.OBB.Transform(obbInfo.OBB, calcTransform);
				obbInfo.Color = decalInfo.DebugRenderColor;
				mDebugDrawManager->Submit(obbInfo);
				obbInfo = {};
				obbInfo.OBB.Center = { 0.f, -0.25f, 0.f };
				obbInfo.OBB.Extents = { 0.1f, 0.25f, 0.1f };
				obbInfo.OBB.Transform(obbInfo.OBB, calcTransform);
				obbInfo.Color = decalInfo.DebugRenderColor;
				mDebugDrawManager->Submit(obbInfo);
			}
		}
	}

	void DeferredDecalPass::InitBlendState()
	{
		D3D11_BLEND_DESC blendDesc = CD3D11_BLEND_DESC{ CD3D11_DEFAULT{} };
		blendDesc.AlphaToCoverageEnable = FALSE;
		blendDesc.IndependentBlendEnable = TRUE;
		blendDesc.RenderTarget[0].BlendEnable = TRUE;
		blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[1].BlendEnable = FALSE;
		blendDesc.RenderTarget[2].BlendEnable = FALSE;
		blendDesc.RenderTarget[3].BlendEnable = TRUE;
		blendDesc.RenderTarget[3].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		blendDesc.RenderTarget[3].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		blendDesc.RenderTarget[3].BlendOp = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[3].SrcBlendAlpha = D3D11_BLEND_ONE;
		blendDesc.RenderTarget[3].DestBlendAlpha = D3D11_BLEND_ZERO;
		blendDesc.RenderTarget[3].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[4] = blendDesc.RenderTarget[0];

		for (int i = 0; i < 2; ++i)
		{
			for (int j = 0; j < 2; ++j)
			{
				for (int k = 0; k < 2; ++k)
				{
					blendDesc.RenderTarget[0].RenderTargetWriteMask = i == 0 ? 0 : D3D11_COLOR_WRITE_ENABLE_ALL;
					blendDesc.RenderTarget[1].RenderTargetWriteMask = j == 0 ? 0 : D3D11_COLOR_WRITE_ENABLE_ALL;
					blendDesc.RenderTarget[2].RenderTargetWriteMask = k == 0 ? 0 : D3D11_COLOR_WRITE_ENABLE_ALL;

					size_t index = k << 2 | j << 1 | i << 0;
					HR(mDevice->GetDevice()->CreateBlendState(&blendDesc, mBlendStates[index].GetAddressOf()));
				}
			}
		}
	}
}