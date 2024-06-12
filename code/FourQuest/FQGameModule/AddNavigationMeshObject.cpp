#include "AddNavigationMeshObject.h"

fq::game_module::AddNavigationMeshObject::AddNavigationMeshObject()
	:mIsUsed(true)
{
}

std::shared_ptr<fq::game_module::Component> fq::game_module::AddNavigationMeshObject::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	std::shared_ptr<AddNavigationMeshObject> cloneAddNavigationMeshObject = std::dynamic_pointer_cast<AddNavigationMeshObject>(clone);

	if (cloneAddNavigationMeshObject == nullptr) // 새로 생성해서 복사본을 준다
	{
		cloneAddNavigationMeshObject = game_module::ObjectPool::GetInstance()->Assign<AddNavigationMeshObject>(*this);
	}
	else // clone에 데이터를 복사한다.
	{
		// 기본 대입 연산자 호출한다.
		*cloneAddNavigationMeshObject = *this;
	}

	return cloneAddNavigationMeshObject;
}

