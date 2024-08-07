#include "NavigationMesh.h"

#include "../FQGameModule/NavigationAgent.h"
#include "../FQGameModule/StaticMeshRenderer.h"
#include "../FQGameModule/Terrain.h"
#include "../FQGameModule/Transform.h"

#include <fstream>
#include <filesystem>

#include <recastnavigation/RecastAlloc.h>

fq::game_engine::NavigationMeshBuilder::NavigationMeshBuilder(GameProcess* tempProcess)
	:mTempProcess(tempProcess),
	mNavigationMeshData(nullptr),
	mHasNavigationMesh(false)
{
}

fq::game_engine::NavigationMeshBuilder::~NavigationMeshBuilder()
{
	delete mNavigationMeshData;
}

void fq::game_engine::NavigationMeshBuilder::BuildNavigationMesh(fq::game_module::Scene* scene, BuildSettings buildSettrings)
{
	if (mNavigationMeshData != nullptr)
	{
		delete mNavigationMeshData;
	}

	mNavigationMeshData = new NavigationMeshData(this);

	std::vector<DirectX::SimpleMath::Vector3> fieldVertices;
	std::vector<int> fieldIndices;

	// Mesh 하나로 통합하기 (하나의 Vertex Buffer, Index Buffer)
	auto componentView = scene->GetComponentView<fq::game_module::StaticMeshRenderer>();
	for (auto& navObject : componentView)
	{
		// NavMesh 추가는 해뒀지만 디버그 등의 이유로 사용하지 않기로 했다면
		bool isUsed = navObject.GetComponent<fq::game_module::StaticMeshRenderer>()->GetIsNavigationMeshUsed();
		if (!isUsed)
		{
			continue;
		}

		auto staticMesh = navObject.GetComponent<fq::game_module::StaticMeshRenderer>();
		if (staticMesh != nullptr)
		{
			if (staticMesh->GetStaticMeshObject() == nullptr)
			{
				continue;
			}
			const auto& staticMeshInterface = staticMesh->GetStaticMeshObject()->GetStaticMesh();
			auto vertices = staticMeshInterface->GetMeshData().Vertices;
			auto indices = staticMeshInterface->GetMeshData().Indices;

			UINT preVerticesSize = fieldVertices.size();
			fieldVertices.reserve(preVerticesSize + vertices.size());
			fieldIndices.reserve(fieldIndices.size() + indices.size());

			DirectX::SimpleMath::Matrix worldMatrix = navObject.GetComponent<fq::game_module::Transform>()->GetWorldMatrix();

			for (UINT i = 0; i < vertices.size(); i++)
			{
				DirectX::SimpleMath::Vector3 pos = vertices[i].Pos;
				pos = DirectX::SimpleMath::Vector3::Transform(pos, worldMatrix);
				fieldVertices.push_back(pos);
			}
			for (UINT i = 0; i < indices.size(); i++)
			{
				fieldIndices.push_back(indices[i] + preVerticesSize);
			}
		}
	}
	// Terrain 처리 추가
	auto terrainComponentView = scene->GetComponentView<fq::game_module::Terrain>();
	for (auto& navObject : terrainComponentView)
	{
		// NavMesh 추가는 해뒀지만 디버그 등의 이유로 사용하지 않기로 했다면
		bool isUsed = navObject.GetComponent<fq::game_module::Terrain>()->GetIsUseNavMesh();
		if (!isUsed)
		{
			continue;
		}

		auto terrainMesh = navObject.GetComponent<fq::game_module::Terrain>();
		if (terrainMesh != nullptr)
		{
			auto vertices = terrainMesh->GetTerrainMeshObject()->GetMeshData().Vertices;
			auto indices = terrainMesh->GetTerrainMeshObject()->GetMeshData().Indices;

			UINT preVerticesSize = fieldVertices.size();
			fieldVertices.reserve(preVerticesSize + vertices.size());
			fieldIndices.reserve(fieldIndices.size() + indices.size());

			DirectX::SimpleMath::Matrix worldMatrix = navObject.GetComponent<fq::game_module::Transform>()->GetWorldMatrix();

			for (UINT i = 0; i < vertices.size(); i++)
			{
				DirectX::SimpleMath::Vector3 pos = vertices[i].Pos;
				pos = DirectX::SimpleMath::Vector3::Transform(pos, worldMatrix);
				fieldVertices.push_back(pos);
			}
			for (UINT i = 0; i < indices.size(); i++)
			{
				fieldIndices.push_back(indices[i] + preVerticesSize);
			}
		}
	}

	if (fieldVertices.size() == 0)
	{
		spdlog::error("There are no navObjects registered.");
		return;
	}

	build(fieldVertices, fieldIndices, buildSettrings);
}

