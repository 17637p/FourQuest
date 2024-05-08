#include "SkinnedMeshRenderer.h"

fq::game_module::SkinnedMeshRenderer::SkinnedMeshRenderer()
	:mMeshInfomation{}
	,mSkinnedMeshObject(nullptr)
{

}

fq::game_module::SkinnedMeshRenderer::~SkinnedMeshRenderer()
{

}

entt::meta_handle fq::game_module::SkinnedMeshRenderer::GetHandle()
{
	return *this;
}


std::shared_ptr<fq::game_module::Component> fq::game_module::SkinnedMeshRenderer::Clone(std::shared_ptr<Component> clone /*= nullptr*/) const
{
	auto cloneMesh = std::dynamic_pointer_cast<SkinnedMeshRenderer>(clone);

	if (cloneMesh == nullptr) // ���� �����ؼ� ���纻�� �ش�
	{
		cloneMesh = ObjectPool::GetInstance()->AssignComponent<SkinnedMeshRenderer>(*this);
	}
	else // clone�� �����͸� �����Ѵ�.
	{
		*cloneMesh = *this;
	}

	cloneMesh->mSkinnedMeshObject = nullptr;

	return cloneMesh;
}

void fq::game_module::SkinnedMeshRenderer::SetMeshName(std::string name)
{
	mMeshInfomation.MeshName = name;
}

void fq::game_module::SkinnedMeshRenderer::SetMaterials(std::vector<std::string> materials)
{
	mMeshInfomation.MaterialNames = std::move(materials);
}


void fq::game_module::SkinnedMeshRenderer::SetMeshObjectInfomation(fq::graphics::MeshObjectInfo info)
{
	mMeshInfomation = std::move(info);
}

void fq::game_module::SkinnedMeshRenderer::SetModelPath(std::string path)
{
	mMeshInfomation.ModelPath = path;

}
