#pragma once

#include "CommonHeader.h"
#include "../FQCommon/FQCommonGraphics.h"

namespace fq::graphics
{
	class D3D11Device;
	class D3D11ResourceManager;
	class D3D11CameraManager;

	struct GPUParticlePartA
	{
		DirectX::SimpleMath::Vector4 Params[3];
	};

	struct GPUParticlePartB
	{
		DirectX::SimpleMath::Vector4 Params[3];
	};

#define NUM_EMITTERS					4

	struct PER_FRAME_CONSTANT_BUFFER
	{
		DirectX::XMVECTOR	m_StartColor[NUM_EMITTERS];
		DirectX::XMVECTOR	m_EndColor[NUM_EMITTERS];
		DirectX::XMVECTOR	m_EmitterLightingCenter[NUM_EMITTERS];

		DirectX::XMMATRIX	m_ViewProjection;
		DirectX::XMMATRIX	m_ViewProjInv;
		DirectX::XMMATRIX	m_View;
		DirectX::XMMATRIX	m_ViewInv;
		DirectX::XMMATRIX	m_Projection;
		DirectX::XMMATRIX	m_ProjectionInv;

		DirectX::XMVECTOR	m_EyePosition;
		DirectX::XMVECTOR	m_SunDirection;
		DirectX::XMVECTOR	m_SunColor;
		DirectX::XMVECTOR	m_AmbientColor;

		DirectX::XMVECTOR	m_SunDirectionVS;
		DirectX::XMVECTOR	pads2[3];

		float				m_FrameTime;
		int					m_ScreenWidth;
		int					m_ScreenHeight;
		int					m_FrameIndex;

		float				m_AlphaThreshold;
		float				m_CollisionThickness;
		float				m_ElapsedTime;
		int					m_CollisionsEnabled;

		int					m_ShowSleepingParticles;
		int					m_EnableSleepState;
		int					pads[2];
	};

	typedef struct SortConstants
	{
		int x, y, z, w;
	}int4;

	class D3D11ParticleManager
	{
	public:
		D3D11ParticleManager() = default;

		static float RandF();
		static float RandF(float a, float b);
		static Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> CreateRandomTexture1DSRV(ID3D11Device* device);
		static Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> CreateRandomTexture2DSRV(ID3D11Device* device);
		static int Align(int value, int aligment);
		static void CalculateNumToEmit(std::shared_ptr<EmitterParams> emitter, std::shared_ptr<EmissionRate> emissionRate, float frameTime);

		void Initialize(const std::shared_ptr<D3D11Device> device, std::shared_ptr<D3D11ResourceManager> resourceManager, std::shared_ptr<D3D11CameraManager> cameraManager);
		void Reset();
		void UpdatePerFrame();
		void BindFrameCB();
		void Emit();
		void Simulate();
		void Sort();
		void Render();

		void AddEmitter(size_t id, EmitterParams emitter, EmissionRate emissionRate);
		void SetActive(size_t id, bool bIsActive);
		void DeleteEmitter(size_t id);
		void SetFrameTime(float frameTime);
		float GetFrameTime() const;

	private:
		int readCounter(ID3D11UnorderedAccessView* uav);

		bool sortInitial(unsigned int maxSize);
		bool sortIncremental(unsigned int presorted, unsigned int maxSize);

	private:
		enum { MAX_PARTICLE = 400 * 1024 };

		std::shared_ptr<D3D11Device> mDevice;
		std::shared_ptr<D3D11ResourceManager> mResourceManager;
		std::shared_ptr<D3D11CameraManager> mCameraManager;

		std::map<size_t, std::shared_ptr<EmitterParams>> mEmitters;
		std::map<size_t, std::shared_ptr<EmissionRate>> mEmissionRates;
		std::set<size_t> mActiveParticles;

		float mFrameTime;
		bool mbIsUdpatedFrameTime = true;
		bool mbIsReset = true;
		PER_FRAME_CONSTANT_BUFFER mGlobalConstantBuffer;

		// debugInfo
		int mNumDeadParticlesOnInit = 0;
		int mNumDeadParticlesAfterEmit = 0;
		int mNumDeadParticlesAfterSimulation = 0;
		int mNumActiveParticlesAfterSimulation = 0;

		std::shared_ptr<class D3D11RenderTargetView> mSwapChainRTV;
		std::shared_ptr<class D3D11DepthStencilView> mNoneDSV;
		std::shared_ptr<class D3D11DepthStencilView> mDSV;

		std::shared_ptr<class ShaderProgram> mRenderProgram;

		std::shared_ptr<class D3D11ComputeShader> mInitDeadListCS;
		std::shared_ptr<class D3D11ComputeShader> mInitParticlesCS;
		std::shared_ptr<class D3D11ComputeShader> mEmitCS;
		std::shared_ptr<class D3D11ComputeShader> mSimulateCS;

		std::shared_ptr<class D3D11ComputeShader> m_pCSSortStep;			// CS port of the VS/PS bitonic sort
		std::shared_ptr<class D3D11ComputeShader> m_pCSSort512;			// CS implementation to sort a number of 512 element sized arrays using a single dispatch
		std::shared_ptr<class D3D11ComputeShader> m_pCSSortInner512;		// CS implementation of the "down" pass from 512 to 1
		std::shared_ptr<class D3D11ComputeShader> m_pCSInitArgs;			// CS to write indirect args for Dispatch calls

		std::shared_ptr<class D3D11SamplerState> mLinearWrap;
		std::shared_ptr<class D3D11SamplerState> mPointClamp;

		Microsoft::WRL::ComPtr<ID3D11Buffer> mParticleBufferA;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mParticleBufferASRV;
		Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> mParticleBufferAUAV;

		Microsoft::WRL::ComPtr<ID3D11Buffer> mParticleBufferB;
		Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> mParticleBufferBUAV;

		Microsoft::WRL::ComPtr<ID3D11Buffer> mViewSpaceParticlePositions;;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mViewSpaceParticlePositionsSRV;
		Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> mViewSpaceParticlePositionsUAV;

		Microsoft::WRL::ComPtr<ID3D11Buffer> mMaxRadiusBuffer;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mMaxRadiusBufferSRV;
		Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> mMaxRadiusBufferUAV;

		Microsoft::WRL::ComPtr<ID3D11Buffer> mDeadListBuffer;
		Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> mDeadListUAV;

		Microsoft::WRL::ComPtr<ID3D11Buffer> mDebugCounterBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer> mDeadListCB;
		Microsoft::WRL::ComPtr<ID3D11Buffer> mActiveListCB;
		Microsoft::WRL::ComPtr<ID3D11Buffer> mEmitterCB;
		Microsoft::WRL::ComPtr<ID3D11Buffer> mPerFrameCB;
		Microsoft::WRL::ComPtr<ID3D11Buffer> mDispatchInfoCB;

		Microsoft::WRL::ComPtr<ID3D11Buffer> mAliveIndexBuffer;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mAliveIndexBufferSRV;
		Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> mAliveIndexBufferUAV;

		Microsoft::WRL::ComPtr<ID3D11Buffer> mIndirectDrawArgsBuffer;
		Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> mIndirectDrawArgsBufferUAV;

		Microsoft::WRL::ComPtr<ID3D11Buffer> mIndirectSortArgsBuffer;
		Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> mIndirectSortArgsBufferUAV;

		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mRandomTextureSRV;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mDepthSRV;
		std::shared_ptr<class D3D11Texture> mAtlasSRV;


	};
};