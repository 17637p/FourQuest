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

fq::game_module::Component* fq::game_module::SkinnedMeshRenderer::Clone(Component* clone /* = nullptr */) const
{
	SkinnedMeshRenderer* cloneMesh = static_cast<SkinnedMeshRenderer*>(clone);

	if (cloneMesh == nullptr) // ���� �����ؼ� ���纻�� �ش�
	{
		cloneMesh = new SkinnedMeshRenderer(*this);
	}
	else // clone�� �����͸� �����Ѵ�.
	{
		// �⺻ ���� ������ ȣ���Ѵ�.
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