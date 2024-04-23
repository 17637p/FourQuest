#include "StaticMeshRenderer.h"


fq::game_module::StaticMeshRenderer::StaticMeshRenderer()
	:mStaticMeshObject(nullptr)
{}

fq::game_module::StaticMeshRenderer::~StaticMeshRenderer()
{
}

entt::meta_handle fq::game_module::StaticMeshRenderer::GetHandle()
{
	return *this;
}

fq::game_module::Component* fq::game_module::StaticMeshRenderer::Clone(Component* clone /* = nullptr */) const
{
	StaticMeshRenderer* cloneMesh = static_cast<StaticMeshRenderer*>(clone);

	if (cloneMesh == nullptr) // ���� �����ؼ� ���纻�� �ش�
	{
		cloneMesh = new StaticMeshRenderer(*this);
	}
	else // clone�� �����͸� �����Ѵ�.
	{
		// �⺻ ���� ������ ȣ���Ѵ�.
		*cloneMesh = *this;
	}
	
	cloneMesh->SetStaticMeshObject(nullptr);

	return cloneMesh;
}

void fq::game_module::StaticMeshRenderer::SetModelPath(std::string path)
{
	mMeshInfomation.ModelPath = path;
}

void fq::game_module::StaticMeshRenderer::SetMeshName(std::string name)
{
	mMeshInfomation.MeshName = name;
}

void fq::game_module::StaticMeshRenderer::SetMeshObjectInfomation(fq::graphics::MeshObjectInfo info)
{
	mMeshInfomation = std::move(info);

}

void fq::game_module::StaticMeshRenderer::SetMaterials(std::vector<std::string> materials)
{
	mMeshInfomation.MaterialNames = std::move(materials);
}

