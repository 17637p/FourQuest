#include "RenderObject.h"
#include "BoneHierarchy.h"

#include "Material.h"

#include <d3d11.h>

#include <algorithm>

namespace fq::graphics
{
	StaticMeshObject::StaticMeshObject(std::shared_ptr<StaticMesh> staticMesh,
		std::vector<std::shared_ptr<Material>> materials,
		DirectX::SimpleMath::Matrix transform)
		: mStaticMesh(staticMesh)
		, mMaterials(materials)
		, mTransform(transform)
		, mObjectRenderType(EObjectRenderType::Opaque)
		, mAlpha(1.f)
		, mbUseShadow(true)
		, mOutLineColor{ -1, -1, -1 }
	{
	}

	void StaticMeshObject::SetOutlineColor(const DirectX::SimpleMath::Color& color)
	{
		mOutLineColor = color;
	}

	DirectX::SimpleMath::Color StaticMeshObject::GetOutlineColor() const
	{
		return mOutLineColor;
	}

	SkinnedMeshObject::SkinnedMeshObject(std::shared_ptr<SkinnedMesh> skinnedMesh,
		std::vector<std::shared_ptr<Material>> materials,
		DirectX::SimpleMath::Matrix transform,
		BoneHierarchyCache boneHierarchyCache)
		: mSkinnedMesh(skinnedMesh)
		, mMaterials(materials)
		, mTransform(transform)
		, mBoneHierarchyCache(boneHierarchyCache)
		, mObjectRenderType(EObjectRenderType::Opaque)
		, mAlpha(1.f)
		, mbUseShadow(true)
		, mOutLineColor{ -1, -1, -1 }
	{
	}

	void SkinnedMeshObject::SetOutlineColor(const DirectX::SimpleMath::Color& color)
	{
		mOutLineColor = color;
	}

	DirectX::SimpleMath::Color SkinnedMeshObject::GetOutlineColor() const
	{
		return mOutLineColor;
	}

	TerrainMeshObject::TerrainMeshObject(
		const std::shared_ptr<D3D11Device>& device,
		std::shared_ptr<StaticMesh> staticMesh,
		DirectX::SimpleMath::Matrix transform)
		:mTerrainMesh(),
		mTempStaticMesh(staticMesh),
		mMaterial{ nullptr },
		mTransform(transform)
	{
	}

	void TerrainMeshObject::SetTerrainMaterial(const std::shared_ptr<D3D11Device>& device, const TerrainMaterialInfo& terrainMaterial)
	{
		mMaterial = make_shared<TerrainMaterial>(device, terrainMaterial);
		BuildTerrainMesh(device, mTempStaticMesh);
	}

	float TerrainMeshObject::GetWidth() const
	{
		// temp
		const UINT width = 513;

		return (width - 1) * 0.5f;
	}

	float TerrainMeshObject::GetDepth() const
	{
		// temp
		const UINT height = 513;

		return (height - 1) * 0.5f;
	}

