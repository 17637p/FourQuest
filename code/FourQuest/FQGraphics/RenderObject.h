#pragma once

#include <algorithm>
#include <memory>
#include <array>
#include <vector>
#include <cmath>
#include <d3d11.h>
#include <wrl.h>

#include "Mesh.h"
#include "NodeHierarchy.h"
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
	class D3D11JobManager;
	class D3D11VertexBuffer;

	class StaticMeshObject : public IStaticMeshObject
	{
	public:
		StaticMeshObject(std::shared_ptr<IStaticMesh> staticMesh,
			std::vector<std::shared_ptr<IMaterial>> materials,
			const MeshObjectInfo& info,
			const DirectX::SimpleMath::Matrix& transforms);
		~StaticMeshObject() = default;

		// Transform
		virtual void SetTransform(const DirectX::SimpleMath::Matrix& transform) override { mTransform = transform; }
		virtual const DirectX::SimpleMath::Matrix& GetTransform() const override { return mTransform; }

		// Info
		virtual void SetMeshObjectInfo(const MeshObjectInfo& info) override { mInfo = info; }
		virtual const MeshObjectInfo& GetMeshObjectInfo() const override { return mInfo; }

		// Material
		virtual void SetMaterials(const std::vector<std::shared_ptr<IMaterial>>& materials) override { mMaterials = materials; }
		virtual const std::vector<std::shared_ptr<IMaterial>>& GetMaterials() const override { return mMaterials; }

		// Mesh
		virtual void SetStaticMesh(std::shared_ptr<IStaticMesh> staticMesh) override { mStaticMesh = staticMesh; }
		virtual std::shared_ptr<IStaticMesh> GetStaticMesh() const override { return mStaticMesh; }

		// NodeHierarchy
		virtual void SetNodeHierarchyInstance(std::shared_ptr<INodeHierarchyInstance> nodeHierarchyInstance) override { mNodeHierarchyInstance = nodeHierarchyInstance; }
		virtual std::shared_ptr<INodeHierarchyInstance> GetNodeHierarchyInstance() const override { return mNodeHierarchyInstance; }
		virtual void SetReferenceBoneIndex(size_t index) override { mIndex = index; }
		virtual size_t GetReferenceBoneIndex() const override { return mIndex; }

		// UVAnimInstance
		virtual void SetUVAnimationInstance(std::shared_ptr<IUVAnimationInstance> uvAnimationInstance) { mIUVAnimationInstance = uvAnimationInstance; }
		virtual std::shared_ptr<IUVAnimationInstance> GetUVAnimationInstanceOrNull() const { return mIUVAnimationInstance; }

		// LightmapUV
		virtual void SetLightmapUVScaleOffset(const DirectX::SimpleMath::Vector4& scaleOffset) override { mScaleOffset = scaleOffset; }
		virtual const DirectX::SimpleMath::Vector4& GetLightmapUVScaleOffset() const override { return mScaleOffset; }

		virtual void SetLightmapIndex(unsigned int lightmapIndex) override { mLightmapIndex = lightmapIndex; }
		virtual unsigned int GetLightmapIndex() const override { return mLightmapIndex; }

		virtual void SetMaterialInstanceInfo(const MaterialInstanceInfo& materialInstanceInfo) override { mMaterialInstanceInfo = materialInstanceInfo; }
		virtual const MaterialInstanceInfo& GetMaterialInstanceInfo() const override { return mMaterialInstanceInfo; }

		// Render
		virtual void SetIsRender(bool bIsRender) override { mbIsRender = bIsRender; }
		virtual bool GetIsRender() const override { return mbIsRender; }

	private:
		std::shared_ptr<INodeHierarchyInstance> mNodeHierarchyInstance;
		size_t mIndex;
		std::shared_ptr<IStaticMesh> mStaticMesh;
		std::vector<std::shared_ptr<IMaterial>> mMaterials;
		MeshObjectInfo mInfo;
		DirectX::SimpleMath::Matrix mTransform;
		std::shared_ptr<IUVAnimationInstance> mIUVAnimationInstance;
		DirectX::SimpleMath::Vector4 mScaleOffset;
		unsigned int mLightmapIndex;
		MaterialInstanceInfo mMaterialInstanceInfo;
		bool mbIsRender;
	};

	class SkinnedMeshObject : public ISkinnedMeshObject
	{
	public:
		SkinnedMeshObject(std::shared_ptr<ISkinnedMesh> SkinnedMesh,
			std::vector<std::shared_ptr<IMaterial>> materials,
			const MeshObjectInfo& info,
			const DirectX::SimpleMath::Matrix& transforms);
		~SkinnedMeshObject() = default;

		// Transform
		virtual void SetTransform(const DirectX::SimpleMath::Matrix& transform) override { mTransform = transform; }
		virtual const DirectX::SimpleMath::Matrix& GetTransform() const override { return mTransform; }

		// Info
		virtual void SetMeshObjectInfo(const MeshObjectInfo& info) override { mInfo = info; }
		virtual const MeshObjectInfo& GetMeshObjectInfo() const override { return mInfo; }

		// Material
		virtual void SetMaterials(const std::vector<std::shared_ptr<IMaterial>>& materials) override { mMaterials = materials; }
		virtual const std::vector<std::shared_ptr<IMaterial>>& GetMaterials() const override { return mMaterials; }

		// Mesh
		virtual void SetSkinnedMesh(std::shared_ptr<ISkinnedMesh> skinnedMesh) override { mSkinnedMesh = skinnedMesh; }
		virtual std::shared_ptr<ISkinnedMesh> GetSkinnedMesh() const override { return mSkinnedMesh; }

		// NodeHierarchy
		virtual void SetNodeHierarchyInstance(std::shared_ptr<INodeHierarchyInstance> nodeHierarchyInstance) override { mNodeHierarchyInstance = nodeHierarchyInstance; }
		virtual std::shared_ptr<INodeHierarchyInstance> GetNodeHierarchyInstance() const override { return mNodeHierarchyInstance; }

		virtual void SetMaterialInstanceInfo(const MaterialInstanceInfo& materialInstanceInfo) override { mMaterialInstanceInfo = materialInstanceInfo; }
		virtual const MaterialInstanceInfo& GetMaterialInstanceInfo() const override { return mMaterialInstanceInfo; }

		// Render
		virtual void SetIsRender(bool bIsRender) override { mbIsRender = bIsRender; }
		virtual bool GetIsRender() const override { return mbIsRender; }

	private:
		std::shared_ptr<INodeHierarchyInstance> mNodeHierarchyInstance;
		std::shared_ptr<ISkinnedMesh> mSkinnedMesh;
		std::vector<std::shared_ptr<IMaterial>> mMaterials;
		MeshObjectInfo mInfo;
		DirectX::SimpleMath::Matrix mTransform;
		MaterialInstanceInfo mMaterialInstanceInfo;
		bool mbIsRender;
	};

	class TerrainMeshObject : public ITerrainMeshObject
	{
	public:
		TerrainMeshObject(
			const std::shared_ptr<D3D11Device>& device,
			std::shared_ptr<StaticMesh> staticMesh,
			DirectX::SimpleMath::Matrix transform);
		~TerrainMeshObject() = default;

		inline virtual void SetTransform(const DirectX::SimpleMath::Matrix& transform) override;
		void SetTerrainMaterial(const std::shared_ptr<D3D11Device>& device,
			const TerrainMaterialInfo& terrainMaterial);

		inline virtual DirectX::SimpleMath::Matrix GetTransform() const override;
		inline virtual DirectX::BoundingBox GetRenderBoundingBox() const override;
		inline virtual DirectX::BoundingSphere GetRenderBoundingSphere() const override;

		virtual const fq::common::Mesh& GetMeshData() const override;
		virtual int* GetHeightData() const override;

		inline const std::shared_ptr<TerrainMesh>& GetTerrainMesh() const;
		inline const std::shared_ptr<StaticMesh>& GetStaticMesh() const;
		inline const std::shared_ptr<TerrainMaterial>& GetTerrainMaterial() const;

		size_t GetNumIndices() const { return mNumIndices; }

		// Lightmap
		inline void SetLightmapUVScaleOffset(const DirectX::SimpleMath::Vector4& scaleOffset) override;
		inline const DirectX::SimpleMath::Vector4& GetLightmapUVScaleOffset() const override;

		inline void SetLightmapIndex(unsigned int lightmapIndex) override;
		inline unsigned int GetLightmapIndex() const override;

		inline void SetIsStatic(bool bIsStatic) override;
		inline bool GetIsStatic() const override;

	private:
		// Terrain Vertex, Index 수정
		void BuildTerrainMesh(const std::shared_ptr<D3D11Device>& device, std::shared_ptr<StaticMesh> staticMesh);

		// 네비 메쉬, 픽킹을 위한 것
		void BuildStaticMesh(const std::shared_ptr<D3D11Device>& device, const fq::common::Mesh& terrainMesh);

		float GetWidth() const;
		float GetDepth() const;
		void BuildQuadPatchVB(std::vector<DirectX::SimpleMath::Vector2>& patchBoundsY, fq::common::Mesh& mesh);
		void BuildQuadPatchIB(fq::common::Mesh& mesh);

		void CalcAllPatchBoundsY(std::vector<DirectX::SimpleMath::Vector2>& patchBoundsY);
		void CalcAllPatchBoundsY(std::vector<DirectX::SimpleMath::Vector2>& patchBoundsY, UINT i, UINT j);
		void CalcNormalTangent(fq::common::Mesh& mesh);

	private:
		std::shared_ptr<StaticMesh> mTempStaticMesh;
		std::shared_ptr<TerrainMesh> mTerrainMesh;
		std::shared_ptr<TerrainMaterial> mMaterial;

		size_t mNumIndices;
		unsigned int mWidth;
		unsigned int mHeight;

		unsigned int mTextureWidth;
		unsigned int mTextureHeight;

		UINT mNumPatchVertRows;
		UINT mNumPatchVertCols;

		UINT mNumPatchVertices;
		UINT mNumPatchQuadFaces;

		unsigned int mCellsPerPatch;

		// Geometry Shader 로 메시를 저장 해서 버퍼로 불러왔는가 ...?
		bool mIsBakeMesh;

		DirectX::SimpleMath::Matrix mTransform;

		DirectX::SimpleMath::Vector4 mLightmapUVScaleOffset;
		int mLightmapIndex;
		bool mbIsStatic;
	};

