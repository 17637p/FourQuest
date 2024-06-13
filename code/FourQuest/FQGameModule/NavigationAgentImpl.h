#pragma once
#include <recastnavigation/DetourCrowd.h>
#include "NavigationAgent.h"

namespace fq::game_module
{
    class NavigationAgent::Impl
    {
    public:
        int agentIdx{ -1 };
        //const dtCrowdAgent* agent{ nullptr };
        dtCrowd* crowd{ nullptr };
        dtPolyRef targetRef;
        float targetPos[3];
        dtCrowdAgentParams agentParams
        {
            .radius = 1,
            .height = 0.3,
            .maxAcceleration = std::numeric_limits<float>::max(),
            .maxSpeed = 5,
            .collisionQueryRange = 12,
            .pathOptimizationRange = 30,
            .separationWeight = 2,
            .updateFlags = DT_CROWD_ANTICIPATE_TURNS |
            DT_CROWD_OPTIMIZE_VIS |
            DT_CROWD_OBSTACLE_AVOIDANCE,
            .obstacleAvoidanceType = (unsigned char)3,
        };
        NavigationAgent* navAgentComponent;

	private:
		Impl(NavigationAgent* navAgentComponent);

		virtual ~Impl();
		friend NavigationAgent;
    };
}