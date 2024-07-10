#include "SkinnedMeshRenderer.h"

fq::game_module::SkinnedMeshRenderer::SkinnedMeshRenderer()
	:mMeshInfomation{}
	,mSkinnedMeshObject(nullptr)
	,mTexturePath{}
	,mModelPath{}
{
}

fq::game_module::SkinnedMeshRenderer::~SkinnedMeshRenderer()
{

}

void fq::game_module::SkinnedMeshRenderer::SetMaterialInfos(std::vector<fq::graphics::MaterialInfo> materialInfos)
{
	mMaterialInfos = std::move(materialInfos);

	size_t MAX_COUNT = std::min<size_t>(mMaterialInfos.size(), mMaterialInterfaces.size());

	for (size_t i = 0; i < MAX_COUNT; ++i)
	{
		mMaterialInterfaces[i]->SetInfo(mMaterialInfos[i]);
	}
}

void fq::game_module::SkinnedMeshRenderer::SetMaterialInterfaces(std::vector<std::shared_ptr<fq::graphics::IMaterial>> materialInterfaces)
{
	mMaterialInterfaces = materialInterfaces;
	mMaterialInfos.clear();

	for (auto& materialInterface : materialInterfaces)
	{
		mMaterialInfos.push_back(materialInterface->GetInfo());
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

