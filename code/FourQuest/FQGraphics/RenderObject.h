#pragma once

#include <algorithm>
#include <memory>
#include <array>
#include <vector>
#include <cmath>
#include <d3d11.h>
#include <wrl.h>

#include "Mesh.h"
#include "BoneHierarchy.h"
#include "AnimationHelper.h"
#include "D3D11Texture.h"
#include "D3D11Device.h"

#include "../FQCommon/IFQRenderObject.h"
#include "../FQCommon/FQCommonGraphics.h"

namespace fq::graphics
{
	class StaticMesh;
	class SkinnedMesh;
	class Material;
	class TerrainMaterial;
	struct TerrainMaterialInfo;

	class StaticMeshObject : public IStaticMeshObject
	{
	public:
		StaticMeshObject(std::shared_ptr<StaticMesh> staticMesh,
			std::vector<std::shared_ptr<Material>> materials,
			DirectX::SimpleMath::Matrix transform);
		~StaticMeshObject() = default;

		inline virtual void UpdateTransform(const DirectX::SimpleMath::Matrix& transform) override;

		inline virtual void SetTransform(const DirectX::SimpleMath::Matrix& transform) override;
		inline virtual void SetObjectRenderType(EObjectRenderType renderType) override;
		inline virtual void SetAlpha(float alpha) override;
		inline virtual void SetUseShadow(bool bUseShadow) override;
		inline virtual void SetAnimationTime(float timePos) override;
		inline virtual bool SetAnimationKey(const std::string& animationKey) override;
		inline virtual bool SetBlendAnimationKey(const std::array<std::string, 2> animationKeys) override;
		inline virtual bool SetBlendAnimationKey(const std::string& lhsAnimationKey, const std::string& rhsAnimationKey) override;
		inline virtual void SetBlendAnimationTime(const std::array<float, 2>& timePos, float blendWeight) override;
		inline virtual void SetBlendAnimationTime(float timePos, float blendTimePos, float blendWeight) override;

		inline virtual const DirectX::SimpleMath::Matrix& GetTransform() const override;
		inline virtual EObjectRenderType GetObjectRenderType() const override;
		inline virtual float GetAlpha() const override;
		inline virtual bool GetUseShadow() const override;
		inline virtual DirectX::BoundingBox GetRenderBoundingBox() const override;
		inline virtual DirectX::BoundingSphere GetRenderBoundingSphere() const override;
		inline virtual float GetAnimationTime() const override;
		inline virtual float GetBlendTime() const override;

		inline const std::shared_ptr<StaticMesh>& GetStaticMesh() const;
		inline const std::vector<std::shared_ptr<Material>>& GetMaterials() const;
		inline void AddAnimation(std::string animationKey, std::shared_ptr<fq::common::AnimationClip> animationClip);

	private:
		std::shared_ptr<StaticMesh> mStaticMesh;
		std::vector<std::shared_ptr<Material>> mMaterials;
		DirectX::SimpleMath::Matrix mTransform;
		EObjectRenderType mObjectRenderType;
		float mAlpha;
		bool mbUseShadow;

		float mTimePos;
		std::map<std::string, std::shared_ptr<fq::common::AnimationClip>> mAnimationMap;
		const fq::common::NodeClip* mNodeCache;
		std::shared_ptr<fq::common::AnimationClip> mClipCache;

		const fq::common::NodeClip* mBlendNodeCache;
		std::shared_ptr<fq::common::AnimationClip> mBlendClipCache;
		float mBlendTimePos;
	};

#pragma region inlineFunc

	void StaticMeshObject::UpdateTransform(const DirectX::SimpleMath::Matrix& transform)
	{
		SetTransform(transform);
	}

