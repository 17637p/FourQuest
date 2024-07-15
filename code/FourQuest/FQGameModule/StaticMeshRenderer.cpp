#include "StaticMeshRenderer.h"


fq::game_module::StaticMeshRenderer::StaticMeshRenderer()
	:mStaticMeshObject(nullptr)
	, mMeshInfomation{}
	, mIsNavigationMeshUsed(true)
	, mTexturePath{}
{}

fq::game_module::StaticMeshRenderer::~StaticMeshRenderer()
{
}

void fq::game_module::StaticMeshRenderer::SetMaterialInfos(std::vector<fq::graphics::MaterialInfo> materialInfos)
{
	mMaterialInfos = std::move(materialInfos);

	size_t MAX_COUNT = std::min<size_t>(mMaterialInfos.size(), mMaterialInterfaces.size());

	for (size_t i = 0; i < MAX_COUNT; ++i)
	{
		mMaterialInterfaces[i]->SetInfo(mMaterialInfos[i]);
	}
}

void fq::game_module::StaticMeshRenderer::SetMaterialInterfaces(std::vector<std::shared_ptr<fq::graphics::IMaterial>> materialInterfaces)
{
	mMaterialInterfaces = materialInterfaces;
	mMaterialInfos.clear();

	for (auto& materialInterface : materialInterfaces)
	{
		mMaterialInfos.push_back(materialInterface->GetInfo());
	}
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