void fq::game_engine::NavigationMeshBuilder::build(
	std::vector<DirectX::SimpleMath::Vector3> worldVertices,
	std::vector<int> faces,
	const BuildSettings& buildSettings /* = Default */)
{
	static_assert(sizeof(DirectX::SimpleMath::Vector3) == sizeof(float) * 3);
	assert(!worldVertices.empty() && !faces.empty());
	assert(faces.size() % 3 == 0);
	preBuildNavigationMesh(reinterpret_cast<float*>(&worldVertices[0]), worldVertices.size(), &faces[0], faces.size() / 3, buildSettings);
}

void fq::game_engine::NavigationMeshBuilder::preBuildNavigationMesh(
	const float* worldVertices, size_t verticesNum,
	const int* faces, size_t facesNum,
	const BuildSettings& buildSettings /* = Default */)
{
	float bmin[3]{ FLT_MAX, FLT_MAX, FLT_MAX };
	float bmax[3]{ -FLT_MAX, -FLT_MAX, -FLT_MAX };

	// 바운더리 정보부터 설정
	for (auto i = 0; i < verticesNum; i++)
	{
		if (bmin[0] > worldVertices[i * 3])
			bmin[0] = worldVertices[i * 3];
		if (bmin[1] > worldVertices[i * 3 + 1])
			bmin[1] = worldVertices[i * 3 + 1];
		if (bmin[2] > worldVertices[i * 3 + 2])
			bmin[2] = worldVertices[i * 3 + 2];

		if (bmax[0] < worldVertices[i * 3])
			bmax[0] = worldVertices[i * 3];
		if (bmax[1] < worldVertices[i * 3 + 1])
			bmax[1] = worldVertices[i * 3 + 1];
		if (bmax[2] < worldVertices[i * 3 + 2])
			bmax[2] = worldVertices[i * 3 + 2];
	}
	auto& config{ mNavigationMeshData->config };
	memset(&config, 0, sizeof(rcConfig));

	config.cs = buildSettings.divisionSizeXZ;
	config.ch = buildSettings.divisionSizeY;
	config.walkableSlopeAngle = buildSettings.walkableSlopeAngle;
	config.walkableHeight = (int)ceilf(buildSettings.walkableHeight / config.ch);
	config.walkableClimb = (int)floorf(buildSettings.walkableClimb / config.ch);
	config.walkableRadius = (int)ceilf(config.cs * 2 / config.cs);
	config.maxEdgeLen = (int)(config.cs * 40 / config.cs);
	config.maxSimplificationError = 1.3f;
	config.minRegionArea = (int)rcSqr(config.cs * 27);		// Note: area = size*size
	config.mergeRegionArea = (int)rcSqr(config.cs * 67);	// Note: area = size*size
	config.maxVertsPerPoly = (int)6;
	config.detailSampleDist = 6.0f < 0.9f ? 0 : config.cs * 6.0f;
	config.detailSampleMaxError = config.ch * 1;

	rcVcopy(config.bmin, bmin);
	rcVcopy(config.bmax, bmax);
	rcCalcGridSize(config.bmin, config.bmax, config.cs, &config.width, &config.height);

	// 작업 맥락을 저장할 context 객체 생성, 작업의 성패여부를 저장할 processResult 선언
	auto* context = mNavigationMeshData->context.get();
	bool processResult{ false };
	// 복셀 높이필드 공간 할당
	rcHeightfield* heightField{ rcAllocHeightfield() };
	assert(heightField != nullptr);

	processResult = rcCreateHeightfield(context, *heightField, config.width, config.height, config.bmin, config.bmax, config.cs, config.ch);
	assert(processResult == true);

	std::vector<unsigned char> triareas;
	triareas.resize(facesNum);
	//unsigned char * triareas = new unsigned char[facesNum];
	//memset(triareas, 0, facesNum*sizeof(unsigned char));

	rcMarkWalkableTriangles(context, config.walkableSlopeAngle, worldVertices, verticesNum, faces, facesNum, triareas.data());
	processResult = rcRasterizeTriangles(context, worldVertices, verticesNum, faces, triareas.data(), facesNum, *heightField, config.walkableClimb);
	assert(processResult == true);

	// 필요없는 부분 필터링
	rcFilterLowHangingWalkableObstacles(context, config.walkableClimb, *heightField);
	rcFilterLedgeSpans(context, config.walkableHeight, config.walkableClimb, *heightField);
	rcFilterWalkableLowHeightSpans(context, config.walkableHeight, *heightField);

	// 밀집 높이 필드 만들기
	rcCompactHeightfield* compactHeightField{ rcAllocCompactHeightfield() };
	assert(compactHeightField != nullptr);

	processResult = rcBuildCompactHeightfield(context, config.walkableHeight, config.walkableClimb, *heightField, *compactHeightField);
	rcFreeHeightField(heightField);
	assert(processResult == true);

	//processResult = rcErodeWalkableArea(context, config.walkableRadius, *compactHeightField);
	//assert(processResult == true);

	processResult = rcBuildDistanceField(context, *compactHeightField);
	assert(processResult == true);

	rcBuildRegions(context, *compactHeightField, 0, config.minRegionArea, config.mergeRegionArea);
	assert(processResult == true);

	// 윤곽선 만들기
	rcContourSet* contourSet{ rcAllocContourSet() };
	assert(contourSet != nullptr);

	processResult = rcBuildContours(context, *compactHeightField, config.maxSimplificationError, config.maxEdgeLen, *contourSet);
	assert(processResult == true);

	// 윤곽선으로부터 폴리곤 생성
	rcPolyMesh*& polyMesh{ mNavigationMeshData->polyMesh = rcAllocPolyMesh() };
	assert(polyMesh != nullptr);

	processResult = rcBuildPolyMesh(context, *contourSet, config.maxVertsPerPoly, *polyMesh);
	assert(processResult == true);

	// 디테일 메시 생성
	auto& detailMesh{ mNavigationMeshData->polyMeshDetail = rcAllocPolyMeshDetail() };
	assert(detailMesh != nullptr);

	processResult = rcBuildPolyMeshDetail(context, *polyMesh, *compactHeightField, config.detailSampleDist, config.detailSampleMaxError, *detailMesh);
	assert(processResult == true);

	rcFreeCompactHeightfield(compactHeightField);
	rcFreeContourSet(contourSet);

	buildNavigationMesh(buildSettings);
}

