#pragma once

#include "CommonHeader.h"
#include "../FQCommon/FQCommonGraphics.h"
#include "ConstantBufferStructure.h"

namespace fq::graphics
{
	class D3D11Device;
	class D3D11ResourceManager;
	class D3D11CameraManager;
	class ParticleObject;
	class IParticleObject;
	template <typename T>
	class D3D11ConstantBuffer;
	class IParticleMaterial;

	class D3D11ParticleManager
	{
	public:
		D3D11ParticleManager() = default;
		~D3D11ParticleManager() = default;

		void Initialize(const std::shared_ptr<D3D11Device> device, std::shared_ptr<D3D11ResourceManager> resourceManager, std::shared_ptr<D3D11CameraManager> cameraManager);
		void OnResize(unsigned short width, unsigned short height);

		void Excute();

		IParticleObject* CreateParticleObject(const DirectX::SimpleMath::Matrix& transform, const ParticleInfo& particleInfo, std::shared_ptr<IParticleMaterial> iParticleMaterial);
		void DeleteParticleObject(IParticleObject* particleObjectInterface);

		const std::set<IParticleObject*>& GetParticleObjects() const { return mParticleObjects; }

	private:
		void updateParticleObjectCB(IParticleObject* particleObjectInterface);
		void emit(IParticleObject* particleObjectInterface);
		void simulate(IParticleObject* particleObjectInterface);
		void render(IParticleObject* particleObjectInterface);

		int align(int value, int aligment);
		int readCounter(ID3D11UnorderedAccessView* uav);

	private:
		std::shared_ptr<D3D11Device> mDevice;
		std::shared_ptr<D3D11ResourceManager> mResourceManager;
		std::shared_ptr<D3D11CameraManager> mCameraManager;

		std::set<IParticleObject*> mParticleObjects;

		std::shared_ptr<D3D11ConstantBuffer<ParticleFrameData>> mParticleFrameCB;
		std::shared_ptr<D3D11ConstantBuffer<ParticleObjectData>> mParticleObjectCB;

		std::shared_ptr<class D3D11RenderTargetView> mBackBufferRTV;
		std::shared_ptr<class D3D11DepthStencilView> mNoneDSV;
		std::shared_ptr<class D3D11DepthStencilView> mDSV;

		std::shared_ptr<class D3D11RasterizerState> mDefaultRasterizer;
		std::shared_ptr<class D3D11RasterizerState> mCullOffRasterizer;
		std::shared_ptr<class D3D11BlendState> mAdditiveState;
		std::shared_ptr<class D3D11BlendState> mSubtractiveState;
		std::shared_ptr<class D3D11BlendState> mModulateState;
		std::shared_ptr<class D3D11BlendState> mAlphaBlendState;
		std::shared_ptr<class ShaderProgram> mRenderProgram;

		std::shared_ptr<class D3D11ComputeShader> mInitDeadListCS;
		std::shared_ptr<class D3D11ComputeShader> mInitParticlesCS;
		std::shared_ptr<class D3D11ComputeShader> mEmitCS;
		std::shared_ptr<class D3D11ComputeShader> mSimulateCS;

		std::shared_ptr<class D3D11SamplerState> mLinearWrap;
		std::shared_ptr<class D3D11SamplerState> mPointClamp;

		Microsoft::WRL::ComPtr<ID3D11Buffer> mDebugCounterBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer> mDeadListCB;
		Microsoft::WRL::ComPtr<ID3D11Buffer> mActiveListCB;

		Microsoft::WRL::ComPtr<ID3D11Buffer> mIndirectDrawArgsBuffer;
		Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> mIndirectDrawArgsBufferUAV;

		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mRandomTextureSRV;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mDepthSRV;
	};
};