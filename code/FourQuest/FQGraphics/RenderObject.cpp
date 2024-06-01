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
		mTransform(transform),
		mNumIndices(0),
		mWidth(0),
		mHeight(0),
		mNumPatchVertRows(0),
		mNumPatchVertCols(0),
		mNumPatchVertices(0),
		mNumPatchQuadFaces(0),
		mCellsPerPatch(64)
	{
	}

	void TerrainMeshObject::SetTerrainMaterial(const std::shared_ptr<D3D11Device>& device, const TerrainMaterialInfo& terrainMaterial)
	{
		mWidth = terrainMaterial.Width;
		mHeight = terrainMaterial.Height;
		mMaterial = make_shared<TerrainMaterial>(device, terrainMaterial);
		BuildTerrainMesh(device, mTempStaticMesh);
	}

	float TerrainMeshObject::GetWidth() const
	{
		return (mWidth - 1) * 0.5f;
	}

	float TerrainMeshObject::GetDepth() const
	{
		return (mHeight - 1) * 0.5f;
	}

	void TerrainMeshObject::BuildQuadPatchVB(std::vector<DirectX::SimpleMath::Vector2>& patchBoundsY, fq::common::Mesh& mesh)
	{
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

				// Temp: normal & tangent  Y 를 기반으로 계산해서 다시 넣어 줘야 한다...
				patchVertices[i * mNumPatchVertCols + j].Normal = DirectX::XMFLOAT3(0.f, 0.f, -1.f);
				patchVertices[i * mNumPatchVertCols + j].Tangent = DirectX::XMFLOAT3(1.f, 0.f, 0.f);
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

		CalcNormal(mesh);
		mesh.Subsets[0].IndexCount = mNumIndices;

		mTerrainMesh = std::make_shared<TerrainMesh>(device, mesh);
	}

	void TerrainMeshObject::CalcAllPatchBoundsY(std::vector<DirectX::SimpleMath::Vector2>& patchBoundsY)
	{
		mNumPatchVertRows = ((mHeight - 1) / mCellsPerPatch) + 1;
		mNumPatchVertCols = ((mWidth - 1) / mCellsPerPatch) + 1;

		mNumPatchVertices = mNumPatchVertRows * mNumPatchVertCols;
		mNumPatchQuadFaces = (mNumPatchVertRows - 1) * (mNumPatchVertCols - 1);

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
		UINT x0 = j * mCellsPerPatch;
		UINT x1 = (j + 1) * mCellsPerPatch;

		UINT y0 = i * mCellsPerPatch;
		UINT y1 = (i + 1) * mCellsPerPatch;

		float minY = FLT_MAX;
		float maxY = -FLT_MAX;

		auto heightMap = mMaterial->GetHeightMapData();
		for (UINT y = y0; y <= y1; ++y)
		{
			for (UINT x = x0; x <= x1; ++x)
			{
				UINT k = y * mWidth + x;
				minY = min(minY, heightMap[k]);
				maxY = max(maxY, heightMap[k]);
			}
		}

		UINT patchID = i * (mNumPatchVertCols - 1) + j;
		patchBoundsY[patchID] = DirectX::XMFLOAT2(minY, maxY);
	}

	void TerrainMeshObject::CalcNormal(fq::common::Mesh& mesh)
	{
		// https://copynull.tistory.com/324
		// https://github.com/jjuiddong/Introduction-to-3D-Game-Programming-With-DirectX11
		// https://ddidding.tistory.com/95
		// 새벽이라 계산하기 싫다! 지금은 오전 4시
		int index1 = 0;
		int index2 = 0;
		int index3 = 0;
		int index = 0;
		int count = 0;
		float vertex1[3] = { 0.f, 0.f, 0.f };
		float vertex2[3] = { 0.f, 0.f, 0.f };
		float vertex3[3] = { 0.f, 0.f, 0.f };
		float vector1[3] = { 0.f, 0.f, 0.f };
		float vector2[3] = { 0.f, 0.f, 0.f };
		float sum[3] = { 0.f, 0.f, 0.f };
		float length = 0.0f;

		// 정규화되지 않은 법선 벡터를 저장할 임시 배열을 만듭니다.
		DirectX::SimpleMath::Vector3* normals = new DirectX::SimpleMath::Vector3[(mNumPatchVertCols - 1) * (mNumPatchVertRows - 1)];
		if (!normals)
		{
			// 아무튼 할당 못했다는 경고 추가 
		}

		// 메쉬의 모든면을 살펴보고 법선을 계산합니다.
		for (int j = 0; j < (mNumPatchVertCols - 1); j++)
		{
			for (int i = 0; i < (mNumPatchVertRows - 1); i++)
			{
				index1 = mesh.Indices[j * (mNumPatchVertCols - 1) + i];      // 왼쪽 아래 꼭지점.
				index2 = mesh.Indices[j * (mNumPatchVertCols - 1) + i + 1];  // 오른쪽 하단 정점.
				index3 = mesh.Indices[j * (mNumPatchVertCols - 1) + i + 2];          // 좌상단의 정점.

				// 표면에서 세 개의 꼭지점을 가져옵니다.
				vertex1[0] = mesh.Vertices[index1].Pos.x;
				vertex1[1] = mesh.Vertices[index1].Pos.y;
				vertex1[2] = mesh.Vertices[index1].Pos.z;

				vertex2[0] = mesh.Vertices[index2].Pos.x;
				vertex2[1] = mesh.Vertices[index2].Pos.y;
				vertex2[2] = mesh.Vertices[index2].Pos.z;

				vertex3[0] = mesh.Vertices[index3].Pos.x;
				vertex3[1] = mesh.Vertices[index3].Pos.y;
				vertex3[2] = mesh.Vertices[index3].Pos.z;

				// 표면의 두 벡터를 계산합니다.
				vector1[0] = vertex1[0] - vertex3[0];
				vector1[1] = vertex1[1] - vertex3[1];
				vector1[2] = vertex1[2] - vertex3[2];
				vector2[0] = vertex3[0] - vertex2[0];
				vector2[1] = vertex3[1] - vertex2[1];
				vector2[2] = vertex3[2] - vertex2[2];

				index = mesh.Indices[j * (mNumPatchVertCols - 1) + i];

				// 이 두 법선에 대한 정규화되지 않은 값을 얻기 위해 두 벡터의 외적을 계산합니다.
				normals[index].x = (vector1[1] * vector2[2]) - (vector1[2] * vector2[1]);
				normals[index].y = (vector1[2] * vector2[0]) - (vector1[0] * vector2[2]);
				normals[index].z = (vector1[0] * vector2[1]) - (vector1[1] * vector2[0]);

				// 길이를 계산합니다.
				length = (float)sqrt(
					(normals[index].x * normals[index].x) +
					(normals[index].y * normals[index].y) +
					(normals[index].z * normals[index].z));

				// 길이를 사용하여이면의 최종 값을 표준화합니다.
				normals[index].x = (normals[index].x / length);
				normals[index].y = (normals[index].y / length);
				normals[index].z = (normals[index].z / length);
			}
		}

		// 이제 모든 정점을 살펴보고 각면의 평균을 취합니다.     
		// 정점이 닿아 그 정점에 대한 평균 평균값을 얻는다.
		for (int j = 0; j < mNumPatchVertCols; j++)
		{
			for (int i = 0; i < mNumPatchVertRows; i++)
			{
				// 합계를 초기화합니다.
				sum[0] = 0.0f;
				sum[1] = 0.0f;
				sum[2] = 0.0f;

				// 왼쪽 아래면.
				if (((i - 1) >= 0) && ((j - 1) >= 0))
				{
					index = mesh.Indices[j * (mNumPatchVertCols - 1) + i];

					sum[0] += normals[index].x;
					sum[1] += normals[index].y;
					sum[2] += normals[index].z;
				}

				// 오른쪽 아래 면.
				if ((i < (mNumPatchVertRows - 1)) && ((j - 1) >= 0))
				{
					index = mesh.Indices[j * (mNumPatchVertCols - 1) + i + 1];

					sum[0] += normals[index].x;
					sum[1] += normals[index].y;
					sum[2] += normals[index].z;
				}

				// 왼쪽 위 면.
				if (((i - 1) >= 0) && (j < (mNumPatchVertCols - 1)))
				{
					index = mesh.Indices[j * (mNumPatchVertCols - 1) + i + 2];

					sum[0] += normals[index].x;
					sum[1] += normals[index].y;
					sum[2] += normals[index].z;
				}

				// 오른쪽 위 면.
				if ((i < (mNumPatchVertRows - 1)) && (j < (mNumPatchVertCols - 1)))
				{
					index = mesh.Indices[j * (mNumPatchVertCols - 1) + i + 3];

					sum[0] += normals[index].x;
					sum[1] += normals[index].y;
					sum[2] += normals[index].z;
				}

				// 이 법선의 길이를 계산합니다.
				length = (float)sqrt((sum[0] * sum[0]) + (sum[1] * sum[1]) + (sum[2] * sum[2]));

				// 높이 맵 배열의 정점 위치에 대한 인덱스를 가져옵니다.
				index = mesh.Indices[(j * mNumPatchVertRows) + i];

				// 이 정점의 최종 공유 법선을 표준화하여 높이 맵 배열에 저장합니다.
				mesh.Vertices[index].Normal.x = (sum[0] / length);
				mesh.Vertices[index].Normal.y = (sum[1] / length);
				mesh.Vertices[index].Normal.z = (sum[2] / length);
			}
		}

		// 임시 법선을 해제합니다.
		delete[] normals;
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