void duDebugDrawNavMeshPoly(std::vector<DirectX::SimpleMath::Vector3>& navMeshVertices, const dtNavMesh& mesh, dtPolyRef ref)
{
	const dtMeshTile* tile = 0;
	const dtPoly* poly = 0;
	if (dtStatusFailed(mesh.getTileAndPolyByRef(ref, &tile, &poly)))
		return;

	const unsigned int ip = (unsigned int)(poly - tile->polys);

	const dtPolyDetail* pd = &tile->detailMeshes[ip];

	for (int i = 0; i < pd->triCount; ++i)
	{
		const unsigned char* t = &tile->detailTris[(pd->triBase + i) * 4];
		for (int j = 0; j < 3; ++j)
		{
			const float* pos;
			if (t[j] < poly->vertCount)
			{
				pos = &tile->verts[poly->verts[t[j]] * 3];
				//DrawVertex(&tile->verts[poly->verts[t[j]] * 3], tempProcess);
			}
			else
			{
				pos = &tile->detailVerts[(pd->vertBase + t[j] - poly->vertCount) * 3];
				//DrawVertex(&tile->detailVerts[(pd->vertBase + t[j] - poly->vertCount) * 3], tempProcess);
			}
			navMeshVertices.push_back({ pos[0], pos[1], pos[2] });
		}
	}
}

