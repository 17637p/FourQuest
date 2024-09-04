#include "NavigationAgent.h"

#include "NavigationAgentImpl.h"
#include "../FQGameEngine/PathFindingSystem.h"

#include "../FQGameModule/Transform.h"

fq::game_module::NavigationAgent::NavigationAgent()
	:mImpl(),
	mPathFindingSystem(nullptr)
	,mbSyncRotationWithMovementDirection(true)
	,mbIsStop(false)
{

}

fq::game_module::NavigationAgent::~NavigationAgent()
{
	//if (mImpl != nullptr)
	//{
	//	mImpl->crowd->removeAgent(mImpl->agentIdx);
	//}
}

void fq::game_module::NavigationAgent::OnUpdate(float dt)
{

}

void fq::game_module::NavigationAgent::RegisterNavigationField(fq::game_engine::PathFindingSystem* pathFindingSystem)
{
	mPathFindingSystem = pathFindingSystem;

	Transform* agentTransform = GetGameObject()->GetComponent<Transform>();
	mImpl->agentIdx = pathFindingSystem->AddAgentToCrowd(agentTransform->GetWorldPosition(), &mImpl->agentParams);

	mImpl->crowd = mPathFindingSystem->GetCrowd();
}

float fq::game_module::NavigationAgent::GetSpeed()const
{
	if (mImpl->crowd == nullptr)
	{
		return 0.f;
	}

	return mImpl->agentParams.maxSpeed;
}

void fq::game_module::NavigationAgent::SetSpeed(float speed)
{
	mImpl->agentParams.maxSpeed = speed;

	if (mImpl->crowd != nullptr)
		mImpl->crowd->updateAgentParameters(mImpl->agentIdx, &mImpl->agentParams);
}

float fq::game_module::NavigationAgent::GetAcceleration()
{
	return mImpl->agentParams.maxAcceleration;
}

void fq::game_module::NavigationAgent::SetAcceleration(float accel)
{
	mImpl->agentParams.maxAcceleration = accel;
	if (mImpl->crowd != nullptr)
		mImpl->crowd->updateAgentParameters(mImpl->agentIdx, &mImpl->agentParams);
}

float fq::game_module::NavigationAgent::GetRadius()
{
	if (mImpl->crowd == nullptr)
	{
		return 0.f;
	}

	return mImpl->agentParams.radius;
}

void fq::game_module::NavigationAgent::SetAgentState()
{
	if (mImpl->crowd != nullptr)
	{
		dtCrowdAgent* agent = mImpl->crowd->getEditableAgent(mImpl->agentIdx);
		//agent->state = DT_CROWDAGENT_STATE_INVALID;
		agent->state = DT_CROWDAGENT_STATE_OFFMESH;

	}

}


void fq::game_module::NavigationAgent::SetRadius(float radius)
{
	mImpl->agentParams.radius = radius;
	if (mImpl->crowd != nullptr)
		mImpl->crowd->updateAgentParameters(mImpl->agentIdx, &mImpl->agentParams);
}

DirectX::SimpleMath::Vector3 fq::game_module::NavigationAgent::GetTargetPosition()
{
	return { mImpl->targetPos[0], mImpl->targetPos[1], mImpl->targetPos[2] };
}

void fq::game_module::NavigationAgent::MoveTo(DirectX::SimpleMath::Vector3 destination)
{
	if (mPathFindingSystem == nullptr)
	{
		return;
	}

	if (!mPathFindingSystem->HasNavigationMesh())
	{
		return;
	}

	const dtQueryFilter* filter{ mImpl->crowd->getFilter(0) };
	const dtCrowdAgent* agent = mImpl->crowd->getAgent(mImpl->agentIdx);
	
	const float* halfExtents = mImpl->crowd->getQueryExtents();

	mPathFindingSystem->GetNavQuery()->findNearestPoly(reinterpret_cast<float*>(&destination), halfExtents, filter, &mImpl->targetRef, mImpl->targetPos);
	mImpl->crowd->requestMoveTarget(mImpl->agentIdx, mImpl->targetRef, mImpl->targetPos);

	mbIsStop = false;

	//auto param = mImpl->crowd->getObstacleAvoidanceParams(mImpl->agentParams.obstacleAvoidanceType);
	//
	//dtObstacleAvoidanceParams nextParam{*param};
	//
	//nextParam.horizTime = 20;
	//nextParam.adaptiveDivs = 20;
	//nextParam.adaptiveRings = 7;
	//nextParam.adaptiveDepth = 10;
	//nextParam.weightSide = 3;
	//mImpl->crowd->setObstacleAvoidanceParams(mImpl->agentParams.obstacleAvoidanceType, &nextParam);
}

int fq::game_module::NavigationAgent::GetAgentIndex()
{
	return mImpl->agentIdx;
}

std::shared_ptr<fq::game_module::Component> fq::game_module::NavigationAgent::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	std::shared_ptr<NavigationAgent> cloneAgent = std::dynamic_pointer_cast<NavigationAgent>(clone);

	if (cloneAgent == nullptr) // 새로 생성해서 복사본을 준다
	{
		cloneAgent = game_module::ObjectPool::GetInstance()->Assign<NavigationAgent>(*this);
	}
	else // clone에 데이터를 복사한다.
	{
		// 기본 대입 연산자 호출한다.
		*cloneAgent = *this;
	}

	cloneAgent->mImpl = nullptr;
	cloneAgent->mPathFindingSystem = nullptr;

	return cloneAgent;
}

void fq::game_module::NavigationAgent::CreateAgentData()
{
	mImpl = new Impl(this);
}

void fq::game_module::NavigationAgent::Stop()
{
	mImpl->crowd->resetMoveTarget(mImpl->agentIdx);
	mbIsStop = true;
}

void fq::game_module::NavigationAgent::DeleteAgentData()
{
	mImpl->crowd->removeAgent(mImpl->agentIdx);
	delete mImpl;
}

bool fq::game_module::NavigationAgent::HasReachedDestination() const
{
	const dtCrowdAgent* agent = mImpl->crowd->getAgent(mImpl->agentIdx);

	if (!agent)
	{
		return false;
	}

	constexpr float tolerance = 0.5f;  // 허용오차  
	float dx = mImpl->targetPos[0] - agent->npos[0];
	float dy = mImpl->targetPos[1] - agent->npos[1];
	float dz = mImpl->targetPos[2] - agent->npos[2];

	float distance = sqrt(dx * dx + dy * dy + dz * dz);

	return distance < tolerance;
}

bool fq::game_module::NavigationAgent::IsValid(DirectX::SimpleMath::Vector3 position)
{
	const dtQueryFilter* filter{ mImpl->crowd->getFilter(0) };
	const float* halfExtents = mImpl->crowd->getQueryExtents();

	dtStatus status = mPathFindingSystem->GetNavQuery()->findNearestPoly(reinterpret_cast<float*>(&position), halfExtents, filter, &mImpl->targetRef, mImpl->targetPos);
	dtPolyRef nearestPolyRef = 0;
	if (dtStatusSucceed(status) && nearestPolyRef != 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}
