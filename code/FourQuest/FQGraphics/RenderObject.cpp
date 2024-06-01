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

				// �ؽ�ó�� ���� ��ü�� �������� �Ѵ�.
				patchVertices[i * mNumPatchVertCols + j].Tex.x = j * du;
				patchVertices[i * mNumPatchVertCols + j].Tex.y = i * dv;

				// Temp: normal & tangent  Y �� ������� ����ؼ� �ٽ� �־� ��� �Ѵ�...
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

		// �� �簢������ ���ε��� ����Ѵ�.
		int k = 0;
		for (UINT i = 0; i < mNumPatchVertRows - 1; i++)
		{
			for (UINT j = 0; j < mNumPatchVertCols - 1; j++)
			{
				// 2x2 �簢�� ��ġ�� ���� ���� �� ��
				indices[k] = i * mNumPatchVertCols + j;
				indices[k + 1] = i * mNumPatchVertCols + j + 1;

				// 2x2 �簢�� ��ġ�� �Ʒ��� ���� �� ��
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
		// �����̶� ����ϱ� �ȴ�! ������ ���� 4��
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

		// ����ȭ���� ���� ���� ���͸� ������ �ӽ� �迭�� ����ϴ�.
		DirectX::SimpleMath::Vector3* normals = new DirectX::SimpleMath::Vector3[(mNumPatchVertCols - 1) * (mNumPatchVertRows - 1)];
		if (!normals)
		{
			// �ƹ�ư �Ҵ� ���ߴٴ� ��� �߰� 
		}

		// �޽��� ������ ���캸�� ������ ����մϴ�.
		for (int j = 0; j < (mNumPatchVertCols - 1); j++)
		{
			for (int i = 0; i < (mNumPatchVertRows - 1); i++)
			{
				index1 = mesh.Indices[j * (mNumPatchVertCols - 1) + i];      // ���� �Ʒ� ������.
				index2 = mesh.Indices[j * (mNumPatchVertCols - 1) + i + 1];  // ������ �ϴ� ����.
				index3 = mesh.Indices[j * (mNumPatchVertCols - 1) + i + 2];          // �»���� ����.

				// ǥ�鿡�� �� ���� �������� �����ɴϴ�.
				vertex1[0] = mesh.Vertices[index1].Pos.x;
				vertex1[1] = mesh.Vertices[index1].Pos.y;
				vertex1[2] = mesh.Vertices[index1].Pos.z;

				vertex2[0] = mesh.Vertices[index2].Pos.x;
				vertex2[1] = mesh.Vertices[index2].Pos.y;
				vertex2[2] = mesh.Vertices[index2].Pos.z;

				vertex3[0] = mesh.Vertices[index3].Pos.x;
				vertex3[1] = mesh.Vertices[index3].Pos.y;
				vertex3[2] = mesh.Vertices[index3].Pos.z;

				// ǥ���� �� ���͸� ����մϴ�.
				vector1[0] = vertex1[0] - vertex3[0];
				vector1[1] = vertex1[1] - vertex3[1];
				vector1[2] = vertex1[2] - vertex3[2];
				vector2[0] = vertex3[0] - vertex2[0];
				vector2[1] = vertex3[1] - vertex2[1];
				vector2[2] = vertex3[2] - vertex2[2];

				index = mesh.Indices[j * (mNumPatchVertCols - 1) + i];

				// �� �� ������ ���� ����ȭ���� ���� ���� ��� ���� �� ������ ������ ����մϴ�.
				normals[index].x = (vector1[1] * vector2[2]) - (vector1[2] * vector2[1]);
				normals[index].y = (vector1[2] * vector2[0]) - (vector1[0] * vector2[2]);
				normals[index].z = (vector1[0] * vector2[1]) - (vector1[1] * vector2[0]);

				// ���̸� ����մϴ�.
				length = (float)sqrt(
					(normals[index].x * normals[index].x) +
					(normals[index].y * normals[index].y) +
					(normals[index].z * normals[index].z));

				// ���̸� ����Ͽ��̸��� ���� ���� ǥ��ȭ�մϴ�.
				normals[index].x = (normals[index].x / length);
				normals[index].y = (normals[index].y / length);
				normals[index].z = (normals[index].z / length);
			}
		}

		// ���� ��� ������ ���캸�� ������ ����� ���մϴ�.     
		// ������ ��� �� ������ ���� ��� ��հ��� ��´�.
		for (int j = 0; j < mNumPatchVertCols; j++)
		{
			for (int i = 0; i < mNumPatchVertRows; i++)
			{
				// �հ踦 �ʱ�ȭ�մϴ�.
				sum[0] = 0.0f;
				sum[1] = 0.0f;
				sum[2] = 0.0f;

				// ���� �Ʒ���.
				if (((i - 1) >= 0) && ((j - 1) >= 0))
				{
					index = mesh.Indices[j * (mNumPatchVertCols - 1) + i];

					sum[0] += normals[index].x;
					sum[1] += normals[index].y;
					sum[2] += normals[index].z;
				}

				// ������ �Ʒ� ��.
				if ((i < (mNumPatchVertRows - 1)) && ((j - 1) >= 0))
				{
					index = mesh.Indices[j * (mNumPatchVertCols - 1) + i + 1];

					sum[0] += normals[index].x;
					sum[1] += normals[index].y;
					sum[2] += normals[index].z;
				}

				// ���� �� ��.
				if (((i - 1) >= 0) && (j < (mNumPatchVertCols - 1)))
				{
					index = mesh.Indices[j * (mNumPatchVertCols - 1) + i + 2];

					sum[0] += normals[index].x;
					sum[1] += normals[index].y;
					sum[2] += normals[index].z;
				}

				// ������ �� ��.
				if ((i < (mNumPatchVertRows - 1)) && (j < (mNumPatchVertCols - 1)))
				{
					index = mesh.Indices[j * (mNumPatchVertCols - 1) + i + 3];

					sum[0] += normals[index].x;
					sum[1] += normals[index].y;
					sum[2] += normals[index].z;
				}

				// �� ������ ���̸� ����մϴ�.
				length = (float)sqrt((sum[0] * sum[0]) + (sum[1] * sum[1]) + (sum[2] * sum[2]));

				// ���� �� �迭�� ���� ��ġ�� ���� �ε����� �����ɴϴ�.
				index = mesh.Indices[(j * mNumPatchVertRows) + i];

				// �� ������ ���� ���� ������ ǥ��ȭ�Ͽ� ���� �� �迭�� �����մϴ�.
				mesh.Vertices[index].Normal.x = (sum[0] / length);
				mesh.Vertices[index].Normal.y = (sum[1] / length);
				mesh.Vertices[index].Normal.z = (sum[2] / length);
			}
		}

		// �ӽ� ������ �����մϴ�.
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