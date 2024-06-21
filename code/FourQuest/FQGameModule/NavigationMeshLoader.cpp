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

	if (cloneAgent == nullptr) // 새로 생성해서 복사본을 준다
	{
		cloneAgent = game_module::ObjectPool::GetInstance()->Assign<NavigationMeshLoader>(*this);
	}
	else // clone에 데이터를 복사한다.
	{
		// 기본 대입 연산자 호출한다.
		*cloneAgent = *this;
	}

	return cloneAgent;
}
