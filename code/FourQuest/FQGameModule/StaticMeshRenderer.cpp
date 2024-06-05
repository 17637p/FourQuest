#include "StaticMeshRenderer.h"


fq::game_module::StaticMeshRenderer::StaticMeshRenderer()
	:mStaticMeshObject(nullptr)
	, mMeshInfomation{}
	, mOutlineColor{0.f,0.f,0.f,0.f}
{}

fq::game_module::StaticMeshRenderer::~StaticMeshRenderer()
{
}

entt::meta_handle fq::game_module::StaticMeshRenderer::GetHandle()
{
	return *this;
}

std::shared_ptr<fq::game_module::Component> fq::game_module::StaticMeshRenderer::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	auto cloneMesh = std::dynamic_pointer_cast<StaticMeshRenderer>(clone);

	if (cloneMesh == nullptr) // ���� �����ؼ� ���纻�� �ش�
	{
		cloneMesh = ObjectPool::GetInstance()->Assign<StaticMeshRenderer>(*this);
	}
	else // clone�� �����͸� �����Ѵ�.
	{
		// �⺻ ���� ������ ȣ���Ѵ�.
		*cloneMesh = *this;
	}

	cloneMesh->mStaticMeshObject = nullptr;

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

void fq::game_module::StaticMeshRenderer::SetOutlineColor(DirectX::SimpleMath::Color color)
{
	mOutlineColor = color;

	if (mStaticMeshObject)
		mStaticMeshObject->SetOutlineColor(mOutlineColor);
}

