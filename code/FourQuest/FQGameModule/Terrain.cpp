#include "Terrain.h"

fq::game_module::Terrain::Terrain()
	:mTerrainLayers{},
	mAlPhaMapPath{}
{

}

fq::game_module::Terrain::~Terrain()
{

}

entt::meta_handle fq::game_module::Terrain::GetHandle()
{
	return *this;
}

std::shared_ptr<fq::game_module::Component> fq::game_module::Terrain::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	std::shared_ptr<Terrain> cloneTerrain = std::dynamic_pointer_cast<Terrain>(clone);

	if (cloneTerrain == nullptr) // ���� �����ؼ� ���纻�� �ش�
	{
		cloneTerrain = ObjectPool::GetInstance()->Assign<Terrain>(*this);
	}
	else // clone�� �����͸� �����Ѵ�.
	{
		// �⺻ ���� ������ ȣ���Ѵ�.
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