	void TerrainMeshObject::BuildQuadPatchVB(std::vector<DirectX::SimpleMath::Vector2>& patchBoundsY, fq::common::Mesh& mesh)
	{
		// temp
		const UINT width = 513;
		const UINT height = 513;

		static const int CellsPerPatch = 64;

		UINT mNumPatchVertRows = ((height - 1) / CellsPerPatch) + 1;
		UINT mNumPatchVertCols = ((width - 1) / CellsPerPatch) + 1;

		UINT mNumPatchVertices = mNumPatchVertRows * mNumPatchVertCols;
		UINT mNumPatchQuadFaces = (mNumPatchVertRows - 1) * (mNumPatchVertCols - 1);

		std::vector<fq::common::Mesh::Vertex> patchVertices(mNumPatchVertRows * mNumPatchVertCols);
		float halfWidth = 0.5f * GetWidth();
		float halfDepth = 0.5f * GetDepth();

		float patchWidth = GetWidth() / (mNumPatchVertCols - 1);
		float patchDepth = GetDepth() / (mNumPatchVertRows - 1);
		float du = 1.0f / (mNumPatchVertCols - 1);
		float dv = 1.0f / (mNumPatchVertRows - 1);
		for (UINT i = 0; i < mNumPatchVertRows; i++)
		{
			float z = halfDepth - i * patchDepth;
			for (UINT j = 0; j < mNumPatchVertCols; j++)
			{
				float x = -halfWidth + j * patchWidth;

				patchVertices[i * mNumPatchVertCols + j].Pos = DirectX::XMFLOAT3(x, 0.0f, z);

				// 텍스처가 격자 전체에 입혀지게 한다.
				patchVertices[i * mNumPatchVertCols + j].Tex.x = j * du;
				patchVertices[i * mNumPatchVertCols + j].Tex.y = i * dv;
			}
		}
		mesh.Vertices = patchVertices;

		std::vector<fq::common::Mesh::BoundsYVertex> patchBoundsYVertices(mNumPatchVertRows * mNumPatchVertCols);
		for (UINT i = 0; i < mNumPatchVertRows - 1; i++)
		{
			for (UINT j = 0; j < mNumPatchVertCols - 1; j++)
			{
				UINT patchID = i * (mNumPatchVertCols - 1) + j;
				patchBoundsYVertices[i * mNumPatchVertCols + j].BoundsY = patchBoundsY[patchID];
			}
		}
		mesh.BoundsYVertices = patchBoundsYVertices;
	}

	void TerrainMeshObject::BuildQuadPatchIB(fq::common::Mesh& mesh)
	{
		// temp
		const UINT width = 513;
		const UINT height = 513;

		static const int CellsPerPatch = 64;

		UINT mNumPatchVertRows = ((height - 1) / CellsPerPatch) + 1;
		UINT mNumPatchVertCols = ((width - 1) / CellsPerPatch) + 1;

		UINT mNumPatchVertices = mNumPatchVertRows * mNumPatchVertCols;
		UINT mNumPatchQuadFaces = (mNumPatchVertRows - 1) * (mNumPatchVertCols - 1);

		mNumIndices = mNumPatchQuadFaces * 4;
		std::vector<unsigned int> indices(mNumIndices);

		// 각 사각형마다 색인들을 계산한다.
		int k = 0;
		for (UINT i = 0; i < mNumPatchVertRows - 1; i++)
		{
			for (UINT j = 0; j < mNumPatchVertCols - 1; j++)
			{
				// 2x2 사각형 패치의 위쪽 정점 두 개
				indices[k] = i * mNumPatchVertCols + j;
				indices[k + 1] = i * mNumPatchVertCols + j + 1;

				// 2x2 사각형 패치의 아래쪽 정점 두 개
				indices[k + 2] = (i + 1) * mNumPatchVertCols + j;
				indices[k + 3] = (i + 1) * mNumPatchVertCols + j + 1;
				k += 4;
			}
		}

		mesh.Indices = indices;
	}

	void TerrainMeshObject::BuildTerrainMesh(const std::shared_ptr<D3D11Device>& device, std::shared_ptr<StaticMesh> staticMesh)
	{
		std::vector<DirectX::SimpleMath::Vector2> mPatchBoundsY;

		CalcAllPatchBoundsY(mPatchBoundsY);

		fq::common::Mesh mesh = staticMesh->GetMeshData();
		BuildQuadPatchVB(mPatchBoundsY, mesh);
		BuildQuadPatchIB(mesh);
		mesh.Subsets[0].IndexCount = mNumIndices;

		mTerrainMesh = std::make_shared<TerrainMesh>(device, mesh);
	}

	void TerrainMeshObject::CalcAllPatchBoundsY(std::vector<DirectX::SimpleMath::Vector2>& patchBoundsY)
	{
		// temp
		const UINT width = 513;
		const UINT height = 513;

		static const int CellsPerPatch = 64;

		UINT mNumPatchVertRows = ((height - 1) / CellsPerPatch) + 1;
		UINT mNumPatchVertCols = ((width - 1) / CellsPerPatch) + 1;

		UINT mNumPatchVertices = mNumPatchVertRows * mNumPatchVertCols;
		UINT mNumPatchQuadFaces = (mNumPatchVertRows - 1) * (mNumPatchVertCols - 1);

		patchBoundsY.resize(mNumPatchQuadFaces);

		for (UINT i = 0; i < mNumPatchVertRows - 1; i++)
		{
			for (UINT j = 0; j < mNumPatchVertCols - 1; j++)
			{
				CalcAllPatchBoundsY(patchBoundsY, i, j);
			}
		}
	}

