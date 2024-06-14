#include "NavigationAgent.h"

#include "NavigationAgentImpl.h"
#include "../FQGameEngine/PathFindingSystem.h"

fq::game_module::NavigationAgent::NavigationAgent()
	:impl(new Impl{this}),
	mPathFindingSystem(nullptr)
{

}

fq::game_module::NavigationAgent::~NavigationAgent()
{

}

void fq::game_module::NavigationAgent::OnUpdate(float dt)
{

}

void fq::game_module::NavigationAgent::RegisterNavigationField(fq::game_engine::PathFindingSystem* pathFindingSystem)
{
	//m_crowd = dtAllocCrowd();
	// addagent
	mPathFindingSystem = pathFindingSystem;

	Transform* agentTransform =  GetGameObject()->GetComponent<Transform>();
	impl->agentIdx = pathFindingSystem->AddAgent(agentTransform->GetWorldPosition(), &impl->agentParams);
	// 등록 
}

float fq::game_module::NavigationAgent::GetSpeed()
{
	return impl->agentParams.maxSpeed;
}

void fq::game_module::NavigationAgent::SetSpeed(float speed)
{
	impl->agentParams.maxSpeed = speed;
	if (impl->crowd != nullptr)
		impl->crowd->updateAgentParameters(impl->agentIdx, &impl->agentParams);
}

float fq::game_module::NavigationAgent::GetAcceleration()
{
	return impl->agentParams.maxAcceleration;
}

void fq::game_module::NavigationAgent::SetAcceleration(float accel)
{
	impl->agentParams.maxAcceleration = accel;
	if (impl->crowd != nullptr)
		impl->crowd->updateAgentParameters(impl->agentIdx, &impl->agentParams);
}

float fq::game_module::NavigationAgent::GetRadius()
{
	return impl->agentParams.radius;
}

void fq::game_module::NavigationAgent::SetRadius(float radius)
{
	impl->agentParams.radius = radius;
	if (impl->crowd != nullptr)
		impl->crowd->updateAgentParameters(impl->agentIdx, &impl->agentParams);
}

DirectX::SimpleMath::Vector3 fq::game_module::NavigationAgent::GetTargetPosition()
{
	return { impl->targetPos[0], impl->targetPos[1], impl->targetPos[2] };
}

void fq::game_module::NavigationAgent::MoveTo(DirectX::SimpleMath::Vector3 destination)
{
	if (mPathFindingSystem == nullptr)
		return;

	impl->crowd = mPathFindingSystem->GetCrowd();

	const dtQueryFilter* filter{ impl->crowd->getFilter(0) };
	const dtCrowdAgent* agent = impl->crowd->getAgent(impl->agentIdx);
	const float* halfExtents = impl->crowd->getQueryExtents();


	mPathFindingSystem->GetNavQuery()->findNearestPoly(reinterpret_cast<float*>(&destination), halfExtents, filter, &impl->targetRef, impl->targetPos);
	impl->crowd->requestMoveTarget(impl->agentIdx, impl->targetRef, impl->targetPos);
}

int fq::game_module::NavigationAgent::GetAgentIndex()
{
	return impl->agentIdx;
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

	return cloneAgent;
}
