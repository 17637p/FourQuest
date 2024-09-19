#include "Terrain.h"
#include <spdlog/spdlog.h>

fq::game_module::Terrain::Terrain()
	:mTerrainLayers{},
	mAlPhaMapPath{},
	mHeightMapPath{ "./resource/internal/terrain.raw" },
	mHeightScale(65),
	mWidth(65),
	mHeight(65),
	mTextureWidth(513),
	mTextureHeight(513),
	mIsUseNavMesh(true),
	mLightmapScaleOffset{},
	mLightmapIndex(-1),
	mbIsStatic(false)
{

}

fq::game_module::Terrain::~Terrain()
{

}

void fq::game_module::Terrain::SetLightmapUVScaleOffset(const DirectX::SimpleMath::Vector4& scaleOffset)
{
	mLightmapScaleOffset = scaleOffset;
}

const DirectX::SimpleMath::Vector4& fq::game_module::Terrain::GetLightmapUVScaleOffset() const
{
	return mLightmapScaleOffset;
}

void fq::game_module::Terrain::SetLightmapIndex(unsigned int lightmapIndex)
{
	mLightmapIndex = lightmapIndex;
}

int fq::game_module::Terrain::GetLightmapIndex() const
{
	return mLightmapIndex;
}

void fq::game_module::Terrain::SetIsStatic(bool bIsStatic)
{
	mbIsStatic = bIsStatic;
}

bool fq::game_module::Terrain::GetIsStatic() const
{
	return mbIsStatic;
}

entt::meta_handle fq::game_module::Terrain::GetHandle()
{
	return *this;
}

std::shared_ptr<fq::game_module::Component> fq::game_module::Terrain::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	std::shared_ptr<Terrain> cloneTerrain = std::dynamic_pointer_cast<Terrain>(clone);

	if (cloneTerrain == nullptr) // 새로 생성해서 복사본을 준다
	{
		cloneTerrain = ObjectPool::GetInstance()->Assign<Terrain>(*this);
	}
	else // clone에 데이터를 복사한다.
	{
		// 기본 대입 연산자 호출한다.
		*cloneTerrain = *this;
	}
	cloneTerrain->mTerrainMeshObject = nullptr;

	return cloneTerrain;
}

void fq::game_module::Terrain::SetTerrainLayers(const std::vector<fq::graphics::TerrainLayer>& terrainLayers)
{
	mTerrainLayers = terrainLayers;
}

std::vector<fq::graphics::TerrainLayer> fq::game_module::Terrain::GetTerrainLayers() const
{
	return mTerrainLayers;
}

void fq::game_module::Terrain::SetAlphaMap(const std::string& alphaMapPath)
{
	mAlPhaMapPath = alphaMapPath;
}

std::string fq::game_module::Terrain::GetAlphaMap() const
{
	return mAlPhaMapPath;
}

fq::graphics::ITerrainMeshObject* fq::game_module::Terrain::GetTerrainMeshObject() const
{
	return mTerrainMeshObject;
}

void fq::game_module::Terrain::SetTerrainMeshObject(fq::graphics::ITerrainMeshObject* terrainMeshObject)
{
	mTerrainMeshObject = terrainMeshObject;
}

void fq::game_module::Terrain::SetHeightScale(float heightScale)
{
	if (heightScale <= 0)
	{
		spdlog::error("Terrain HeightScale Must be greater than 0");

		return;
	}
	mHeightScale = heightScale;
}

float fq::game_module::Terrain::GetHeightScale() const
{
	return mHeightScale;
}

void fq::game_module::Terrain::SetHeight(float height)
{
	if (height <= 0)
	{
		spdlog::error("Terrain Height Must be greater than 0");

		return;
	}
	mHeight = height;
}

float fq::game_module::Terrain::GetHeight() const
{
	return mHeight;
}

void fq::game_module::Terrain::SetWidth(float width)
{
	if (width <= 0)
	{
		spdlog::error("Terrain Width Must be greater than 0");

		return;
	}

	mWidth = width;
}

float fq::game_module::Terrain::GetWidth() const
{
	return mWidth;
}

void fq::game_module::Terrain::SetHeightMap(const std::string& heightMap)
{
	if (heightMap == "")
	{
		spdlog::error("Terrain HeightMap Must not be blank");

		return;
	}
	mHeightMapPath = heightMap;
}

std::string fq::game_module::Terrain::GetHeightMap() const
{
	return mHeightMapPath;
}