	void TerrainMeshObject::CalcAllPatchBoundsY(std::vector<DirectX::SimpleMath::Vector2>& patchBoundsY, UINT i, UINT j)
	{
		// temp
		const UINT width = 513;
		const UINT height = 513;

		static const int CellsPerPatch = 64;

		UINT mNumPatchVertRows = ((height - 1) / CellsPerPatch) + 1;
		UINT mNumPatchVertCols = ((width - 1) / CellsPerPatch) + 1;

		UINT mNumPatchVertices = mNumPatchVertRows * mNumPatchVertCols;
		UINT mNumPatchQuadFaces = (mNumPatchVertRows - 1) * (mNumPatchVertCols - 1);

		UINT x0 = j * CellsPerPatch;
		UINT x1 = (j + 1) * CellsPerPatch;

		UINT y0 = i * CellsPerPatch;
		UINT y1 = (i + 1) * CellsPerPatch;

		float minY = FLT_MAX;
		float maxY = -FLT_MAX;

		auto heightMap = mMaterial->GetHeightMapData();
		for (UINT y = y0; y <= y1; ++y)
		{
			for (UINT x = x0; x <= x1; ++x)
			{
				UINT k = y * width + x;
				minY = min(minY, heightMap[k]);
				maxY = max(maxY, heightMap[k]);
			}
		}

		UINT patchID = i * (mNumPatchVertCols - 1) + j;
		patchBoundsY[patchID] = DirectX::XMFLOAT2(minY, maxY);
	}

	ImageObject::ImageObject()
		:mStartX(0),
		mStartY(0),
		mWidth(0),
		mHeight(0),
		mXRatio(1),
		mYRatio(1),
		mLayer(0),
		mImagePath(""),
		mRotationAngle(0),
		mScaleX(1),
		mScaleY(1)
	{
	}

	ImageObject::~ImageObject()
	{

	}

	void ImageObject::SetStartX(float startX)
	{
		mStartX = startX;
	}

	void ImageObject::SetStartY(float startY)
	{
		mStartY = startY;
	}

	float ImageObject::GetStartX()
	{
		return mStartX;
	}

	float ImageObject::GetStartY()
	{
		return mStartY;
	}

	void ImageObject::SetWidth(float width)
	{
		mWidth = width;
	}

	void ImageObject::SetHeight(float height)
	{
		mHeight = height;
	}

	float ImageObject::GetWidth()
	{
		return mWidth;
	}

	float ImageObject::GetHeight()
	{
		return mHeight;
	}

	void ImageObject::SetImagePath(const std::string& texturePath)
	{
		mImagePath = texturePath;
	}

	std::string ImageObject::GetImagePath()
	{
		return mImagePath;
	}

	void ImageObject::SetLayer(unsigned int layer)
	{
		mLayer = layer;
	}

	unsigned int ImageObject::GetLayer()
	{
		return mLayer;
	}

	void ImageObject::SetXRatio(float xRatio)
	{
		mXRatio = xRatio;
	}

	float ImageObject::GetXRatio()
	{
		return mXRatio;
	}

	void ImageObject::SetYRatio(float yRatio)
	{
		mYRatio = yRatio;
	}

	float ImageObject::GetYRatio()
	{
		return mYRatio;
	}

	void ImageObject::SetAlpha(float alpha)
	{
		mAlpha = alpha;
	}

	float ImageObject::GetAlpha()
	{
		return mAlpha;
	}

	void ImageObject::SetScaleX(float scaleX)
	{
		mScaleX = scaleX;
	}

	void ImageObject::SetScaleY(float scaleY)
	{
		mScaleY = scaleY;
	}

	float ImageObject::GetScaleX() const
	{
		return mScaleX;
	}

	float ImageObject::GetScaleY() const
	{
		return mScaleY;
	}

	void ImageObject::SetRotation(float angle)
	{
		mRotationAngle = angle;
	}

	float ImageObject::GetRotation() const
	{
		return mRotationAngle;
	}

}