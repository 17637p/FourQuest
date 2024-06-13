#pragma once

// Create
#include "../FQGameModule/GameModule.h"

// Navigation 
#include <memory>

#include <recastnavigation/Recast.h>

#include <recastnavigation/DetourNavMesh.h>
#include <recastnavigation/DetourNavMeshBuilder.h>
#include <recastnavigation/DetourNavMeshQuery.h>
#include <recastnavigation/DetourCrowd.h>

namespace fq::game_module{ class Scene; }

namespace fq::game_engine
{
	class GameProcess;

	class NavigationMeshBuilder
	{
		class NavigationMeshData;
	public:
		struct BuildSettings
		{
			int maxCrowdNumber{ 256 }; // 길찾기 주체들의 최대 개체수
			float maxAgentRadius{ 0.6 }; // 길찾기 주체들의 최대 충돌반경
			float walkableSlopeAngle{ 30 }; // 오를수 있는 경사
			float walkableClimb{ 0.2 }; // 오를 수 있는 단차
			float walkableHeight{ 0.3 }; // 천장의 최소 높이
			float divisionSizeXZ{ 0.3 }; // x축,z축 공간 분할의 단위, 단위가 작을수록 판정이 더 세밀해지지만, 네비게이션 빌드와 길찾기 시스템의 부하가 늘게 된다.
			float divisionSizeY{ 0.2 }; // y축 공간 분할의 단위, 단위가 작을수록 판정이 더 세밀해지지만, 네비게이션 빌드와 길찾기 시스템의 부하가 늘게 된다.
			
			// 공간 분할은 xz축으로 250*330, y축으로 200개 정도 분할되는 정도면 순식간에 네비게이션 빌드도 되면서 길찾기도 무리없이 하게 되는 정도다.
			// xz축으로 743* 989개 정도 분할이 되도 큰 부하는 없다.
		};

		NavigationMeshBuilder(GameProcess* tempProcess);
		virtual ~NavigationMeshBuilder();

		void BuildNavigationMesh(fq::game_module::Scene* scene, BuildSettings buildSettrings);
		// Debug Draw 용
		std::vector<DirectX::SimpleMath::Vector3> GetNavMeshVertices();

	private:
		void build(std::vector<DirectX::SimpleMath::Vector3> worldVertices, 
			std::vector<int> faces, 
			const BuildSettings& buildSettings = BuildSettings{});
		void buildNavigationMesh(
			const float* worldVertices, size_t verticesNum,
			const int* faces, size_t facesNum,
			const BuildSettings& buildSettings = BuildSettings{});

	private:
		GameProcess* mTempProcess;
		bool mHasNavigationMesh;
		NavigationMeshData* impl{ nullptr };
		//friend NavigationAgent;
	};

	class NavigationMeshBuilder::NavigationMeshData
	{
	public:
		NavigationMeshData(NavigationMeshBuilder* navFieldComponent);
		virtual ~NavigationMeshData();

	public:
		NavigationMeshBuilder* navFieldComponent;

		std::unique_ptr<rcContext> context;
		rcPolyMesh* polyMesh;
		rcConfig config;
		rcPolyMeshDetail* polyMeshDetail;
		class dtNavMesh* navMesh;
		class dtNavMeshQuery* navQuery;
		class dtCrowd* crowd;
	};

	class PathFindingSystem
	{
	public:
		PathFindingSystem(GameProcess* tempProcess);
		~PathFindingSystem();

		NavigationMeshBuilder::BuildSettings& GetBuildingSettrings();
		void BuildNavigationMesh(fq::game_module::Scene* scene);
		std::vector<DirectX::SimpleMath::Vector3> GetNavMeshVertices();

	private:
		NavigationMeshBuilder::BuildSettings mBuildSettings;
		NavigationMeshBuilder* mBuilder;
	};
}

