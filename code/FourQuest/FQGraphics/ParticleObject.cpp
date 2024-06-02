#include "ParticleObject.h"
#include "D3D11Device.h"

namespace fq::graphics
{
	ParticleObject::ParticleObject(std::shared_ptr<D3D11Device> device, std::shared_ptr<D3D11Texture> texture, const ParticleInfo& particleInfo, const DirectX::SimpleMath::Matrix& transform)
		: mTexture(texture)
		, mParticleInfo(particleInfo)
		, mbIsReset(true)
		, mTimePos(0.f)
		, mFrameTime(0.f)
		, mAccumlation(0.f)
		, mNumToEmit(0)
		, mDebugInfo{ 0, }
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
}