std::vector<DirectX::SimpleMath::Vector3> fq::game_engine::NavigationMeshBuilder::GetNavMeshVertices()
{
	std::vector<DirectX::SimpleMath::Vector3> navMeshVertices;

	if (!mHasNavigationMesh)
	{
		spdlog::error("No navgationMesh was created");
		return navMeshVertices;
	}

	const auto* mesh = mNavigationMeshData->navMesh;
	for (int i = 0; i < mesh->getMaxTiles(); ++i)
	{
		const dtMeshTile* tile = mesh->getTile(i);
		if (!tile->header) continue;
		dtPolyRef base = mesh->getPolyRefBase(tile);

		for (int j = 0; j < tile->header->polyCount; ++j)
		{
			const dtPoly* p = &tile->polys[j];
			//if ((p->flags & polyFlags) == 0) continue;
			duDebugDrawNavMeshPoly(navMeshVertices, *mesh, base | (dtPolyRef)j);
		}
	}

	return navMeshVertices;
}

void fq::game_engine::NavigationMeshBuilder::Update(float dt)
{
	if (mNavigationMeshData->crowd == nullptr)
		return;

	if (mNavigationMeshData->crowd->getAgentCount() != 0)
	{
		mNavigationMeshData->crowd->update(dt, nullptr);
	}
}

dtNavMeshQuery* fq::game_engine::NavigationMeshBuilder::GetNavQuery() const
{
	return mNavigationMeshData->navQuery;
}

int fq::game_engine::NavigationMeshBuilder::AddAgent(DirectX::SimpleMath::Vector3 pos, dtCrowdAgentParams* agentParams)
{
	const float posf[3] = { pos.x, pos.y, pos.z };
	return mNavigationMeshData->crowd->addAgent(posf, agentParams);
}

dtCrowd* fq::game_engine::NavigationMeshBuilder::GetCrowd()
{
	return mNavigationMeshData->crowd;
}

void fq::game_engine::NavigationMeshBuilder::RemoveAgent(UINT agentIndex)
{
	mNavigationMeshData->crowd->removeAgent(agentIndex);
}

