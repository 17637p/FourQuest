#include <d3d11.h>
#include <algorithm>
#include "RenderObject.h"
#include "NodeHierarchy.h"
#include "Material.h"
#include "D3D11JobManager.h"

namespace fq::graphics
{
	StaticMeshObject::StaticMeshObject(std::shared_ptr<IStaticMesh> staticMesh,
		std::vector<std::shared_ptr<IMaterial>> materials,
		const MeshObjectInfo& info,
		const DirectX::SimpleMath::Matrix& transform)
		: mNodeHierarchyInstance(nullptr)
		, mIndex((size_t)-1)
		, mStaticMesh(staticMesh)
		, mMaterials(materials)
		, mInfo(info)
		, mTransform(transform)
	{
	}

	void StaticMeshObject::Render(const DirectX::SimpleMath::Matrix& transform, const MeshObjectInfo& mashObjectInfo)
	{
		// create job;
	}
	void StaticMeshObject::RenderInstanced(const DirectX::SimpleMath::Matrix& transform, const MeshObjectInfo& mashObjectInfo)
	{
		mInstanceData.push_back({ transform, mashObjectInfo });
	}

	SkinnedMeshObject::SkinnedMeshObject(std::shared_ptr<ISkinnedMesh> skinnedMesh,
		std::vector<std::shared_ptr<IMaterial>> materials,
		const MeshObjectInfo& info,
		const DirectX::SimpleMath::Matrix& transform)
		: mNodeHierarchyInstance(nullptr)
		, mSkinnedMesh(skinnedMesh)
		, mMaterials(materials)
		, mInfo(info)
		, mTransform(transform)
	{
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
		mTextureWidth(0),
		mTextureHeight(0),
		mNumPatchVertRows(0),
		mNumPatchVertCols(0),
		mNumPatchVertices(0),
		mNumPatchQuadFaces(0),
		mIsBakeMesh(false),
		mCellsPerPatch(64)
	{
	}

	void TerrainMeshObject::SetTerrainMaterial(const std::shared_ptr<D3D11Device>& device, const TerrainMaterialInfo& terrainMaterial)
	{
		mWidth = terrainMaterial.TerrainWidth;
		mHeight = terrainMaterial.TerrainHeight;

		mTextureWidth = terrainMaterial.TextureWidth;
		mTextureHeight = terrainMaterial.TextureHeight;

		mCellsPerPatch = (mTextureWidth + mTextureHeight) / 4;

		mMaterial = make_shared<TerrainMaterial>(device, terrainMaterial, mTextureWidth, mTextureHeight);
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
				//patchVertices[i * mNumPatchVertCols + j].Normal = DirectX::XMFLOAT3(0.f, 0.f, -1.f);
				//patchVertices[i * mNumPatchVertCols + j].Tangent = DirectX::XMFLOAT3(0.f, 0.f, -1.f);
				patchVertices[i * mNumPatchVertCols + j].Normal = DirectX::XMFLOAT3(0.f, 0.f, 0.f);
				patchVertices[i * mNumPatchVertCols + j].Tangent = DirectX::XMFLOAT3(0.f, 0.f, 0.f);
			}
		}

		std::vector<fq::common::Mesh::BoundsYVertex> patchBoundsYVertices(mNumPatchVertRows * mNumPatchVertCols);
		for (UINT i = 0; i < mNumPatchVertRows - 1; i++)
		{
			for (UINT j = 0; j < mNumPatchVertCols - 1; j++)
			{
				UINT patchID = i * (mNumPatchVertCols - 1) + j;
				patchVertices[i * mNumPatchVertCols + j].Pos.y = patchBoundsY[patchID].y;
				patchBoundsYVertices[i * mNumPatchVertCols + j].BoundsY = patchBoundsY[patchID];
			}
		}
		mesh.Vertices = patchVertices;
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

		CalcNormalTangent(mesh);
		mesh.Subsets[0].IndexCount = mNumIndices;

		mTerrainMesh = std::make_shared<TerrainMesh>(device, mesh);