#pragma region inlineFunc
	inline void TerrainMeshObject::SetTransform(const DirectX::SimpleMath::Matrix& transform)
	{
		mTransform = transform;
	}
	inline const std::shared_ptr<TerrainMesh>& TerrainMeshObject::GetTerrainMesh() const
	{
		return mTerrainMesh;
	}
	inline const std::shared_ptr<StaticMesh>& TerrainMeshObject::GetStaticMesh() const
	{
		return mTempStaticMesh;
	}
	inline DirectX::BoundingBox TerrainMeshObject::GetRenderBoundingBox() const
	{
		return mTerrainMesh->GetMeshData().RenderBoundingBox;
	}
	inline DirectX::BoundingSphere TerrainMeshObject::GetRenderBoundingSphere() const
	{
		return mTerrainMesh->GetMeshData().RenderBoundingSphere;
	}
	inline const std::shared_ptr<TerrainMaterial>& TerrainMeshObject::GetTerrainMaterial() const
	{
		return mMaterial;
	}
	inline DirectX::SimpleMath::Matrix TerrainMeshObject::GetTransform() const
	{
		// 이동 성분만 뽑아서 반환
		DirectX::SimpleMath::Vector3 translateVec(mTransform._41, mTransform._42, mTransform._43);
		DirectX::SimpleMath::Matrix matrix = DirectX::SimpleMath::Matrix::CreateTranslation(translateVec);

		return matrix;
	}
	inline void TerrainMeshObject::SetLightmapUVScaleOffset(const DirectX::SimpleMath::Vector4& scaleOffset)
	{
		mLightmapUVScaleOffset = scaleOffset;
	}
	inline const DirectX::SimpleMath::Vector4& TerrainMeshObject::GetLightmapUVScaleOffset() const
	{
		return mLightmapUVScaleOffset;
	}

	inline void TerrainMeshObject::SetLightmapIndex(unsigned int lightmapIndex)
	{
		mLightmapIndex = lightmapIndex;
	}
	inline unsigned int TerrainMeshObject::GetLightmapIndex() const
	{
		return mLightmapIndex;
	}

	inline void TerrainMeshObject::SetIsStatic(bool bIsStatic)
	{
		mbIsStatic = bIsStatic;
	}
	inline bool TerrainMeshObject::GetIsStatic() const
	{
		return mbIsStatic;
	}

