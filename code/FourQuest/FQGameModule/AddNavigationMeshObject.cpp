#include "AddNavigationMeshObject.h"

fq::game_module::AddNavigationMeshObject::AddNavigationMeshObject()
	:mIsUsed(true)
{
}

std::shared_ptr<fq::game_module::Component> fq::game_module::AddNavigationMeshObject::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	std::shared_ptr<AddNavigationMeshObject> cloneAddNavigationMeshObject = std::dynamic_pointer_cast<AddNavigationMeshObject>(clone);

	if (cloneAddNavigationMeshObject == nullptr) // ���� �����ؼ� ���纻�� �ش�
	{
		cloneAddNavigationMeshObject = game_module::ObjectPool::GetInstance()->Assign<AddNavigationMeshObject>(*this);
	}
	else // clone�� �����͸� �����Ѵ�.
	{
		// �⺻ ���� ������ ȣ���Ѵ�.
		*cloneAddNavigationMeshObject = *this;
	}

	return cloneAddNavigationMeshObject;
}

