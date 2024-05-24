#pragma once

#include "CommonHeader.h"
#include "../FQCommon/FQCommonGraphics.h"

namespace fq::graphics
{
	class D3D11Device;
	class D3D11ResourceManager;
	class D3D11CameraManager;

	

#define MAX_ACTIVE_PARTICLE	 10

	struct PER_FRAME_CONSTANT_BUFFER
	{
		DirectX::SimpleMath::Vector4 m_StartColor[MAX_ACTIVE_PARTICLE];
		DirectX::SimpleMath::Vector4 m_EndColor[MAX_ACTIVE_PARTICLE];
		DirectX::SimpleMath::Vector4 m_EmitterLightingCenter[MAX_ACTIVE_PARTICLE];

		DirectX::SimpleMath::Matrix m_ViewProjection;
		DirectX::SimpleMath::Matrix m_ViewProjInv;
		DirectX::SimpleMath::Matrix m_View;
		DirectX::SimpleMath::Matrix m_ViewInv;
		DirectX::SimpleMath::Matrix m_Projection;
		DirectX::SimpleMath::Matrix m_ProjectionInv;

		DirectX::XMVECTOR	m_EyePosition;
		DirectX::XMVECTOR	m_SunDirection;
		DirectX::XMVECTOR	m_SunColor;
		DirectX::XMVECTOR	m_AmbientColor;

		DirectX::XMVECTOR	m_SunDirectionView;
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
	} int4;

	class ParticleObject;

	class D3D11ParticleManager
	{
	public:
		D3D11ParticleManager() = default;

		static float RandF();
		static float RandF(float a, float b);
		static Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> CreateRandomTexture1DSRV(ID3D11Device* device);
		static Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> CreateRandomTexture2DSRV(ID3D11Device* device);
		static int Align(int value, int aligment);
		static void CalculateNumToEmit(std::shared_ptr<ParticleObject> particleObject, float frameTime);

		void Excute();

		void Initialize(const std::shared_ptr<D3D11Device> device, std::shared_ptr<D3D11ResourceManager> resourceManager, std::shared_ptr<D3D11CameraManager> cameraManager);
		void Reset(std::shared_ptr<ParticleObject> particleObject);
		void UpdatePerFrame();
		void BindFrameCB();
		void Emit(std::shared_ptr<ParticleObject> particleObject);
		void Simulate(std::shared_ptr<ParticleObject> particleObject);
		void Render(std::shared_ptr<ParticleObject> particleObject);

		void AddEmitter(size_t id, ParticleInfo particleInfo);
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

		std::map<size_t, std::shared_ptr<ParticleObject>> mParticleObjects;

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

		std::shared_ptr<class D3D11ComputeShader> m_pCSSortStep;
		std::shared_ptr<class D3D11ComputeShader> m_pCSSort512;
		std::shared_ptr<class D3D11ComputeShader> m_pCSSortInner512;
		std::shared_ptr<class D3D11ComputeShader> m_pCSInitArgs;

		std::shared_ptr<class D3D11SamplerState> mLinearWrap;
		std::shared_ptr<class D3D11SamplerState> mPointClamp;

		Microsoft::WRL::ComPtr<ID3D11Buffer> mDebugCounterBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer> mDeadListCB;
		Microsoft::WRL::ComPtr<ID3D11Buffer> mActiveListCB;
		Microsoft::WRL::ComPtr<ID3D11Buffer> mEmitterCB;
		Microsoft::WRL::ComPtr<ID3D11Buffer> mPerFrameCB;
		Microsoft::WRL::ComPtr<ID3D11Buffer> mDispatchInfoCB;


		Microsoft::WRL::ComPtr<ID3D11Buffer> mIndirectDrawArgsBuffer;
		Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> mIndirectDrawArgsBufferUAV;

		Microsoft::WRL::ComPtr<ID3D11Buffer> mIndirectSortArgsBuffer;
		Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> mIndirectSortArgsBufferUAV;

		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mRandomTextureSRV;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mDepthSRV;
		std::shared_ptr<class D3D11Texture> mAtlasSRV;


	};
};