#pragma endregion inlineFunc

	class ImageObject : public IImageObject
	{
	public:
		ImageObject();
		virtual ~ImageObject();

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

		virtual void SetAlpha(float alpha) override;
		virtual float GetAlpha() override;

		virtual void SetRotation(float angle) override;
		virtual float GetRotation() const override;

		virtual void SetScaleX(float scaleX) override;
		virtual void SetScaleY(float scaleY) override;
		virtual float GetScaleX() const override;
		virtual float GetScaleY() const override;

		virtual void SetImagePath(const std::string& texturePath) override;
		virtual std::string GetImagePath() override;

		virtual std::string GetMaskPath() const override { return mMaskPath; }
		virtual void SetMaskPath(const std::string& val) override { mMaskPath = val; }

		virtual void SetFillDegree(float Degree) override { mFillDegree = Degree; };
		virtual float GetFillDegree() const override { return mFillDegree; };

		virtual bool GetIsRender() const override { return mIsRender; }
		virtual void SetIsRender(bool val) override { mIsRender = val; }

		virtual bool GetRenderMode() const override { return mIsCenter; };
		virtual void SetRenderMode(bool isCenter) override { mIsCenter = isCenter; }; // true 면 중앙부터 그리기 , false 면 좌상단 부터 그리기

		virtual bool GetIsOnText() const override { return mIsOnText; };
		virtual void SetIsOnText(bool isOnText) override { mIsOnText = isOnText; };

		virtual DirectX::SimpleMath::Color GetColor() const override { return mColor; }
		virtual void SetColor(DirectX::SimpleMath::Color val) override { mColor = val; }

	protected:
		float mStartX;
		float mStartY;
		float mWidth;
		float mHeight;

		float mXRatio;
		float mYRatio;

		float mAlpha;

		unsigned int mLayer;

		float mRotationAngle;
		float mScaleX;
		float mScaleY;

		std::string mImagePath;
		std::string mMaskPath;

		float mFillDegree;

		bool mIsRender;
		bool mIsCenter;
		bool mIsOnText;

		DirectX::SimpleMath::Color mColor;
	};

	class ProbeObject : public IProbeObject
	{
	public:
		ProbeObject(std::shared_ptr<IStaticMesh> staticMesh,
			const DirectX::SimpleMath::Matrix& transform,
			int index);
		~ProbeObject();

		// Transform
		virtual void SetTransform(const DirectX::SimpleMath::Matrix& transform) override;
		virtual const DirectX::SimpleMath::Matrix& GetTransform() const override;

		// Mesh
		virtual void SetStaticMesh(std::shared_ptr<IStaticMesh> staticMesh) override;
		virtual std::shared_ptr<IStaticMesh> GetStaticMesh() const override;

		// LightProbe
		virtual void SetIndex(int index) override;
		virtual int GetIndex() override;

	private:
		std::shared_ptr<IStaticMesh> mStaticMesh;
		int mIndex;
		DirectX::SimpleMath::Matrix mTransform;
	};

	class TextObject : public ITextObject
	{
	public:
		virtual void SetTextInformation(TextInfo textInfo) override { mTextInfo = textInfo; };
		virtual TextInfo GetTextInformation() override { return mTextInfo; };

	private:
		TextInfo mTextInfo;
	};

	class SpriteAnimationObject : public ISpriteAnimationObject
	{
	public:
		virtual void SetSpriteInfo(SpriteInfo spriteInfo) override { mSpriteInfo = spriteInfo; };
		virtual SpriteInfo GetSpriteInfo() override { return mSpriteInfo; };

	private:
		SpriteInfo mSpriteInfo;
	};

	class VideoObject : public IVideoObject
	{
	public:
		virtual void SetVideoInfo(VideoInfo videoInfo) override { mVideoInfo = videoInfo; };
		virtual VideoInfo GetVideoInfo() override { return mVideoInfo; };

	private:
		VideoInfo mVideoInfo;
	};
}

