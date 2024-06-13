#include "NavigationAgentImpl.h"

fq::game_module::NavigationAgent::Impl::Impl(NavigationAgent* navAgentComponent)
	:navAgentComponent(navAgentComponent)
{
}

fq::game_module::NavigationAgent::Impl::~Impl()
{
	if (crowd != nullptr && agentIdx != -1)
		crowd->removeAgent(agentIdx);
}
