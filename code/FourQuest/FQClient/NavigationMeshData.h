#pragma once
#include <memory>

#include <recastnavigation/Recast.h>

#include <recastnavigation/DetourNavMesh.h>
#include <recastnavigation/DetourNavMeshBuilder.h>
#include <recastnavigation/DetourNavMeshQuery.h>
#include <recastnavigation/DetourCrowd.h>

#include "CreateNavigationMesh.h"

// Impl�� ���� �����͸� �׾Ƶδ� ������ ����.
class CreateNavigationMesh::NavigationMeshData
{
private:
	NavigationMeshData(CreateNavigationMesh* navFieldComponent);
	virtual ~NavigationMeshData();

public:
	CreateNavigationMesh* navFieldComponent;

	std::unique_ptr<rcContext> context;
	rcPolyMesh* polyMesh;
	rcConfig config;
	rcPolyMeshDetail* polyMeshDetail;
	class dtNavMesh* navMesh;
	class dtNavMeshQuery* navQuery;
	class dtCrowd* crowd;
};