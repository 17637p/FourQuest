#include "SkinnedMeshRenderer.h"

#include "Scene.h"
#include "EventManager.h"
#include "Event.h"

fq::game_module::SkinnedMeshRenderer::SkinnedMeshRenderer()
	:mMeshInfomation{}
	, mSkinnedMeshObject(nullptr)
	, mModelPath{}
{
}

fq::game_module::SkinnedMeshRenderer::~SkinnedMeshRenderer()
{

}

entt::meta_handle fq::game_module::SkinnedMeshRenderer::GetHandle()
{
	return *this;
}

std::shared_ptr<fq::game_module::Component> fq::game_module::SkinnedMeshRenderer::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	auto cloneMesh = std::dynamic_pointer_cast<SkinnedMeshRenderer>(clone);

	if (cloneMesh == nullptr) // 새로 생성해서 복사본을 준다
	{
		cloneMesh = ObjectPool::GetInstance()->Assign<SkinnedMeshRenderer>(*this);
	}
	else // clone에 데이터를 복사한다.
	{
		// 기본 대입 연산자 호출한다.
		*cloneMesh = *this;
	}

	cloneMesh->mSkinnedMeshObject = nullptr;

	return cloneMesh;
}