#include "GameModuleRegister.h"

#include "../FQReflect/FQReflect.h"
#include "GameModuleEnum.h"
#include "GameObject.h"
#include "Transform.h"
#include "Camera.h"
#include "StaticMeshRenderer.h"
#include "SkinnedMeshRenderer.h"

void fq::game_module::RegisterMetaData()
{
	using namespace entt::literals;
	using namespace fq::game_module;

	// Tag
	entt::meta<ETag>()
		.prop(fq::reflect::prop::Name, "Tag")
		.conv<std::underlying_type_t<ETag>>()
		.data<ETag::Untagged>("Untagged"_hs)
		.prop(fq::reflect::prop::Name, "Untagged")
		.data<ETag::Camera>("Camera"_hs)
		.prop(fq::reflect::prop::Name, "Camera")
		.data<ETag::Player>("Player"_hs)
		.prop(fq::reflect::prop::Name, "Player");

	// GameObject 
	entt::meta<GameObject>()
		.type("GameObject"_hs)
		.prop(fq::reflect::prop::Name, "GameObject")
		.data<&GameObject::SetName, &GameObject::GetName>("mName"_hs)
		.prop(fq::reflect::prop::Name, "mName")
		.data<&GameObject::SetTag, &GameObject::GetTag>("mTag"_hs)
		.prop(fq::reflect::prop::Name, "mTag");

	// Transform
	entt::meta<Transform>()
		.type("Transform"_hs)
		.prop(fq::reflect::prop::Name, "Transform")
		.data<&Transform::SetLocalPosition, &Transform::GetLocalPosition>("Position"_hs)
		.prop(fq::reflect::prop::Name, "Position")
		.prop(fq::reflect::prop::Comment, u8"·ÎÄÃ ÁÂÇ¥")
		.data<&Transform::SetLocalRotation, &Transform::GetLocalRotation>("Rotation"_hs)
		.prop(fq::reflect::prop::Name, "Rotation")
		.data<&Transform::SetLocalScale, &Transform::GetLocalScale>("Scale"_hs)
		.prop(fq::reflect::prop::Name, "Scale")
		.base<Component>();

	// Camera
	entt::meta<Camera>()
		.type("Camera"_hs)
		.prop(fq::reflect::prop::Name, "Camera")
		.data<&Camera::SetFieldOfView, &Camera::GetFieldOfView>("FOV"_hs)
		.prop(fq::reflect::prop::Name, "FOV")
		.prop(fq::reflect::prop::Comment, "0 ~ 180")
		.data<&Camera::SetMainCamera, &Camera::IsMain>("IsMain"_hs)
		.prop(fq::reflect::prop::Name, "IsMain")
		.data<&Camera::SetFarPlain, &Camera::GetFarPlain>("FarPlain"_hs)
		.prop(fq::reflect::prop::Name, "FarPlain")
		.data<&Camera::SetNearPlain, &Camera::GetNearPlain>("NearPlain"_hs)
		.prop(fq::reflect::prop::Name, "NearPlain")
		.base<Component>();

	// StaticMeshRenderer
	entt::meta<StaticMeshRenderer>()
		.type("StaticMeshRenderer"_hs)
		.prop(fq::reflect::prop::Name, "StaticMeshRenderer")
		.data<&StaticMeshRenderer::SetModelPath , &StaticMeshRenderer::GetModelPath>("ModelPath"_hs)
		.prop(fq::reflect::prop::Name, "ModelPath")
		.prop(fq::reflect::prop::RelativePath)
		.data<&StaticMeshRenderer::SetMeshName, &StaticMeshRenderer::GetMeshName>("MeshName"_hs)
		.prop(fq::reflect::prop::Name, "MeshName")
		.data<&StaticMeshRenderer::SetMaterials, &StaticMeshRenderer::GetMaterials>("Materials"_hs)
		.prop(fq::reflect::prop::Name, "Materials")
		.base<Component>();

	// SkinnedMeshRenderer
	entt::meta<SkinnedMeshRenderer>()
		.type("SkinnedMeshRenderer"_hs)
		.prop(fq::reflect::prop::Name, "SkinnedMeshRenderer")
		.data<&SkinnedMeshRenderer::SetModelPath, &SkinnedMeshRenderer::GetModelPath>("ModelPath"_hs)
		.prop(fq::reflect::prop::Name, "ModelPath")
		.prop(fq::reflect::prop::RelativePath)
		.data<&SkinnedMeshRenderer::SetMeshName, &SkinnedMeshRenderer::GetMeshName>("MeshName"_hs)
		.prop(fq::reflect::prop::Name, "MeshName")
		.data<&SkinnedMeshRenderer::SetMaterials, &SkinnedMeshRenderer::GetMaterials>("Materials"_hs)
		.prop(fq::reflect::prop::Name, "Materials")
		.base<Component>();

}
