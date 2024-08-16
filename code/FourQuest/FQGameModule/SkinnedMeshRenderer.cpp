#include "SkinnedMeshRenderer.h"

#include "Scene.h"
#include "EventManager.h"
#include "Event.h"

fq::game_module::SkinnedMeshRenderer::SkinnedMeshRenderer()
	:mMeshInfomation{}
	, mSkinnedMeshObject(nullptr)
	, mModelPath{}
	, mbIsRender{ true }
{
}

fq::game_module::SkinnedMeshRenderer::~SkinnedMeshRenderer()
{

}

void fq::game_module::SkinnedMeshRenderer::SetIsRender(bool bIsRender)
{
	mbIsRender = bIsRender;

	if (mSkinnedMeshObject != nullptr)
	{
		mSkinnedMeshObject->SetIsRender(mbIsRender);
	}
}

void fq::game_module::SkinnedMeshRenderer::SetMaterialInstanceInfo(const fq::graphics::MaterialInstanceInfo& info)
{
	mMaterialInstanceInfo = info;

	if (mSkinnedMeshObject != nullptr)
	{
		mSkinnedMeshObject->SetMaterialInstanceInfo(mMaterialInstanceInfo);
	}
}

entt::meta_handle fq::game_module::SkinnedMeshRenderer::GetHandle()
{
	return *this;
}

std::shared_ptr<fq::game_module::Component> fq::game_module::SkinnedMeshRenderer::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	auto cloneMesh = std::dynamic_pointer_cast<SkinnedMeshRenderer>(clone);

	if (cloneMesh == nullptr) // ���� �����ؼ� ���纻�� �ش�
	{
		cloneMesh = ObjectPool::GetInstance()->Assign<SkinnedMeshRenderer>(*this);
	}
	else // clone�� �����͸� �����Ѵ�.
	{
		// �⺻ ���� ������ ȣ���Ѵ�.
		*cloneMesh = *this;
	}

	cloneMesh->mSkinnedMeshObject = nullptr;

	return cloneMesh;
}

void fq::game_module::SkinnedMeshRenderer::SetMeshObjectInfomation(const fq::graphics::MeshObjectInfo& info)
{
	mMeshInfomation = info;

	if (mSkinnedMeshObject != nullptr)
	{
		mSkinnedMeshObject->SetMeshObjectInfo(mMeshInfomation);
	}
}


void fq::game_module::SkinnedMeshRenderer::SetMaterialInterfaces(std::vector<std::shared_ptr<fq::graphics::IMaterial>> materialInterfaces) {
	mMaterialInterfaces = materialInterfaces;

	if (mSkinnedMeshObject != nullptr)
	{
		mSkinnedMeshObject->SetMaterials(mMaterialInterfaces);
	}
}