	inline void StaticMeshObject::SetTransform(const DirectX::SimpleMath::Matrix& transform)
	{
		mTransform = transform;
	}
	inline void StaticMeshObject::SetObjectRenderType(EObjectRenderType renderType)
	{
		mObjectRenderType = renderType;
	}
	inline void StaticMeshObject::SetAlpha(float alpha)
	{
		mAlpha = std::clamp<float>(alpha, 0.f, 1.f);
	}
	inline void StaticMeshObject::SetUseShadow(bool bUseShadow)
	{
		mbUseShadow = bUseShadow;
	}
	inline void StaticMeshObject::SetAnimationTime(float timePos)
	{
		mTimePos = timePos;

		if (mNodeCache != nullptr && mClipCache != nullptr)
		{
			fq::common::Keyframe lhs;
			fq::common::Keyframe rhs;
			float weight;

			AnimationHelper::FindKeyframe(mNodeCache->Keyframes, *mClipCache, timePos, &lhs, &rhs, &weight);
			fq::common::Keyframe keyframe = AnimationHelper::Interpolate(lhs, rhs, weight);
			mTransform = AnimationHelper::CreateMatrix(keyframe);
		}
	}
	inline bool StaticMeshObject::SetAnimationKey(const std::string& animationKey)
	{
		auto find = mAnimationMap.find(animationKey);

		if (find == mAnimationMap.end())
		{
			return false;
		}

		const std::vector<fq::common::NodeClip>& nodeClips = find->second->NodeClips;
		auto nodeClip = std::find_if(nodeClips.begin(), nodeClips.end(), [&](const fq::common::NodeClip& nodeClip) { return nodeClip.NodeName == mStaticMesh->GetMeshData().NodeName; });

		mClipCache = find->second;

		if (nodeClip != nodeClips.end())
		{
			mNodeCache = &(*nodeClip);
		}

		return true;
	}
	inline bool StaticMeshObject::SetBlendAnimationKey(const std::array<std::string, 2> animationKeys)
	{
		return SetBlendAnimationKey(animationKeys[0], animationKeys[1]);
	}
	inline bool StaticMeshObject::SetBlendAnimationKey(const std::string& animationKey, const std::string& blendAnimationKey)
	{
		auto find = mAnimationMap.find(animationKey);

		if (find == mAnimationMap.end())
		{
			return false;
		}

		auto secondFind = mAnimationMap.find(blendAnimationKey);

		if (secondFind == mAnimationMap.end())
		{
			return false;
		}

		const std::vector<fq::common::NodeClip>& nodeClips = find->second->NodeClips;
		auto nodeClip = std::find_if(nodeClips.begin(), nodeClips.end(), [&](const fq::common::NodeClip& nodeClip) { return nodeClip.NodeName == mStaticMesh->GetMeshData().NodeName; });
		mClipCache = find->second;

		if (nodeClip != nodeClips.end())
		{
			mNodeCache = &(*nodeClip);
		}

		const std::vector<fq::common::NodeClip>& secondNodeClips = secondFind->second->NodeClips;
		auto secondNodeClip = std::find_if(secondNodeClips.begin(), secondNodeClips.end(), [&](const fq::common::NodeClip& nodeClip) { return nodeClip.NodeName == mStaticMesh->GetMeshData().NodeName; });
		mBlendClipCache = secondFind->second;

		if (secondNodeClip != secondNodeClips.end())
		{
			mBlendNodeCache = &(*secondNodeClip);
		}

		return true;
	}
	inline void StaticMeshObject::SetBlendAnimationTime(const std::array<float, 2>& timePos, float blendWeight)
	{
		SetBlendAnimationTime(timePos[0], timePos[1], blendWeight);
	}
	inline void StaticMeshObject::SetBlendAnimationTime(float timePos, float blendTimePos, float blendWeight)
	{
		mTimePos = timePos;
		mBlendTimePos = blendTimePos;

		if (mNodeCache == nullptr && mBlendNodeCache == nullptr)
		{
			return;
		}

		blendWeight = std::min<float>(blendWeight, 1.f);
		blendWeight = std::max<float>(blendWeight, 0.f);

		fq::common::Keyframe srcAnimation;
		fq::common::Keyframe destAnimation;

		if (mNodeCache != nullptr && mClipCache != nullptr)
		{
			fq::common::Keyframe lhs;
			fq::common::Keyframe rhs;
			float weight;

			AnimationHelper::FindKeyframe(mNodeCache->Keyframes, *mClipCache, timePos, &lhs, &rhs, &weight);
			srcAnimation = AnimationHelper::Interpolate(lhs, rhs, weight);
		}

		if (mBlendNodeCache != nullptr && mBlendClipCache != nullptr)
		{
			fq::common::Keyframe lhs;
			fq::common::Keyframe rhs;
			float weight;

			AnimationHelper::FindKeyframe(mBlendNodeCache->Keyframes, *mBlendClipCache, timePos, &lhs, &rhs, &weight);
			destAnimation = AnimationHelper::Interpolate(lhs, rhs, weight);
		}

		fq::common::Keyframe resultKeyframe = AnimationHelper::Interpolate(srcAnimation, destAnimation, blendWeight);
		mTransform = AnimationHelper::CreateMatrix(resultKeyframe);
	}

