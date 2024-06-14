#include "NavigationAgent.h"

#include "NavigationAgentImpl.h"
#include "../FQGameEngine/PathFindingSystem.h"

#include "../FQGameModule/Transform.h"

fq::game_module::NavigationAgent::NavigationAgent()
	:mImpl(nullptr),
	mPathFindingSystem(nullptr)
{

}

fq::game_module::NavigationAgent::~NavigationAgent()
{
	delete mImpl;
}

void fq::game_module::NavigationAgent::OnUpdate(float dt)
{

}

void fq::game_module::NavigationAgent::RegisterNavigationField(fq::game_engine::PathFindingSystem* pathFindingSystem)
{
	// addagent
	mPathFindingSystem = pathFindingSystem;

	Transform* agentTransform =  GetGameObject()->GetComponent<Transform>();
	mImpl->agentIdx = pathFindingSystem->AddAgent(agentTransform->GetWorldPosition(), &mImpl->agentParams);

	mImpl->crowd = mPathFindingSystem->GetCrowd();
	// 등록 
}

float fq::game_module::NavigationAgent::GetSpeed()
{
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
	return mImpl->agentParams.radius;
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
		return;

	const dtQueryFilter* filter{ mImpl->crowd->getFilter(0) };
	const dtCrowdAgent* agent = mImpl->crowd->getAgent(mImpl->agentIdx);
	const float* halfExtents = mImpl->crowd->getQueryExtents();

	mPathFindingSystem->GetNavQuery()->findNearestPoly(reinterpret_cast<float*>(&destination), halfExtents, filter, &mImpl->targetRef, mImpl->targetPos);
	mImpl->crowd->requestMoveTarget(mImpl->agentIdx, mImpl->targetRef, mImpl->targetPos);
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

void fq::game_module::NavigationAgent::OnStart()
{
	mImpl = new Impl{ this };
}
