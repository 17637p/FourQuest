#include "NavigationMeshData.h"

CreateNavigationMesh::NavigationMeshData::NavigationMeshData(CreateNavigationMesh* navFieldComponent)
	:navFieldComponent(navFieldComponent)
{
	navQuery = dtAllocNavMeshQuery();
	crowd = dtAllocCrowd();
	context = std::make_unique<rcContext>(rcContext());
}

CreateNavigationMesh::NavigationMeshData::~NavigationMeshData()
{
	dtFreeCrowd(crowd);
	dtFreeNavMeshQuery(navQuery);
}