	inline const std::shared_ptr<StaticMesh>& StaticMeshObject::GetStaticMesh() const
	{
		return mStaticMesh;
	}
	inline EObjectRenderType StaticMeshObject::GetObjectRenderType() const
	{
		return mObjectRenderType;
	}
	inline float StaticMeshObject::GetAlpha() const
	{
		return mAlpha;
	}
	inline bool StaticMeshObject::GetUseShadow() const
	{
		return mbUseShadow;
	}
	inline DirectX::BoundingBox StaticMeshObject::GetRenderBoundingBox() const
	{
		return mStaticMesh->GetMeshData().RenderBoundingBox;
	}
	inline DirectX::BoundingSphere StaticMeshObject::GetRenderBoundingSphere() const
	{
		return mStaticMesh->GetMeshData().GetRenderBoundingSphere;
	}
	inline float StaticMeshObject::GetAnimationTime() const
	{
		return mTimePos;
	}
	inline float StaticMeshObject::GetBlendTime() const
	{
		return mBlendTimePos;
	}
	inline const std::vector<std::shared_ptr<Material>>& StaticMeshObject::GetMaterials() const
	{
		return mMaterials;
	}
	inline const DirectX::SimpleMath::Matrix& StaticMeshObject::GetTransform() const
	{
		return mTransform;
	}
	inline void StaticMeshObject::AddAnimation(std::string animationKey, std::shared_ptr<fq::common::AnimationClip> animationClip)
	{
		mAnimationMap.insert({ animationKey, animationClip });
	}

#pragma endregion

	class SkinnedMeshObject : public ISkinnedMeshObject
	{
	public:
		SkinnedMeshObject(std::shared_ptr<SkinnedMesh> SkinnedMesh,
			std::vector<std::shared_ptr<Material>> materials,
			DirectX::SimpleMath::Matrix transform,
			BoneHierarchyCache boneHierarchyCache);
		~SkinnedMeshObject() = default;

		inline virtual void UpdateTransform(const DirectX::SimpleMath::Matrix& transform) override;
		inline virtual void UpdateAnimationTime(float timePos) override;

		inline virtual void SetBindPose() override;
		inline virtual void SetTransform(const DirectX::SimpleMath::Matrix& transform) override;
		inline virtual void SetAnimationTime(float timePos) override;
		inline virtual void SetObjectRenderType(EObjectRenderType renderType) override;
		inline virtual void SetAlpha(float alpha) override;
		inline virtual void SetUseShadow(bool bUseShadow) override;
		inline virtual bool SetAnimationKey(const std::string& animationKey) override;
		inline virtual bool SetBlendAnimationKey(const std::array<std::string, 2> animKey) override;
		inline virtual bool SetBlendAnimationKey(const std::string& animKey, const std::string& blendAnimKey) override;
		inline virtual void SetBlendAnimationTime(const std::array<float, 2>& timePos, float blendWeight) override;
		inline virtual void SetBlendAnimationTime(float timePos, float blendTimePos, float blendWeight) override;