void fq::game_engine::NavigationMeshBuilder::SaveNavMesh(std::string& fileName)
{
	if (!mHasNavigationMesh)
	{
		spdlog::error("Not Exist Navigation Mesh");
		return;
	}

	std::string line;

	std::filesystem::path dirPath = "./resource/NavMesh/";
	if (!std::filesystem::exists(dirPath))
	{
		std::filesystem::create_directory(dirPath);
	}

	std::ofstream navMeshFile(dirPath.string() + fileName + ".Nav");
	if (navMeshFile.is_open()) 
	{
		// Write Confing
		navMeshFile << mNavigationMeshData->config.width << "\n";
		navMeshFile << mNavigationMeshData->config.height << "\n";
		navMeshFile << mNavigationMeshData->config.tileSize << "\n";
		navMeshFile << mNavigationMeshData->config.borderSize << "\n";
		navMeshFile << mNavigationMeshData->config.cs << "\n";
		navMeshFile << mNavigationMeshData->config.ch << "\n";
		navMeshFile << mNavigationMeshData->config.bmin[0] << "\n";
		navMeshFile << mNavigationMeshData->config.bmin[1] << "\n";
		navMeshFile << mNavigationMeshData->config.bmin[2] << "\n";
		navMeshFile << mNavigationMeshData->config.bmax[0] << "\n";
		navMeshFile << mNavigationMeshData->config.bmax[1] << "\n";
		navMeshFile << mNavigationMeshData->config.bmax[2] << "\n";
		navMeshFile << mNavigationMeshData->config.walkableSlopeAngle << "\n";
		navMeshFile << mNavigationMeshData->config.walkableHeight << "\n";
		navMeshFile << mNavigationMeshData->config.walkableClimb << "\n";
		navMeshFile << mNavigationMeshData->config.walkableRadius << "\n";
		navMeshFile << mNavigationMeshData->config.maxEdgeLen << "\n";
		navMeshFile << mNavigationMeshData->config.maxSimplificationError << "\n";
		navMeshFile << mNavigationMeshData->config.minRegionArea << "\n";
		navMeshFile << mNavigationMeshData->config.mergeRegionArea << "\n";
		navMeshFile << mNavigationMeshData->config.maxVertsPerPoly << "\n";
		navMeshFile << mNavigationMeshData->config.detailSampleDist << "\n";
		navMeshFile << mNavigationMeshData->config.detailSampleMaxError << "\n";

		// Write PolyMesh
		navMeshFile << mNavigationMeshData->polyMesh->nverts << "\n";
		navMeshFile << mNavigationMeshData->polyMesh->npolys << "\n";
		navMeshFile << mNavigationMeshData->polyMesh->maxpolys << "\n";
		navMeshFile << mNavigationMeshData->polyMesh->nvp << "\n";
		navMeshFile << mNavigationMeshData->polyMesh->bmin[0] << "\n";
		navMeshFile << mNavigationMeshData->polyMesh->bmin[1] << "\n";
		navMeshFile << mNavigationMeshData->polyMesh->bmin[2] << "\n";
		navMeshFile << mNavigationMeshData->polyMesh->bmax[0] << "\n";
		navMeshFile << mNavigationMeshData->polyMesh->bmax[1] << "\n";
		navMeshFile << mNavigationMeshData->polyMesh->bmax[2] << "\n";
		navMeshFile << mNavigationMeshData->polyMesh->cs << "\n";
		navMeshFile << mNavigationMeshData->polyMesh->ch << "\n";
		navMeshFile << mNavigationMeshData->polyMesh->borderSize << "\n";
		navMeshFile << mNavigationMeshData->polyMesh->maxEdgeError << "\n";
		for (UINT i = 0; i < 3 * mNavigationMeshData->polyMesh->nverts; i++)
		{
			navMeshFile << mNavigationMeshData->polyMesh->verts[i] << "\n";
		}
		for (UINT i = 0; i < mNavigationMeshData->polyMesh->maxpolys * 2 * mNavigationMeshData->polyMesh->nvp; i++)
		{
			navMeshFile << mNavigationMeshData->polyMesh->polys[i] << "\n";
		}
		for (UINT i = 0; i < mNavigationMeshData->polyMesh->maxpolys; i++)
		{
			navMeshFile << mNavigationMeshData->polyMesh->regs[i] << "\n";
		}
		for (UINT i = 0; i < mNavigationMeshData->polyMesh->maxpolys; i++)
		{
			navMeshFile << mNavigationMeshData->polyMesh->flags[i] << "\n";
		}
		for (UINT i = 0; i < mNavigationMeshData->polyMesh->maxpolys; i++)
		{
			navMeshFile << (int)mNavigationMeshData->polyMesh->areas[i] << "\n";
		}

		//Write PolyMeshDetail
		navMeshFile << mNavigationMeshData->polyMeshDetail->nmeshes << "\n";
		navMeshFile << mNavigationMeshData->polyMeshDetail->nverts << "\n";
		navMeshFile << mNavigationMeshData->polyMeshDetail->ntris << "\n";
		for (UINT i = 0; i < 4 * mNavigationMeshData->polyMeshDetail->nmeshes; i++)
		{
			navMeshFile << mNavigationMeshData->polyMeshDetail->meshes[i] << "\n";
		}
		for (UINT i = 0; i < 3 * mNavigationMeshData->polyMeshDetail->nverts; i++)
		{
			navMeshFile << mNavigationMeshData->polyMeshDetail->verts[i] << "\n";
		}
		for (UINT i = 0; i < 4 * mNavigationMeshData->polyMeshDetail->ntris; i++)
		{
			navMeshFile << (int)mNavigationMeshData->polyMeshDetail->tris[i] << "\n";
		}
		navMeshFile.close();
	}
	else
	{
		spdlog::error("NavMesh File Can't write");
	}

	spdlog::info("Navigation Mesh Save Successful");
}

