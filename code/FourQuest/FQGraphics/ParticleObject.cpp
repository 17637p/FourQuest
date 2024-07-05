#include "ParticleObject.h"
#include "D3D11Device.h"
#include "D3D11ResourceManager.h"
#include "D3D11Texture.h"
#include "../FQLoader/Util.h"

namespace fq::graphics
{
	ParticleObject::ParticleObject(std::shared_ptr<D3D11Device> device, std::shared_ptr<IParticleMaterial> iParticleMaterial, const ParticleInfo& particleInfo, const DirectX::SimpleMath::Matrix& transform)
		: mIParticleMaterial(iParticleMaterial)
		, mParticleInfo(particleInfo)
		, mTransform(transform)
		, mTimePos(0.f)
		, mFrameTime(0.f)
		, mAccumlation(0.f)
		, mNumToEmit(0)
		, mDebugInfo{ 0, }
		, mRandomSeed(rand() / (float)(rand()))
	{
		D3D11_BUFFER_DESC desc;
		desc.ByteWidth = sizeof(Particle) * ParticleInfo::MAX_PARTICLE_COUNT;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		desc.StructureByteStride = sizeof(Particle);
		device->GetDevice()->CreateBuffer(&desc, nullptr, mParticleBuffer.GetAddressOf());

		D3D11_SHADER_RESOURCE_VIEW_DESC srv;
		srv.Format = DXGI_FORMAT_UNKNOWN;
		srv.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
		srv.Buffer.ElementOffset = 0;
		srv.Buffer.ElementWidth = ParticleInfo::MAX_PARTICLE_COUNT;
		device->GetDevice()->CreateShaderResourceView(mParticleBuffer.Get(), &srv, mParticleBufferSRV.GetAddressOf());

		D3D11_UNORDERED_ACCESS_VIEW_DESC uav;
		uav.Format = DXGI_FORMAT_UNKNOWN;
		uav.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		uav.Buffer.FirstElement = 0;
		uav.Buffer.NumElements = ParticleInfo::MAX_PARTICLE_COUNT;
		uav.Buffer.Flags = 0;
		device->GetDevice()->CreateUnorderedAccessView(mParticleBuffer.Get(), &uav, mParticleBufferUAV.GetAddressOf());

		// 죽은 파티클 인덱스 목록 생성
		desc.ByteWidth = sizeof(UINT) * ParticleInfo::MAX_PARTICLE_COUNT;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		desc.StructureByteStride = sizeof(UINT);
		device->GetDevice()->CreateBuffer(&desc, nullptr, mDeadListBuffer.GetAddressOf());
		uav.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_APPEND; // 추가 버퍼로 생성
		device->GetDevice()->CreateUnorderedAccessView(mDeadListBuffer.Get(), &uav, mDeadListUAV.GetAddressOf());

		// 정렬할 살아있는 파티클의 인덱스 버퍼
		desc.ByteWidth = sizeof(unsigned int) * ParticleInfo::MAX_PARTICLE_COUNT;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		desc.StructureByteStride = sizeof(unsigned int);
		device->GetDevice()->CreateBuffer(&desc, nullptr, mAliveIndexBuffer.GetAddressOf());

		srv.Format = DXGI_FORMAT_UNKNOWN;
		srv.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
		srv.Buffer.ElementOffset = 0;
		srv.Buffer.ElementWidth = ParticleInfo::MAX_PARTICLE_COUNT;
		device->GetDevice()->CreateShaderResourceView(mAliveIndexBuffer.Get(), &srv, mAliveIndexBufferSRV.GetAddressOf());

		uav.Buffer.NumElements = ParticleInfo::MAX_PARTICLE_COUNT;
		uav.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_COUNTER;
		uav.Format = DXGI_FORMAT_UNKNOWN;
		device->GetDevice()->CreateUnorderedAccessView(mAliveIndexBuffer.Get(), &uav, mAliveIndexBufferUAV.GetAddressOf());
	}

	void ParticleObject::reset()
	{
		mParticleInfo.InstanceData.bIsReset = false;
		mTimePos = 0.f;
		mFrameTime = 0.f;
		mAccumlation = 0.f;
		mNumToEmit = 0;
	}

	void ParticleObject::caculateNumToEmit()
	{
		float frameTime = mFrameTime * mParticleInfo.MainData.SimulationSpeed;

		float prevModTimePos = fmod(mTimePos, mParticleInfo.MainData.Duration);
		mTimePos += frameTime;
		float modTimePos = fmod(mTimePos, mParticleInfo.MainData.Duration);

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
				mStartTimePos = mParticleInfo.MainData.StartDelay.x;
				break;
			case ParticleInfo::EOption::RandomBetweenTwoConstant:
			{
				float ratio = D3D11Util::RandF(0.f, 1.f);
				mStartTimePos = std::lerp(mParticleInfo.MainData.StartDelay.x, mParticleInfo.MainData.StartDelay.y, ratio);
				break;
			}
			default:
				break;
			}

			mbIsRunDuationCycle = false;
		}

		if (mStartTimePos > modTimePos)
		{
			return;
		}

		if (mParticleInfo.InstanceData.bIsEmit)
		{
			mAccumlation += mParticleInfo.EmissionData.ParticlesPerSecond * frameTime;
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

				if (prevModTimePos < checkedTimePos && checkedTimePos < modTimePos)
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
}