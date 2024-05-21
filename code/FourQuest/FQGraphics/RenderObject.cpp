#include "RenderObject.h"
#include "BoneHierarchy.h"

#include "Material.h"

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
	{
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
	{
	}

	TerrainMeshObject::TerrainMeshObject(std::shared_ptr<StaticMesh> staticMesh, 
		DirectX::SimpleMath::Matrix transform)
		:mStaticMesh(staticMesh),
		mMaterial{nullptr},
		mTransform(transform)
	{
	}

	void TerrainMeshObject::SetTerrainMaterial(const std::shared_ptr<D3D11Device>& device, const TerrainMaterialInfo& terrainMaterial)
	{
		mMaterial = make_shared<TerrainMaterial>(device, terrainMaterial);
	}

	ImageObject::ImageObject()
		:mStartX(0),
		mStartY(0),
		mWidth(0),
		mHeight(0),
		mTexturePath(L""),
		mTransform{}
	{
	}

	ImageObject::~ImageObject()
	{

	}

	void ImageObject::SetTransform(const DirectX::SimpleMath::Matrix& transform)
	{
		mTransform = transform;
	}

	const DirectX::SimpleMath::Matrix& ImageObject::GetTransform() const
	{
		return mTransform;
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

	void ImageObject::SetTexturePath(const std::wstring& texturePath)
	{
		mTexturePath = texturePath;
	}

	std::wstring ImageObject::GetTexturePath()
	{
		return mTexturePath;
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

}