		inline virtual const DirectX::SimpleMath::Matrix& GetTransform() const override;
		inline virtual float GetAnimationTime() const override;
		inline virtual EObjectRenderType GetObjectRenderType() const override;
		inline virtual float GetAlpha() const override;
		inline virtual bool GetUseShadow() const override;
		inline virtual std::set<std::string> GetAnimationKeys() const override;
		inline virtual DirectX::BoundingBox GetRenderBoundingBox() const override;
		inline virtual DirectX::BoundingSphere GetRenderBoundingSphere() const override;
		inline virtual float GetBlendTime() const override;

		inline void AddAnimation(std::string animationKey, std::shared_ptr<fq::common::AnimationClip> animationClip);
		inline const std::shared_ptr<SkinnedMesh>& GetSkinnedMesh() const;
		inline const std::vector<std::shared_ptr<Material>>& GetMaterials() const;
		inline const std::vector<DirectX::SimpleMath::Matrix>& GetFinalTransforms() const;

	private:
		std::shared_ptr<SkinnedMesh> mSkinnedMesh;
		std::vector<std::shared_ptr<Material>> mMaterials;
		DirectX::SimpleMath::Matrix mTransform;
		float mTimePos;
		float mBlendTimePos;
		BoneHierarchyCache mBoneHierarchyCache;
		std::map<std::string, std::shared_ptr<fq::common::AnimationClip>> mAnimationMap;
		EObjectRenderType mObjectRenderType;
		float mAlpha;
		bool mbUseShadow;
	};

#pragma region inlineFunc
	inline void SkinnedMeshObject::UpdateTransform(const DirectX::SimpleMath::Matrix& transform)
	{
		mTransform = transform;
	}
	inline void SkinnedMeshObject::UpdateAnimationTime(float timePos)
	{
		mTimePos = timePos;
		mBoneHierarchyCache.Update(timePos);
	}

	inline void SkinnedMeshObject::AddAnimation(std::string animationKey, std::shared_ptr<fq::common::AnimationClip> animationClip)
	{
		mAnimationMap.insert({ animationKey, animationClip });
	}

	inline bool SkinnedMeshObject::SetAnimationKey(const std::string& animationKey)
	{
		auto find = mAnimationMap.find(animationKey);

		if (find == mAnimationMap.end())
		{
			return false;
		}

		mBoneHierarchyCache.SetAnimation(find->second);

		return true;
	}
	inline bool SkinnedMeshObject::SetBlendAnimationKey(const std::array<std::string, 2> animationKeys)
	{
		return SetBlendAnimationKey(animationKeys[0], animationKeys[1]);
	}
	inline bool SkinnedMeshObject::SetBlendAnimationKey(const std::string& srcAnimationKey, const std::string& destAnimationKey)
	{
		auto find = mAnimationMap.find(srcAnimationKey);

		if (find == mAnimationMap.end())
		{
			return false;
		}

		auto secondFind = mAnimationMap.find(destAnimationKey);

		if (secondFind == mAnimationMap.end())
		{
			return false;
		}

		mBoneHierarchyCache.SetAnimation(find->second, secondFind->second);

		return true;
	}

