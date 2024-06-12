#include "PathFindingSystem.h"

#include "../FQGameModule/Scene.h"

// temp
#include "GameProcess.h"
#include "../FQGraphics/IFQGraphics.h"

void fq::game_engine::CreateNavigationMesh::OnStart(fq::game_module::Scene* scene)
{
	//if (mIsBuildMesh)
	{
		UINT obejctCount = scene->GetObjectSize();

		std::vector<DirectX::SimpleMath::Vector3> fieldVertices;
		std::vector<int> fieldIndices;

		// 바닥 가져오기
		// 장애물 가져오기
		for (UINT i = 0; i < obejctCount; i++)
		{
			fq::game_module::GameObject* object = scene->GetObjectByIndex(i);
			if (object->GetTag() == fq::game_module::ETag::Floor || object->GetTag() == fq::game_module::ETag::Obstacle)
			{
				// 다 합치기 (버텍스, 인덱스)
				auto staticMesh = object->GetComponent<fq::game_module::StaticMeshRenderer>();
				if (staticMesh != nullptr)
				{
					auto vertices = staticMesh->GetStaticMeshObject()->GetMeshData().Vertices;
					auto indices = staticMesh->GetStaticMeshObject()->GetMeshData().Indices;

					UINT preVerticesSize = fieldVertices.size();
					fieldVertices.reserve(preVerticesSize + vertices.size());
					fieldIndices.reserve(fieldIndices.size() + indices.size());

					DirectX::SimpleMath::Matrix worldMatrix = object->GetComponent<fq::game_module::Transform>()->GetWorldMatrix();

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
		}
		// 합친 거를 BuildField로 보내기 
		BuildField(fieldVertices, fieldIndices);
		// BuildField로 보내서 만든거 화면에 띄우기  (polygoninfo drawpolygon이 있음)
	}
}

void fq::game_engine::CreateNavigationMesh::BuildField(
	std::vector<DirectX::SimpleMath::Vector3> worldVertices,
	std::vector<int> faces,
	const BuildSettings& buildSettings /* = Default */)
{
	static_assert(sizeof(DirectX::SimpleMath::Vector3) == sizeof(float) * 3);
	assert(!worldVertices.empty() && !faces.empty());
	assert(faces.size() % 3 == 0);
	buildField(reinterpret_cast<float*>(&worldVertices[0]), worldVertices.size(), &faces[0], faces.size() / 3, buildSettings);
}

unsigned int duTransCol(unsigned int c, unsigned int a)
{
	return (a << 24) | (c & 0x00ffffff);
}

unsigned int duRGBA(int r, int g, int b, int a)
{
	return ((unsigned int)r) | ((unsigned int)g << 8) | ((unsigned int)b << 16) | ((unsigned int)a << 24);
}

void DrawPolygon(const float* pos, unsigned int color, fq::game_engine::GameProcess* tempProcess)
{
	// 생각해보니 버텍스가 아니라 폴리곤이여야 되는데... 
	//vertex(pos[0], pos[1], pos[2], color);
	fq::graphics::debug::SphereInfo info;

	info.Sphere.Center = {pos[0], pos[1], pos[2]};
	info.Sphere.Radius = 0.1f;

	info.Color = { 0, 1, 1, 1 };

	tempProcess->mGraphics->DrawSphere(info);
}

void duDebugDrawNavMeshPoly(const dtNavMesh& mesh, dtPolyRef ref, const unsigned int col, fq::game_engine::GameProcess* tempProcess)
{
	const dtMeshTile* tile = 0;
	const dtPoly* poly = 0;
	if (dtStatusFailed(mesh.getTileAndPolyByRef(ref, &tile, &poly)))
		return;

	const unsigned int c = duTransCol(col, 64);
	const unsigned int ip = (unsigned int)(poly - tile->polys);

	const dtPolyDetail* pd = &tile->detailMeshes[ip];

	for (int i = 0; i < pd->triCount; ++i)
	{
		const unsigned char* t = &tile->detailTris[(pd->triBase + i) * 4];
		for (int j = 0; j < 3; ++j)
		{
			if (t[j] < poly->vertCount)
			{
				DrawPolygon(&tile->verts[poly->verts[t[j]] * 3], c, tempProcess);
			}
			else
			{
				DrawPolygon(&tile->detailVerts[(pd->vertBase + t[j] - poly->vertCount) * 3], c, tempProcess);
			}
		}
	}
}

void fq::game_engine::CreateNavigationMesh::DebugDraw()
{
	const auto* mesh = impl->navMesh;
	for (int i = 0; i < mesh->getMaxTiles(); ++i)
	{
		const dtMeshTile* tile = mesh->getTile(i);
		if (!tile->header) continue;
		dtPolyRef base = mesh->getPolyRefBase(tile);

		for (int j = 0; j < tile->header->polyCount; ++j)
		{
			const dtPoly* p = &tile->polys[j];
			//if ((p->flags & polyFlags) == 0) continue;
			duDebugDrawNavMeshPoly(*mesh, base | (dtPolyRef)j, duRGBA(1, 0, 0, 1), mTempProcess);
		}
	}
}

void fq::game_engine::CreateNavigationMesh::buildField(
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
	auto& config{ impl->config };
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
	auto* context = impl->context.get();
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
	//rcFreeHeightField(heightField);
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
	rcPolyMesh*& polyMesh{ impl->polyMesh = rcAllocPolyMesh() };
	assert(polyMesh != nullptr);

	processResult = rcBuildPolyMesh(context, *contourSet, config.maxVertsPerPoly, *polyMesh);
	assert(processResult == true);

	// 디테일 메시 생성
	auto& detailMesh{ impl->polyMeshDetail = rcAllocPolyMeshDetail() };
	assert(detailMesh != nullptr);

	processResult = rcBuildPolyMeshDetail(context, *polyMesh, *compactHeightField, config.detailSampleDist, config.detailSampleMaxError, *detailMesh);
	assert(processResult == true);

	//rcFreeCompactHeightfield(compactHeightField);
	//rcFreeContourSet(contourSet);

	// detour 데이터 생성
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

	processResult = dtCreateNavMeshData(&params, &navData, &navDataSize);
	assert(processResult == true);

	dtNavMesh* navMesh{ impl->navMesh = dtAllocNavMesh() };
	assert(navMesh != nullptr);

	dtStatus status;
	status = navMesh->init(navData, navDataSize, DT_TILE_FREE_DATA);
	//dtFree(navData);
	assert(dtStatusFailed(status) == false);

	dtNavMeshQuery* navQuery{ impl->navQuery };
	status = navQuery->init(navMesh, 2048);
	assert(dtStatusFailed(status) == false);

	impl->crowd->init(1024, buildSettings.maxAgentRadius, navMesh);
}

fq::game_engine::CreateNavigationMesh::CreateNavigationMesh(GameProcess* tempProcess)
{
	mTempProcess = tempProcess;
	impl = new NavigationMeshData(this);
}

fq::game_engine::CreateNavigationMesh::~CreateNavigationMesh()
{
	delete impl;
}

fq::game_engine::CreateNavigationMesh::NavigationMeshData::NavigationMeshData(CreateNavigationMesh* navFieldComponent)
	:navFieldComponent(navFieldComponent)
{
	navQuery = dtAllocNavMeshQuery();
	crowd = dtAllocCrowd();
	context = std::make_unique<rcContext>(rcContext());
}

fq::game_engine::CreateNavigationMesh::NavigationMeshData::~NavigationMeshData()
{
	dtFreeCrowd(crowd);
	dtFreeNavMeshQuery(navQuery);
}
