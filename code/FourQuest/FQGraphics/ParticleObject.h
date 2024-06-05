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
		ParticleObject(std::shared_ptr<D3D11Device> device, std::shared_ptr<D3D11ResourceManager> resourceManager, const ParticleInfo& particleInfo, const DirectX::SimpleMath::Matrix& transform);
		virtual ~ParticleObject() = default;

		void SetInfo(const ParticleInfo& info);
		void SetIsReset(bool bIsReset) { mbIsReset = bIsReset; }
		void SetFrameTime(float frameTime) { mFrameTime = frameTime; }
		void SetTransform(const DirectX::SimpleMath::Matrix& transform) { mTransform = transform; }
		void SetIsEmit(bool bIsEmit) { mbIsEmit = bIsEmit; }

		const ParticleInfo& GetInfo() const { return mParticleInfo; }
		bool GetIsReset() const { return mbIsReset; }
		float GetFrameTIme() const { return mFrameTime; }
		DirectX::SimpleMath::Matrix GetTransform() const { return mTransform; }
		bool GetIsEmit() const { return mbIsEmit; }

		DebugInfo& GetDebugInfo() { return mDebugInfo; }

	private:
		void reset()
		{
			mbIsReset = false;
			mTimePos = 0.f;
			mFrameTime = 0.f;
			mAccumlation = 0.f;
			mNumToEmit = 0;
		}

		void caculateNumToEmit()
		{
			mTimePos += mFrameTime;

			if (mParticleInfo.EmissionData.ParticlesPerSecond < 0.f)
			{
				return;
			}
			if (!mParticleInfo.MainData.bIsLooping && mParticleInfo.MainData.Duration < mTimePos)
			{
				return;
			}

			if (mCycleCount < mTimePos / mParticleInfo.MainData.Duration)
			{
				mCycleCount = mTimePos / mParticleInfo.MainData.Duration;
				mbIsRunDuationCycle = true;
			}

			if (mbIsRunDuationCycle)
			{
				switch (mParticleInfo.MainData.StartDelayOption)
				{
				case ParticleInfo::EOption::Constant:
					mStartTimePos = mParticleInfo.MainData.StartDelay[0];
					break;
				case ParticleInfo::EOption::RandomBetweenTwoConstant:
				{
					float ratio = D3D11Util::RandF(0.f, 1.f);
					mStartTimePos = std::lerp(mParticleInfo.MainData.StartDelay[0], mParticleInfo.MainData.StartDelay[1], ratio);
					break;
				}
				default:
					break;
				}

				mbIsRunDuationCycle = false;
			}

			float modTimePos = fmod(mTimePos, mParticleInfo.MainData.Duration);
			const float prevModTimePos = modTimePos - mTimePos;

			if (mStartTimePos > modTimePos)
			{
				return;
			}

			if (mbIsEmit)
			{
				mAccumlation += mParticleInfo.EmissionData.ParticlesPerSecond * mFrameTime;
			}

			if (mAccumlation > 1.f)
			{
				double integerPart;

				mAccumlation = modf(mAccumlation, &integerPart);
				mNumToEmit = (int)integerPart;
			}
			else
			{
				mNumToEmit = 0;
			}

			for (const ParticleInfo::Emission::Burst& burst : mParticleInfo.EmissionData.Bursts)
			{
				for (size_t i = 0; i < burst.Cycles; ++i)
				{
					float checkedTimePos = burst.TimePos + burst.Interval * i;

					if (prevModTimePos < checkedTimePos && checkedTimePos < mTimePos)
					{
						float ratio = D3D11Util::RandF(0.f, 1.f);

						if (burst.Probability >= ratio)
						{
							mNumToEmit += burst.Count;
						}
					}
				}
			}
		}

	private:
		std::shared_ptr<D3D11ResourceManager> mResourceManager;

		Microsoft::WRL::ComPtr<ID3D11Buffer> mParticleBuffer;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mParticleBufferSRV;
		Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> mParticleBufferUAV;

		Microsoft::WRL::ComPtr<ID3D11Buffer> mDeadListBuffer;
		Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> mDeadListUAV;

		Microsoft::WRL::ComPtr<ID3D11Buffer> mAliveIndexBuffer;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mAliveIndexBufferSRV;
		Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> mAliveIndexBufferUAV;

		std::shared_ptr<D3D11Texture> mTexture;
		DirectX::SimpleMath::Matrix mTransform;
		ParticleInfo mParticleInfo;
		bool mbIsReset;

		float mTimePos;
		float mFrameTime;
		float mAccumlation = 0.f;
		int	mNumToEmit;
		DebugInfo mDebugInfo;
		bool mbIsEmit;

		bool mbIsRunDuationCycle;
		float mStartTimePos;
		size_t mCycleCount;

		float mRandomSeed;
	};
}