	inline void SkinnedMeshObject::SetBindPose()
	{
		mTimePos = 0.f;
		mBoneHierarchyCache.Clear();
	}
	inline void SkinnedMeshObject::SetTransform(const DirectX::SimpleMath::Matrix& transform)
	{
		mTransform = transform;
	}
	inline void SkinnedMeshObject::SetAnimationTime(float timePos)
	{
		mTimePos = timePos;
		mBoneHierarchyCache.Update(timePos);
	}
	inline void SkinnedMeshObject::SetBlendAnimationTime(const std::array<float, 2>& timePos, float blendWeight)
	{
		SetBlendAnimationTime(timePos[0], timePos[1], blendWeight);
	}
	inline void SkinnedMeshObject::SetBlendAnimationTime(float lhsTimePos, float rhsTimePos, float blendWeight)
	{
		mTimePos = lhsTimePos;
		mBlendTimePos = rhsTimePos;
		blendWeight = std::min<float>(blendWeight, 1.f);
		blendWeight = std::max<float>(blendWeight, 0.f);
		mBoneHierarchyCache.Update(mTimePos, mBlendTimePos, blendWeight);
	}
	inline void SkinnedMeshObject::SetObjectRenderType(EObjectRenderType renderType)
	{
		mObjectRenderType = renderType;
	}
	inline const DirectX::SimpleMath::Matrix& SkinnedMeshObject::GetTransform() const
	{
		return mTransform;
	}
	inline float SkinnedMeshObject::GetAnimationTime() const
	{
		return mTimePos;
	}
	inline  std::set<std::string> SkinnedMeshObject::GetAnimationKeys() const
	{
		std::set<std::string> animationKeys;

		for (const auto& [name, clip] : mAnimationMap)
		{
			animationKeys.insert(name);
		}

		return animationKeys;
	}
	inline DirectX::BoundingBox SkinnedMeshObject::GetRenderBoundingBox() const
	{
		return mSkinnedMesh->GetMeshData().RenderBoundingBox;
	}
	inline DirectX::BoundingSphere SkinnedMeshObject::GetRenderBoundingSphere() const
	{
		return mSkinnedMesh->GetMeshData().GetRenderBoundingSphere;
	}
	inline float SkinnedMeshObject::GetBlendTime() const
	{
		return mBlendTimePos;
	}
	inline  EObjectRenderType SkinnedMeshObject::GetObjectRenderType() const
	{
		return mObjectRenderType;
	}
	inline float SkinnedMeshObject::GetAlpha() const
	{
		return mAlpha;
	}
	inline bool SkinnedMeshObject::GetUseShadow() const
	{
		return mbUseShadow;
	}
	inline const std::shared_ptr<SkinnedMesh>& SkinnedMeshObject::GetSkinnedMesh() const
	{
		return mSkinnedMesh;
	}
	inline const std::vector<std::shared_ptr<Material>>& SkinnedMeshObject::GetMaterials() const
	{
		return mMaterials;
	}
	inline const std::vector<DirectX::SimpleMath::Matrix>& SkinnedMeshObject::GetFinalTransforms() const
	{
		return mBoneHierarchyCache.GetFinalTransforms();
	}
	inline void SkinnedMeshObject::SetAlpha(float alpha)
	{
		mAlpha = std::clamp<float>(alpha, 0.f, 1.f);
	}
	inline void SkinnedMeshObject::SetUseShadow(bool bUseShadow)
	{
		mbUseShadow = bUseShadow;
	}

#pragma endregion

	class TerrainMeshObject : public ITerrainMeshObject
	{
	public:
		TerrainMeshObject(std::shared_ptr<StaticMesh> staticMesh,
			DirectX::SimpleMath::Matrix transform);
		~TerrainMeshObject() = default;

		inline virtual void SetTransform(const DirectX::SimpleMath::Matrix& transform) override;
		void SetTerrainMaterial(const std::shared_ptr<D3D11Device>& device,
			const TerrainMaterialInfo& terrainMaterial);

		inline virtual const DirectX::SimpleMath::Matrix& GetTransform() const override;
		inline virtual DirectX::BoundingBox GetRenderBoundingBox() const override;
		inline virtual DirectX::BoundingSphere GetRenderBoundingSphere() const override;

		inline const std::shared_ptr<StaticMesh>& GetStaticMesh() const;
		inline const std::shared_ptr<TerrainMaterial>& GetTerrainMaterial() const;

	private:
		std::shared_ptr<StaticMesh> mStaticMesh;
		std::shared_ptr<TerrainMaterial> mMaterial;

