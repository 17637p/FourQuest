#pragma once

#include <cmath>

#include "CommonHeader.h"
#include "../FQCommon/IFQRenderObject.h"
#include "../FQCommon/FQCommonGraphics.h"
#include "D3D11Util.h"

namespace fq::graphics
{
	class D3D11Device;
	class D3D11Texture;
	class D3D11ResourceManager;

	struct Particle
	{
		DirectX::SimpleMath::Vector3 PositionW; // 12
		float Size; // 4

		DirectX::SimpleMath::Vector3 Velocity; // 12
		float Rotation; // 4

		DirectX::SimpleMath::Vector4 StartColor; // 16
		DirectX::SimpleMath::Vector4 Color; // 16

		DirectX::SimpleMath::Vector3 EmitterPosition; // 16
		float GravityModfier; // 4

		float LifeTime; // 4
		float Age;// 4
	};

	class ParticleObject : public IParticleObject
	{
		friend class D3D11ParticleManager;

	public:
		struct DebugInfo
		{
			int NumDeadParticlesOnInit = 0;
			int NumDeadParticlesAfterEmit = 0;
			int NumDeadParticlesAfterSimulation = 0;
			int NumActiveParticlesAfterSimulation = 0;
		};

	public:
		ParticleObject(std::shared_ptr<D3D11Device> device, const DirectX::SimpleMath::Matrix& transform, const ParticleInfo& particleInfo, std::shared_ptr<IParticleMaterial> iParticleMaterial);
		virtual ~ParticleObject() = default;

		// Info
		void SetInfo(const ParticleInfo& info) override { mParticleInfo = info; }
		const ParticleInfo& GetInfo() const override { return mParticleInfo; }

		// Transform
		void SetTransform(const DirectX::SimpleMath::Matrix& transform)override { mTransform = transform; }
		DirectX::SimpleMath::Matrix GetTransform() const override { return mTransform; }

		// Material
		virtual void SetIParticleMaterial(std::shared_ptr<IParticleMaterial> iParticleMaterial) override { mIParticleMaterial = iParticleMaterial; }
		virtual std::shared_ptr<IParticleMaterial> GetIParticleMaterial() const override { return mIParticleMaterial; }

		// FrameTime
		void SetFrameTime(float frameTime) override { mFrameTime = frameTime; }
		float GetFrameTIme() const override { return mFrameTime; }

		DebugInfo& GetDebugInfo() { return mDebugInfo; }

	private:
		void reset();
		void caculateNumToEmit();

	private:
		Microsoft::WRL::ComPtr<ID3D11Buffer> mParticleBuffer;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mParticleBufferSRV;
		Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> mParticleBufferUAV;

		Microsoft::WRL::ComPtr<ID3D11Buffer> mDeadListBuffer;
		Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> mDeadListUAV;

		Microsoft::WRL::ComPtr<ID3D11Buffer> mAliveIndexBuffer;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mAliveIndexBufferSRV;
		Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> mAliveIndexBufferUAV;

		DirectX::SimpleMath::Matrix mTransform;
		ParticleInfo mParticleInfo;
		std::shared_ptr<IParticleMaterial> mIParticleMaterial;

		float mTimePos;
		float mFrameTime;
		float mAccumlation = 0.f;
		int	mNumToEmit;
		bool mbIsRunDuationCycle;
		float mStartTimePos;
		size_t mCycleCount;

		float mRandomSeed;

		DebugInfo mDebugInfo;
	};
}

