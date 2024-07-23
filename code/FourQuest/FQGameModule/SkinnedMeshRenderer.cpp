#include "SkinnedMeshRenderer.h"

#include "Scene.h"
#include "EventManager.h"
#include "Event.h"

fq::game_module::SkinnedMeshRenderer::SkinnedMeshRenderer()
	:mMeshInfomation{}
	, mSkinnedMeshObject(nullptr)
	, mTexturePath{}
	, mModelPath{}
{
}

fq::game_module::SkinnedMeshRenderer::~SkinnedMeshRenderer()
{

}

const std::vector<std::string>& fq::game_module::SkinnedMeshRenderer::GetMaterials() const
{
	return mMaterialNames;
}

void fq::game_module::SkinnedMeshRenderer::SetMaterials(std::vector<std::string> materials)
{
	mMaterialNames = std::move(materials);
}

void fq::game_module::SkinnedMeshRenderer::UpdateMaterialInfoByMaterialInterface()
{
	mMaterialInfos.clear();

	for (std::shared_ptr<fq::graphics::IMaterial>& materialInterface : mMaterialInterfaces)
	{
		mMaterialInfos.push_back(materialInterface->GetInfo());
	}
}

const std::vector<fq::graphics::MaterialInfo>& fq::game_module::SkinnedMeshRenderer::GetMaterialInfos() const
{
	return mMaterialInfos;
}

void fq::game_module::SkinnedMeshRenderer::SetMaterialInfos(std::vector<fq::graphics::MaterialInfo> materialInfos)
{
	mMaterialInfos = std::move(materialInfos);

	size_t MAX_COUNT = std::min<size_t>(mMaterialInfos.size(), mMaterialInterfaces.size());

	for (size_t i = 0; i < MAX_COUNT; ++i)
	{
		mMaterialInterfaces[i]->SetInfo(mMaterialInfos[i]);
	}

	fq::game_module::Scene* scene = GetScene();

	if (scene != nullptr)
	{
		GetScene()->GetEventManager()->FireEvent<fq::event::UpdateMaterialInfo>({});
	}
}

void fq::game_module::SkinnedMeshRenderer::SetMaterialInterfaces(std::vector<std::shared_ptr<fq::graphics::IMaterial>> materialInterfaces)
{
	mMaterialInterfaces = materialInterfaces;
	UpdateMaterialInfoByMaterialInterface();
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

void fq::game_module::SkinnedMeshRenderer::SetTexturePath(std::string path)
{
	mTexturePath = path;
}