		DirectX::SimpleMath::Matrix mTransform;
	};

#pragma region inlineFunc
	inline void TerrainMeshObject::SetTransform(const DirectX::SimpleMath::Matrix& transform)
	{
		mTransform = transform;
	}
	inline const std::shared_ptr<StaticMesh>& TerrainMeshObject::GetStaticMesh() const
	{
		return mStaticMesh;
	}
	inline DirectX::BoundingBox TerrainMeshObject::GetRenderBoundingBox() const
	{
		return mStaticMesh->GetMeshData().RenderBoundingBox;
	}
	inline DirectX::BoundingSphere TerrainMeshObject::GetRenderBoundingSphere() const
	{
		return mStaticMesh->GetMeshData().GetRenderBoundingSphere;
	}
	inline const std::shared_ptr<TerrainMaterial>& TerrainMeshObject::GetTerrainMaterial() const
	{
		return mMaterial;
	}
	inline const DirectX::SimpleMath::Matrix& TerrainMeshObject::GetTransform() const
	{
		return mTransform;
	}
#pragma endregion inlineFunc

	class ImageObject : public IImageObject
	{
	public:
		ImageObject();
		virtual ~ImageObject();

		virtual void SetTransform(const DirectX::SimpleMath::Matrix& transform) override;
		virtual const DirectX::SimpleMath::Matrix& GetTransform() const override;

		virtual void SetStartX(float startX) override;
		virtual void SetStartY(float startY) override;
		virtual float GetStartX() override;
		virtual float GetStartY() override;

		virtual void SetWidth(float width) override;
		virtual void SetHeight(float height) override;
		virtual float GetWidth() override;
		virtual float GetHeight() override;

		virtual void SetLayer(unsigned int layer) override;
		virtual unsigned int GetLayer() override;

		virtual void SetXRatio(float xRatio) override;
		virtual float GetXRatio() override;
		virtual void SetYRatio(float yRatio) override;
		virtual float GetYRatio() override;

		virtual void SetTexturePath(const std::wstring& texturePath) override;
		virtual std::wstring GetTexturePath() override;

	protected:
		float mStartX;
		float mStartY;
		float mWidth;
		float mHeight;

		float mXRatio;
		float mYRatio;

		unsigned int mLayer;

		std::wstring mTexturePath;
		DirectX::SimpleMath::Matrix mTransform;
	};

	struct GPUParticlePartA
	{
		DirectX::SimpleMath::Vector4 Params[3];
	};

	struct GPUParticlePartB
	{
		DirectX::SimpleMath::Vector4 Params[3];
	};

	class ParticleObject
	{
	public:
		struct DebugInfo
		{
			int NumDeadParticlesOnInit = 0;
			int NumDeadParticlesAfterEmit = 0;
			int NumDeadParticlesAfterSimulation = 0;
			int NumActiveParticlesAfterSimulation = 0;
		};