void fq::game_engine::NavigationMeshBuilder::LoadNavMesh(std::string& fileName, BuildSettings buildSettrings)
{
	if (mNavigationMeshData != nullptr)
	{
		delete mNavigationMeshData;
	}

	if (!(_access(("./resource/NavMesh/" + fileName + ".Nav").c_str(), 0) != -1))
	{
		spdlog::error("Not Exist Navigation Mesh");
	}

	mNavigationMeshData = new NavigationMeshData(this);
	mNavigationMeshData->polyMesh = rcAllocPolyMesh();
	mNavigationMeshData->polyMeshDetail = rcAllocPolyMeshDetail();

	std::ifstream navMeshFile("./resource/NavMesh/" + fileName + ".Nav");

	if (navMeshFile.is_open()) {
		// Read Confing
		navMeshFile >> mNavigationMeshData->config.width;
		navMeshFile >> mNavigationMeshData->config.height;
		navMeshFile >> mNavigationMeshData->config.tileSize;
		navMeshFile >> mNavigationMeshData->config.borderSize;
		navMeshFile >> mNavigationMeshData->config.cs;
		navMeshFile >> mNavigationMeshData->config.ch;
		navMeshFile >> mNavigationMeshData->config.bmin[0];
		navMeshFile >> mNavigationMeshData->config.bmin[1];
		navMeshFile >> mNavigationMeshData->config.bmin[2];
		navMeshFile >> mNavigationMeshData->config.bmax[0];
		navMeshFile >> mNavigationMeshData->config.bmax[1];
		navMeshFile >> mNavigationMeshData->config.bmax[2];
		navMeshFile >> mNavigationMeshData->config.walkableSlopeAngle;
		navMeshFile >> mNavigationMeshData->config.walkableHeight;
		navMeshFile >> mNavigationMeshData->config.walkableClimb;
		navMeshFile >> mNavigationMeshData->config.walkableRadius;
		navMeshFile >> mNavigationMeshData->config.maxEdgeLen; 
		navMeshFile >> mNavigationMeshData->config.maxSimplificationError;
		navMeshFile >> mNavigationMeshData->config.minRegionArea; 
		navMeshFile >> mNavigationMeshData->config.mergeRegionArea;
		navMeshFile >> mNavigationMeshData->config.maxVertsPerPoly; 
		navMeshFile >> mNavigationMeshData->config.detailSampleDist; 
		navMeshFile >> mNavigationMeshData->config.detailSampleMaxError;

		// Read PolyMesh
		navMeshFile >> mNavigationMeshData->polyMesh->nverts;
		navMeshFile >> mNavigationMeshData->polyMesh->npolys;
		navMeshFile >> mNavigationMeshData->polyMesh->maxpolys;
		navMeshFile >> mNavigationMeshData->polyMesh->nvp;
		navMeshFile >> mNavigationMeshData->polyMesh->bmin[0];
		navMeshFile >> mNavigationMeshData->polyMesh->bmin[1];
		navMeshFile >> mNavigationMeshData->polyMesh->bmin[2];
		navMeshFile >> mNavigationMeshData->polyMesh->bmax[0];
		navMeshFile >> mNavigationMeshData->polyMesh->bmax[1];
		navMeshFile >> mNavigationMeshData->polyMesh->bmax[2];
		navMeshFile >> mNavigationMeshData->polyMesh->cs;
		navMeshFile >> mNavigationMeshData->polyMesh->ch;
		navMeshFile >> mNavigationMeshData->polyMesh->borderSize; 
		navMeshFile >> mNavigationMeshData->polyMesh->maxEdgeError; 

		mNavigationMeshData->polyMesh->verts = 
			(unsigned short*)rcAlloc(sizeof(unsigned short) * mNavigationMeshData->polyMesh->nverts * 3, RC_ALLOC_PERM);
		mNavigationMeshData->polyMesh->polys = 
			(unsigned short*)rcAlloc(sizeof(unsigned short) * mNavigationMeshData->polyMesh->maxpolys * 2 * mNavigationMeshData->polyMesh->nvp, RC_ALLOC_PERM);
		mNavigationMeshData->polyMesh->regs = 
			(unsigned short*)rcAlloc(sizeof(unsigned short) * mNavigationMeshData->polyMesh->maxpolys, RC_ALLOC_PERM);
		mNavigationMeshData->polyMesh->flags = 
			(unsigned short*)rcAlloc(sizeof(unsigned short) * mNavigationMeshData->polyMesh->maxpolys, RC_ALLOC_PERM);
		mNavigationMeshData->polyMesh->areas = 
			(unsigned char*)rcAlloc(sizeof(unsigned char) * mNavigationMeshData->polyMesh->maxpolys, RC_ALLOC_PERM);
		
		for (UINT i = 0; i < 3 * mNavigationMeshData->polyMesh->nverts; i++)
		{
			navMeshFile >> mNavigationMeshData->polyMesh->verts[i];
		}
		for (UINT i = 0; i < mNavigationMeshData->polyMesh->maxpolys * 2 * mNavigationMeshData->polyMesh->nvp; i++)
		{
			navMeshFile >> mNavigationMeshData->polyMesh->polys[i];
		}
		for (UINT i = 0; i < mNavigationMeshData->polyMesh->maxpolys; i++)
		{
			navMeshFile >> mNavigationMeshData->polyMesh->regs[i];
		}
		for (UINT i = 0; i < mNavigationMeshData->polyMesh->maxpolys; i++)
		{
			navMeshFile >> mNavigationMeshData->polyMesh->flags[i];
		}
		for (UINT i = 0; i < mNavigationMeshData->polyMesh->maxpolys; i++)
		{
			int temp;
			navMeshFile >> temp;
			mNavigationMeshData->polyMesh->areas[i] = temp;
		}

		// Read PolyMeshDetail
		navMeshFile >> mNavigationMeshData->polyMeshDetail->nmeshes;
		navMeshFile >> mNavigationMeshData->polyMeshDetail->nverts;
		navMeshFile >> mNavigationMeshData->polyMeshDetail->ntris;

		mNavigationMeshData->polyMeshDetail->meshes =
			(unsigned int*)rcAlloc(sizeof(unsigned int) * mNavigationMeshData->polyMeshDetail->nmeshes * 4, RC_ALLOC_PERM);
		mNavigationMeshData->polyMeshDetail->verts =
			(float*)rcAlloc(sizeof(float) * mNavigationMeshData->polyMeshDetail->nverts * 3, RC_ALLOC_PERM);
		mNavigationMeshData->polyMeshDetail->tris =
			(unsigned char*)rcAlloc(sizeof(unsigned char) * mNavigationMeshData->polyMeshDetail->ntris * 4, RC_ALLOC_PERM);
		
		for (UINT i = 0; i < 4 * mNavigationMeshData->polyMeshDetail->nmeshes; i++)
		{
			navMeshFile >> mNavigationMeshData->polyMeshDetail->meshes[i];
		}
		for (UINT i = 0; i < 3 * mNavigationMeshData->polyMeshDetail->nverts; i++)
		{
			navMeshFile >> mNavigationMeshData->polyMeshDetail->verts[i];
		}
		for (UINT i = 0; i < 4 * mNavigationMeshData->polyMeshDetail->ntris; i++)
		{
			int temp;
			navMeshFile >> temp;
			mNavigationMeshData->polyMeshDetail->tris[i] = temp;
		}

		navMeshFile.close();

		buildNavigationMesh(buildSettrings);
		spdlog::info("Navigation Mesh Load Successful");
	}
	else
	{
		spdlog::error("NavMesh File Can't Load");
	}
}

