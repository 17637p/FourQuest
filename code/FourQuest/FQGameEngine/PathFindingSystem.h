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
			int maxCrowdNumber{ 256 }; // ��ã�� ��ü���� �ִ� ��ü��
			float maxAgentRadius{ 0.6 }; // ��ã�� ��ü���� �ִ� �浹�ݰ�
			float walkableSlopeAngle{ 30 }; // ������ �ִ� ���
			float walkableClimb{ 0.2 }; // ���� �� �ִ� ����
			float walkableHeight{ 0.3 }; // õ���� �ּ� ����
			float divisionSizeXZ{ 0.3 }; // x��,z�� ���� ������ ����, ������ �������� ������ �� ������������, �׺���̼� ����� ��ã�� �ý����� ���ϰ� �ð� �ȴ�.
			float divisionSizeY{ 0.2 }; // y�� ���� ������ ����, ������ �������� ������ �� ������������, �׺���̼� ����� ��ã�� �ý����� ���ϰ� �ð� �ȴ�.
			
			// ���� ������ xz������ 250*330, y������ 200�� ���� ���ҵǴ� ������ ���İ��� �׺���̼� ���嵵 �Ǹ鼭 ��ã�⵵ �������� �ϰ� �Ǵ� ������.
			// xz������ 743* 989�� ���� ������ �ǵ� ū ���ϴ� ����.
		};

		NavigationMeshBuilder(GameProcess* tempProcess);
		virtual ~NavigationMeshBuilder();

		void BuildNavigationMesh(fq::game_module::Scene* scene, BuildSettings buildSettrings);
		// Debug Draw ��
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

