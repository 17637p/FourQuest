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

	if (cloneMesh == nullptr) // 새로 생성해서 복사본을 준다
	{
		cloneMesh = new StaticMeshRenderer(*this);
	}
	else // clone에 데이터를 복사한다.
	{
		// 기본 대입 연산자 호출한다.
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