	public:
		ParticleObject(std::shared_ptr<D3D11Device> device, std::shared_ptr<D3D11Texture> texture, const ParticleInfo& particleInfo, const DirectX::SimpleMath::Matrix& transform)
			: mTexture(texture)
			, mParticleInfo(particleInfo)
			, mTimePos(0.f)
			, mbIsReset(false)
		{

			D3D11_BUFFER_DESC desc;
			desc.ByteWidth = sizeof(GPUParticlePartA) * ParticleInfo::MAX_PARTICLE_COUNT;
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
			desc.CPUAccessFlags = 0;
			desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
			desc.StructureByteStride = sizeof(GPUParticlePartA);
			device->GetDevice()->CreateBuffer(&desc, nullptr, mParticleBufferA.GetAddressOf());

			desc.ByteWidth = sizeof(GPUParticlePartB) * ParticleInfo::MAX_PARTICLE_COUNT;
			desc.StructureByteStride = sizeof(GPUParticlePartB);
			device->GetDevice()->CreateBuffer(&desc, nullptr, mParticleBufferB.GetAddressOf());

			D3D11_SHADER_RESOURCE_VIEW_DESC srv;
			srv.Format = DXGI_FORMAT_UNKNOWN;
			srv.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
			srv.Buffer.ElementOffset = 0;
			srv.Buffer.ElementWidth = ParticleInfo::MAX_PARTICLE_COUNT;
			device->GetDevice()->CreateShaderResourceView(mParticleBufferA.Get(), &srv, mParticleBufferASRV.GetAddressOf());

			D3D11_UNORDERED_ACCESS_VIEW_DESC uav;
			uav.Format = DXGI_FORMAT_UNKNOWN;
			uav.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
			uav.Buffer.FirstElement = 0;
			uav.Buffer.NumElements = ParticleInfo::MAX_PARTICLE_COUNT;
			uav.Buffer.Flags = 0;
			device->GetDevice()->CreateUnorderedAccessView(mParticleBufferA.Get(), &uav, mParticleBufferAUAV.GetAddressOf());
			device->GetDevice()->CreateUnorderedAccessView(mParticleBufferB.Get(), &uav, mParticleBufferBUAV.GetAddressOf());

			// 파티클의 뷰 스페이스 위치는 시뮬레이션 중 캐시되므로 버퍼를 할당한다.
			desc.ByteWidth = 16 * ParticleInfo::MAX_PARTICLE_COUNT;
			desc.StructureByteStride = 16;
			device->GetDevice()->CreateBuffer(&desc, 0, mViewSpaceParticlePositions.GetAddressOf());
			device->GetDevice()->CreateShaderResourceView(mViewSpaceParticlePositions.Get(), &srv, mViewSpaceParticlePositionsSRV.GetAddressOf());
			device->GetDevice()->CreateUnorderedAccessView(mViewSpaceParticlePositions.Get(), &uav, mViewSpaceParticlePositionsUAV.GetAddressOf());

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

			struct IndexBufferElement
			{
				float		distanceSquard;	// 카메라와의 제곱 거리
				float		index;		// 전역 파티클 인덱스
			};

			// 정렬할 살아있는 파티클의 인덱스 버퍼
			desc.ByteWidth = sizeof(IndexBufferElement) * ParticleInfo::MAX_PARTICLE_COUNT;
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
			desc.CPUAccessFlags = 0;
			desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
			desc.StructureByteStride = sizeof(IndexBufferElement);
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

		void SetIsReset(bool bIsReset) { mbIsReset = bIsReset; }
		bool GetIsReset() const { return mbIsReset; }

		void SetInfo(const ParticleInfo& info) { mParticleInfo = info; }
		ParticleInfo& GetInfo() { return mParticleInfo; }
		const ParticleInfo& GetInfo() const { return mParticleInfo; }

		void AddTime(float deltaTime) { mTimePos += deltaTime; }
		void SetTime(float timePos) { mTimePos = timePos; }
		float GetTime() const { return mTimePos; }

		DebugInfo& GetDebugInfo() { return mDebugInfo; }

		void SetTransform(const DirectX::SimpleMath::Matrix& transform) { mTransform = transform; }
		DirectX::SimpleMath::Matrix GetTransform() const { return mTransform; }

	public:
		Microsoft::WRL::ComPtr<ID3D11Buffer> mParticleBufferA;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mParticleBufferASRV;
		Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> mParticleBufferAUAV;

		Microsoft::WRL::ComPtr<ID3D11Buffer> mParticleBufferB;
		Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> mParticleBufferBUAV;

		Microsoft::WRL::ComPtr<ID3D11Buffer> mViewSpaceParticlePositions;;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mViewSpaceParticlePositionsSRV;
		Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> mViewSpaceParticlePositionsUAV;

		Microsoft::WRL::ComPtr<ID3D11Buffer> mDeadListBuffer;
		Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> mDeadListUAV;

		Microsoft::WRL::ComPtr<ID3D11Buffer> mAliveIndexBuffer;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mAliveIndexBufferSRV;
		Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> mAliveIndexBufferUAV;

		DebugInfo mDebugInfo;

	private:
		std::shared_ptr<D3D11Texture> mTexture;
		ParticleInfo mParticleInfo;
		float mTimePos;
		bool mbIsReset;

		DirectX::SimpleMath::Matrix mTransform;
	};
}

