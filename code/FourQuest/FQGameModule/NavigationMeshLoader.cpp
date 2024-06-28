#include "NavigationMeshLoader.h"

fq::game_module::NavigationMeshLoader::NavigationMeshLoader()
	:mNavigationMeshPath("")
{
}

fq::game_module::NavigationMeshLoader::~NavigationMeshLoader()
{

}

std::shared_ptr<fq::game_module::Component> fq::game_module::NavigationMeshLoader::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	std::shared_ptr<NavigationMeshLoader> cloneAgent = std::dynamic_pointer_cast<NavigationMeshLoader>(clone);

	if (cloneAgent == nullptr) // ���� �����ؼ� ���纻�� �ش�
	{
		cloneAgent = game_module::ObjectPool::GetInstance()->Assign<NavigationMeshLoader>(*this);
	}
	else // clone�� �����͸� �����Ѵ�.
	{
		// �⺻ ���� ������ ȣ���Ѵ�.
		*cloneAgent = *this;
	}

	return cloneAgent;
}