void fq::game_engine::NavigationMeshBuilder::buildNavigationMesh(const BuildSettings& buildSettings)
{
	// detour 데이터 생성
	rcConfig config = mNavigationMeshData->config;
	rcPolyMesh* polyMesh = mNavigationMeshData->polyMesh;
	rcPolyMeshDetail* detailMesh = mNavigationMeshData->polyMeshDetail;
	unsigned char* navData{ nullptr };
	int navDataSize{ 0 };

	assert(config.maxVertsPerPoly <= DT_VERTS_PER_POLYGON);

	// Update poly flags from areas.
	for (int i = 0; i < polyMesh->npolys; ++i)
	{
		if (polyMesh->areas[i] == RC_WALKABLE_AREA)
		{
			polyMesh->areas[i] = 0;
			polyMesh->flags[i] = 1;
		}
	}
	dtNavMeshCreateParams params;
	memset(&params, 0, sizeof(params));
	params.verts = polyMesh->verts;
	params.vertCount = polyMesh->nverts;
	params.polys = polyMesh->polys;
	params.polyAreas = polyMesh->areas;
	params.polyFlags = polyMesh->flags;
	params.polyCount = polyMesh->npolys;
	params.nvp = polyMesh->nvp;
	params.detailMeshes = detailMesh->meshes;
	params.detailVerts = detailMesh->verts;
	params.detailVertsCount = detailMesh->nverts;
	params.detailTris = detailMesh->tris;
	params.detailTriCount = detailMesh->ntris;
	params.offMeshConVerts = 0;
	params.offMeshConRad = 0;
	params.offMeshConDir = 0;
	params.offMeshConAreas = 0;
	params.offMeshConFlags = 0;
	params.offMeshConUserID = 0;
	params.offMeshConCount = 0;
	params.walkableHeight = config.walkableHeight;
	params.walkableRadius = config.walkableRadius;
	params.walkableClimb = config.walkableClimb;
	rcVcopy(params.bmin, polyMesh->bmin);
	rcVcopy(params.bmax, polyMesh->bmax);
	params.cs = config.cs;
	params.ch = config.ch;
	params.buildBvTree = true;

	bool processResult{ false };

	processResult = dtCreateNavMeshData(&params, &navData, &navDataSize);
	assert(processResult == true);

	dtNavMesh* navMesh{ mNavigationMeshData->navMesh = dtAllocNavMesh() };
	assert(navMesh != nullptr);

	dtStatus status;
	status = navMesh->init(navData, navDataSize, DT_TILE_FREE_DATA);
	assert(dtStatusFailed(status) == false);

	dtNavMeshQuery* navQuery{ mNavigationMeshData->navQuery };
	status = navQuery->init(navMesh, 2048);
	assert(dtStatusFailed(status) == false);

	mNavigationMeshData->crowd->init(buildSettings.maxCrowdNumber, buildSettings.maxAgentRadius, navMesh);

	mHasNavigationMesh = true;

	spdlog::info("Navigation Mesh Build Success");
}

#pragma region NavigationData
fq::game_engine::NavigationMeshBuilder::NavigationMeshData::NavigationMeshData(NavigationMeshBuilder* navFieldComponent)
	:navFieldComponent(navFieldComponent)
{
	navQuery = dtAllocNavMeshQuery();
	crowd = dtAllocCrowd();
	context = std::make_unique<rcContext>(rcContext());
}

fq::game_engine::NavigationMeshBuilder::NavigationMeshData::~NavigationMeshData()
{
	rcFreePolyMesh(polyMesh);
	rcFreePolyMeshDetail(polyMeshDetail);

	dtFreeCrowd(crowd);
	dtFreeNavMeshQuery(navQuery);
	dtFreeNavMesh(navMesh);
}
#pragma endregion NavigationData