		BuildStaticMesh(device, mesh);
	}

	void TerrainMeshObject::CalcAllPatchBoundsY(std::vector<DirectX::SimpleMath::Vector2>& patchBoundsY)
	{
		mNumPatchVertRows = ((mTextureHeight - 1) / mCellsPerPatch) + 1;
		mNumPatchVertCols = ((mTextureWidth - 1) / mCellsPerPatch) + 1;

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
				UINT k = y * mTextureWidth + x;
				minY = min(minY, heightMap[k]);
				maxY = max(maxY, heightMap[k]);
			}
		}

		UINT patchID = i * (mNumPatchVertCols - 1) + j;
		patchBoundsY[patchID] = DirectX::XMFLOAT2(minY, maxY);
	}

	void TerrainMeshObject::CalcNormalTangent(fq::common::Mesh& mesh)
	{
		// https://copynull.tistory.com/324
		// https://github.com/jjuiddong/Introduction-to-3D-Game-Programming-With-DirectX11
		// https://ddidding.tistory.com/95
		for (UINT i = 0; i < mesh.Indices.size() / 4; i++)
		{
			// Normal 계산 
			UINT index0 = mesh.Indices[i * 4 + 0];
			UINT index1 = mesh.Indices[i * 4 + 2];
			UINT index2 = mesh.Indices[i * 4 + 3];

			DirectX::SimpleMath::Vector3 vertex0Pos = mesh.Vertices[index0].Pos;
			DirectX::SimpleMath::Vector3 vertex1Pos = mesh.Vertices[index1].Pos;
			DirectX::SimpleMath::Vector3 vertex2Pos = mesh.Vertices[index2].Pos;

			DirectX::SimpleMath::Vector3 bVec = vertex1Pos - vertex0Pos;
			DirectX::SimpleMath::Vector3 aVec = vertex2Pos - vertex0Pos;

			DirectX::SimpleMath::Vector3 normal;
			normal = aVec.Cross(bVec);

			mesh.Vertices[index0].Normal += normal;
			mesh.Vertices[index1].Normal += normal;
			mesh.Vertices[index2].Normal += normal;

			// Tangent 계산
			DirectX::SimpleMath::Vector2 vertex0uv = mesh.Vertices[index0].Tex;
			DirectX::SimpleMath::Vector2 vertex1uv = mesh.Vertices[index1].Tex;
			DirectX::SimpleMath::Vector2 vertex2uv = mesh.Vertices[index2].Tex;

			DirectX::SimpleMath::Vector2 deltaUV1 = vertex1uv - vertex0uv;
			DirectX::SimpleMath::Vector2 deltaUV0 = vertex2uv - vertex0uv;

			float d = 1.f / (deltaUV0.x * deltaUV1.y - deltaUV0.y * deltaUV1.x);

			mesh.Vertices[index0].Tangent += (aVec * deltaUV1.y - bVec * deltaUV0.y) * d;
			mesh.Vertices[index1].Tangent += (aVec * deltaUV1.y - bVec * deltaUV0.y) * d;
			mesh.Vertices[index2].Tangent += (aVec * deltaUV1.y - bVec * deltaUV0.y) * d;

			/// 두 번째 삼각형 
			// Normal 계산
			index0 = mesh.Indices[i * 4 + 0];
			index1 = mesh.Indices[i * 4 + 3];
			index2 = mesh.Indices[i * 4 + 1];

			bVec = mesh.Vertices[index1].Pos - mesh.Vertices[index0].Pos;
			aVec = mesh.Vertices[index2].Pos - mesh.Vertices[index0].Pos;

			normal = aVec.Cross(bVec);

			mesh.Vertices[index0].Normal += normal;
			mesh.Vertices[index1].Normal += normal;
			mesh.Vertices[index2].Normal += normal;

			// Tangent 계산
			vertex0uv = mesh.Vertices[index0].Tex;
			vertex1uv = mesh.Vertices[index1].Tex;
			vertex2uv = mesh.Vertices[index2].Tex;

			deltaUV1 = vertex1uv - vertex0uv;
			deltaUV0 = vertex2uv - vertex0uv;

			d = 1.f / (deltaUV0.x * deltaUV1.y - deltaUV0.y * deltaUV1.x);

			mesh.Vertices[index0].Tangent += (aVec * deltaUV1.y - bVec * deltaUV0.y) * d;
			mesh.Vertices[index1].Tangent += (aVec * deltaUV1.y - bVec * deltaUV0.y) * d;
			mesh.Vertices[index2].Tangent += (aVec * deltaUV1.y - bVec * deltaUV0.y) * d;
		}

		for (UINT i = 0; i < mesh.Vertices.size(); i++)
		{
			mesh.Vertices[i].Normal.Normalize();
			mesh.Vertices[i].Tangent.Normalize();
			int a = 3;
		}
	}

	void TerrainMeshObject::BuildStaticMesh(const std::shared_ptr<D3D11Device>& device, const fq::common::Mesh& terrainMesh)
	{
		fq::common::Mesh newStaticMesh = terrainMesh;

		for (UINT i = 0; i < terrainMesh.Vertices.size(); i++)
		{
			newStaticMesh.Vertices[i].Pos.y = newStaticMesh.BoundsYVertices[i].BoundsY.y;
		}

		std::vector<unsigned int> terrainTriIndexBuffer;

		for (UINT i = 0; i < terrainMesh.Indices.size(); i += 4)
		{
			terrainTriIndexBuffer.push_back(terrainMesh.Indices[i + 0]);
			terrainTriIndexBuffer.push_back(terrainMesh.Indices[i + 3]);
			terrainTriIndexBuffer.push_back(terrainMesh.Indices[i + 2]);

			terrainTriIndexBuffer.push_back(terrainMesh.Indices[i + 0]);
			terrainTriIndexBuffer.push_back(terrainMesh.Indices[i + 1]);
			terrainTriIndexBuffer.push_back(terrainMesh.Indices[i + 3]);
		}

		newStaticMesh.Indices = terrainTriIndexBuffer;
		//newStaticMesh.Vertices;
		//newStaticMesh.Indices;

		mTempStaticMesh = std::make_shared<StaticMesh>(device, newStaticMesh);
	}

	const fq::common::Mesh& TerrainMeshObject::GetMeshData() const
	{
		return mTempStaticMesh->GetMeshData();
	}

	int* TerrainMeshObject::GetHeightData() const
	{
		return mMaterial->GetHeightMapRawData();
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
		mScaleY(1),
		mAlpha(1.f)
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