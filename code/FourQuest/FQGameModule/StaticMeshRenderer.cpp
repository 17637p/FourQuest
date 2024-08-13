#include "StaticMeshRenderer.h"

#include "Scene.h"
#include "EventManager.h"
#include "Event.h"

fq::game_module::StaticMeshRenderer::StaticMeshRenderer()
	:mStaticMeshObject(nullptr)
	, mMeshInfomation{}
	, mIsNavigationMeshUsed(false)
	, mPrevApplyTransform{}
	, mbIsRender{ true }
{
}

void fq::game_module::StaticMeshRenderer::SetIsRender(bool bIsRender)
{
	mbIsRender = bIsRender;

	if (mStaticMeshObject != nullptr)
	{
		mStaticMeshObject->SetIsRender(mbIsRender);
	}
}

entt::meta_handle fq::game_module::StaticMeshRenderer::GetHandle()
{
	return *this;
}

std::shared_ptr<fq::game_module::Component> fq::game_module::StaticMeshRenderer::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	auto cloneMesh = std::dynamic_pointer_cast<StaticMeshRenderer>(clone);

	if (cloneMesh == nullptr) // 새로 생성해서 복사본을 준다
	{
		cloneMesh = ObjectPool::GetInstance()->Assign<StaticMeshRenderer>(*this);
	}
	else // clone에 데이터를 복사한다.
	{
		// 기본 대입 연산자 호출한다.
		*cloneMesh = *this;
	}

	cloneMesh->mStaticMeshObject = nullptr;

	return cloneMesh;
}

void fq::game_module::StaticMeshRenderer::SetMaterialInterfaces(std::vector<std::shared_ptr<fq::graphics::IMaterial>> materialInterfaces)
{
	mMaterialInterfaces = materialInterfaces;

	if (mStaticMeshObject != nullptr)
	{
		mStaticMeshObject->SetMaterials(mMaterialInterfaces);
	}
}

void fq::game_module::StaticMeshRenderer::SetMeshObjectInfomation(const fq::graphics::MeshObjectInfo& info)
{
	mMeshInfomation = info;

	if (mStaticMeshObject != nullptr)
	{
		mStaticMeshObject->SetMeshObjectInfo(mMeshInfomation);
	}
}


void fq::game_module::StaticMeshRenderer::SetLightmapUVScaleOffset(const DirectX::SimpleMath::Vector4& scaleOffset)
{
	mLightmapScaleOffset = scaleOffset;

	if (mStaticMeshObject != nullptr)
	{
		mStaticMeshObject->SetLightmapUVScaleOffset(mLightmapScaleOffset);
	}
}

const DirectX::SimpleMath::Vector4& fq::game_module::StaticMeshRenderer::GetLightmapUVScaleOffset() const
{
	return mLightmapScaleOffset;
}

void fq::game_module::StaticMeshRenderer::SetLightmapIndex(unsigned int lightmapIndex)
{
	mLightmapIndex = lightmapIndex;

	if (mStaticMeshObject != nullptr)
	{
		mStaticMeshObject->SetLightmapIndex(mLightmapIndex);
	}
}

int fq::game_module::StaticMeshRenderer::GetLightmapIndex() const
{
	return mLightmapIndex;
}

void fq::game_module::StaticMeshRenderer::SetIsStatic(bool bIsStatic)
{
	using namespace fq::graphics;

	mbIsStatic = bIsStatic;
	mMeshInfomation.ObjectType = mbIsStatic ? MeshObjectInfo::EObjectType::Static : MeshObjectInfo::EObjectType::Dynamic;

	if (mStaticMeshObject != nullptr)
	{
		mStaticMeshObject->SetMeshObjectInfo(mMeshInfomation);
	}
}

bool fq::game_module::StaticMeshRenderer::GetIsStatic() const
{
	return mbIsStatic;
}