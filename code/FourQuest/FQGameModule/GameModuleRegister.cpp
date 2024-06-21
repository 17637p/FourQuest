#include "GameModuleRegister.h"

#include "../FQReflect/FQReflect.h"
#include "../FQCommon/FQCommonGraphics.h"
#include "GameModuleEnum.h"
#include "GameObject.h"
#include "Transform.h"
#include "Camera.h"
#include "StaticMeshRenderer.h"
#include "SkinnedMeshRenderer.h"
#include "Light.h"
#include "Particle.h"
#include "Decal.h"
#include "ImageUI.h"
#include "Socket.h"

// Physics
#include "Terrain.h"
#include "RigidBody.h"
#include "BoxCollider.h"
#include "SphereCollider.h"
#include "CapsuleCollider.h"
#include "MeshCollider.h"
#include "CharacterController.h"

//
#include "SoundClip.h"
#include "PrefabResource.h"
#include "PrefabTest.h"
#include "AnimationStateNode.h"
#include "Animator.h"
#include "LogStateBehaviour.h"

// PathFinding
#include "NavigationAgent.h"
#include "NavigationMeshLoader.h"

void fq::game_module::RegisterMetaData()
{
	using namespace entt::literals;
	using namespace fq::game_module;

	//////////////////////////////////////////////////////////////////////////
	//                            GameObject                                //
	//////////////////////////////////////////////////////////////////////////

	// Tag
	entt::meta<ETag>()
		.prop(fq::reflect::prop::Name, "Tag")
		.conv<std::underlying_type_t<ETag>>()
		.data<ETag::Untagged>("Untagged"_hs) // 0
		.prop(fq::reflect::prop::Name, "Untagged")
		.data<ETag::Camera>("Camera"_hs) // 1
		.prop(fq::reflect::prop::Name, "Camera")
		.data<ETag::Player>("Player"_hs) // 2
		.prop(fq::reflect::prop::Name, "Player")
		.data<ETag::Alien>("Alien"_hs) // 3
		.prop(fq::reflect::prop::Name, "Alien")
		.data<ETag::Obstacle>("Obstacle"_hs) // 4
		.prop(fq::reflect::prop::Name, "Obstacle")
		.data<ETag::Wall>("Wall"_hs) // 5
		.prop(fq::reflect::prop::Name, "Wall")
		.data<ETag::PlayerAttack>("PlayerAttack"_hs) // 6
		.prop(fq::reflect::prop::Name, "PlayerAttack")
		.data<ETag::MonsterAttack>("MonsterAttack"_hs) // 7
		.prop(fq::reflect::prop::Name, "MonsterAttack")
		.data<ETag::Armour>("Armour"_hs) // 8
		.prop(fq::reflect::prop::Name, "Armour")
		.data<ETag::Floor>("Floor"_hs) // 9
		.prop(fq::reflect::prop::Name, "Floor");

	// GameObject
	entt::meta<GameObject>()
		.type("GameObject"_hs)
		.prop(fq::reflect::prop::Name, "GameObject")
		.data<&GameObject::SetName, &GameObject::GetName>("mName"_hs)
		.prop(fq::reflect::prop::Name, "mName")
		.data<&GameObject::SetTag, &GameObject::GetTag>("mTag"_hs)
		.prop(fq::reflect::prop::Name, "mTag");

	// PrefabResource
	entt::meta<PrefabResource>()
		.type("PrefabResource"_hs)
		.prop(fq::reflect::prop::Name, "PrefabResource")
		.data<&PrefabResource::SetPrefabPath, &PrefabResource::GetPrefabPath>("Path"_hs)
		.prop(fq::reflect::prop::Name, "Path")
		.prop(fq::reflect::prop::RelativePath);

	//////////////////////////////////////////////////////////////////////////
	//                            Component                                 //
	//////////////////////////////////////////////////////////////////////////

	// Transform
	entt::meta<Transform>()
		.type("Transform"_hs)
		.prop(fq::reflect::prop::Name, "Transform")
		.prop(fq::reflect::prop::Label, "Miscellaneous")
		.data<&Transform::SetLocalPosition, &Transform::GetLocalPosition>("Position"_hs)
		.prop(fq::reflect::prop::Name, "Position")
		.prop(fq::reflect::prop::Comment, u8"���� ��ǥ")
		.data<&Transform::SetLocalRotation, &Transform::GetLocalRotation>("Rotation"_hs)
		.prop(fq::reflect::prop::Name, "Rotation")
		.data<&Transform::SetLocalScale, &Transform::GetLocalScale>("Scale"_hs)
		.prop(fq::reflect::prop::Name, "Scale")
		.base<Component>();

	// Camera
	entt::meta<Camera>()
		.type("Camera"_hs)
		.prop(fq::reflect::prop::Name, "Camera")
		.prop(fq::reflect::prop::Label, "Rendering")
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

	//////////////////////////////////////////////////////////////////////////
	//                                Mesh                                  //
	//////////////////////////////////////////////////////////////////////////

	// StaticMeshRenderer
	entt::meta<StaticMeshRenderer>()
		.type("StaticMeshRenderer"_hs)
		.prop(fq::reflect::prop::Name, "StaticMeshRenderer")
		.prop(fq::reflect::prop::Label, "Rendering")
		.data<&StaticMeshRenderer::SetModelPath, &StaticMeshRenderer::GetModelPath>("ModelPath"_hs)
		.prop(fq::reflect::prop::Name, "ModelPath")
		.prop(fq::reflect::prop::RelativePath)
		.data<&StaticMeshRenderer::SetMeshName, &StaticMeshRenderer::GetMeshName>("MeshName"_hs)
		.prop(fq::reflect::prop::Name, "MeshName")
		.data<&StaticMeshRenderer::SetMaterials, &StaticMeshRenderer::GetMaterials>("Materials"_hs)
		.prop(fq::reflect::prop::Name, "Materials")
		.prop(fq::reflect::prop::ReadOnly)
		.data<&StaticMeshRenderer::SetOutlineColor, &StaticMeshRenderer::GetOutlineColor>("Outline"_hs)
		.prop(fq::reflect::prop::Name, "Outline")
		.data<&StaticMeshRenderer::SetIsNavigationMeshUsed, &StaticMeshRenderer::GetIsNavigationMeshUsed>("isUsedNavigationMesh"_hs)
		.prop(fq::reflect::prop::Name, "isUsedNavigationMesh")
		.base<Component>();

	// SkinnedMeshRenderer
	entt::meta<SkinnedMeshRenderer>()
		.type("SkinnedMeshRenderer"_hs)
		.prop(fq::reflect::prop::Name, "SkinnedMeshRenderer")
		.prop(fq::reflect::prop::Label, "Rendering")
		.data<&SkinnedMeshRenderer::SetModelPath, &SkinnedMeshRenderer::GetModelPath>("ModelPath"_hs)
		.prop(fq::reflect::prop::Name, "ModelPath")
		.prop(fq::reflect::prop::RelativePath)
		.data<&SkinnedMeshRenderer::SetMeshName, &SkinnedMeshRenderer::GetMeshName>("MeshName"_hs)
		.prop(fq::reflect::prop::Name, "MeshName")
		.data<&SkinnedMeshRenderer::SetMaterials, &SkinnedMeshRenderer::GetMaterials>("Materials"_hs)
		.prop(fq::reflect::prop::Name, "Materials")
		.prop(fq::reflect::prop::ReadOnly)
		.data<&SkinnedMeshRenderer::SetOutlineColor, &SkinnedMeshRenderer::GetOutlineColor>("Outline"_hs)
		.prop(fq::reflect::prop::Name, "Outline")
		.base<Component>();

	//////////////////////////////////////////////////////////////////////////
	//						         UI		                                //
	//////////////////////////////////////////////////////////////////////////

	entt::meta<graphics::UIInfo>()
		.type("UIInfomation"_hs)
		.prop(fq::reflect::prop::Name, "UIInfomation")
		.prop(fq::reflect::prop::POD)
		.data<&graphics::UIInfo::StartX>("StartX"_hs)
		.prop(fq::reflect::prop::Name, "StartX")
		.data<&graphics::UIInfo::StartY>("StartY"_hs)
		.prop(fq::reflect::prop::Name, "StartY")
		.data<&graphics::UIInfo::Width>("Width"_hs)
		.prop(fq::reflect::prop::Name, "Width")
		.data<&graphics::UIInfo::Height>("Height"_hs)
		.prop(fq::reflect::prop::Name, "Height")
		.data<&graphics::UIInfo::Alpha>("Alpha"_hs)
		.prop(fq::reflect::prop::Name, "Alpha")
		.data<&graphics::UIInfo::Layer>("Layer"_hs)
		.prop(fq::reflect::prop::Name, "Layer")
		.data<&graphics::UIInfo::XRatio>("XRatio"_hs)
		.prop(fq::reflect::prop::Name, "XRatio")
		.data<&graphics::UIInfo::YRatio>("YRatio"_hs)
		.prop(fq::reflect::prop::Name, "YRatio")
		.data<&graphics::UIInfo::ImagePath>("ImagePath"_hs)
		.prop(fq::reflect::prop::Name, "ImagePath")
		.prop(fq::reflect::prop::RelativePath)
		.prop(fq::reflect::prop::DragDrop, ".png/.jpg/.dds")
		.data<&graphics::UIInfo::RotationAngle>("RotationAngle"_hs)
		.prop(fq::reflect::prop::Name, "RotationAngle")
		.data<&graphics::UIInfo::ScaleX>("ScaleX"_hs)
		.prop(fq::reflect::prop::Name, "ScaleX")
		.data<&graphics::UIInfo::ScaleY>("ScaleY"_hs)
		.prop(fq::reflect::prop::Name, "ScaleY");

	entt::meta<ImageUI>()
		.type("ImageUI"_hs)
		.prop(fq::reflect::prop::Name, "ImageUI")
		.prop(fq::reflect::prop::Label, "UI")
		.data<&ImageUI::setUIInfomations, &ImageUI::GetUIInfomations>("UIInfomations"_hs)
		.prop(fq::reflect::prop::Name, "UIInfomations")
		.base<Component>();

	//////////////////////////////////////////////////////////////////////////
	//                               Light                                  //
	//////////////////////////////////////////////////////////////////////////

	// LightType
	entt::meta<fq::graphics::ELightType>()
		.prop(fq::reflect::prop::Name, "LightType")
		.data<fq::graphics::ELightType::Directional>("Directional"_hs)
		.prop(fq::reflect::prop::Name, "Directional")
		.data<fq::graphics::ELightType::Point>("Point"_hs)
		.prop(fq::reflect::prop::Name, "Point")
		.data<fq::graphics::ELightType::Spot>("Spot"_hs)
		.prop(fq::reflect::prop::Name, "Spot");

	// Light 
	entt::meta<Light>()
		.type("Light"_hs)
		.prop(fq::reflect::prop::Name, "Light")
		.prop(fq::reflect::prop::Label, "Rendering")
		.data<&Light::SetLightType, &Light::GetLightType>("Type"_hs)
		.prop(fq::reflect::prop::Name, "Type")
		.data<&Light::SetLightColor, &Light::GetLightColor>("Color"_hs)
		.prop(fq::reflect::prop::Name, "Color")
		.data<&Light::SetIntensity, &Light::GetIntensity>("Intensity"_hs)
		.prop(fq::reflect::prop::Name, "Intensity")
		.data<&Light::SetAttenuation, &Light::GetAttenuation>("Attenuation"_hs)
		.prop(fq::reflect::prop::Name, "Attenuation")
		.prop(fq::reflect::prop::Comment, u8"���� ��� (Point, Spot)")
		.data<&Light::SetRange, &Light::GetRange>("Range"_hs)
		.prop(fq::reflect::prop::Name, "Range")
		.prop(fq::reflect::prop::Comment, u8"�Ÿ� (Point, Spot)")
		.data<&Light::SetSpot, &Light::GetSpot>("Spot"_hs)
		.prop(fq::reflect::prop::Name, "Spot")
		.prop(fq::reflect::prop::Comment, u8"Spot ���� (Spot)")
		.data<&Light::SetShadow, &Light::OnShadow>("OnShadow"_hs)
		.prop(fq::reflect::prop::Name, "OnShadow")
		.prop(fq::reflect::prop::Comment, u8"Directional Light 3�� ����")
		.base<Component>();

	//////////////////////////////////////////////////////////////////////////
	//                               Terrain                                //
	//////////////////////////////////////////////////////////////////////////

	// TerrainLayer
	entt::meta<fq::graphics::TerrainLayer>()
		.type("TerrainLayer"_hs)
		.prop(fq::reflect::prop::Name, "TerrainLayer")
		.prop(fq::reflect::prop::POD)
		.data<&fq::graphics::TerrainLayer::BaseColor>("BaseColor"_hs)
		.prop(fq::reflect::prop::Name, "BaseColor")
		.prop(fq::reflect::prop::RelativePath)
		.prop(fq::reflect::prop::DragDrop, ".png/.jpg/.dds")
		.data<&fq::graphics::TerrainLayer::NormalMap>("NormalMap"_hs)
		.prop(fq::reflect::prop::Name, "NormalMap")
		.prop(fq::reflect::prop::RelativePath)
		.prop(fq::reflect::prop::DragDrop, ".png/.jpg/.dds")
		.data<&fq::graphics::TerrainLayer::Metalic>("Metalic"_hs)
		.prop(fq::reflect::prop::Name, "Metalic")
		.data<&fq::graphics::TerrainLayer::Roughness>("Roughness"_hs)
		.prop(fq::reflect::prop::Name, "Roughness")
		.data<&fq::graphics::TerrainLayer::TileSizeX>("TileSizeX"_hs)
		.prop(fq::reflect::prop::Name, "TileSizeX")
		.data<&fq::graphics::TerrainLayer::TileSizeY>("TileSizeY"_hs)
		.prop(fq::reflect::prop::Name, "TileSizeY")
		.data<&fq::graphics::TerrainLayer::TileOffsetX>("TileOffsetX"_hs)
		.prop(fq::reflect::prop::Name, "TileOffsetX")
		.data<&fq::graphics::TerrainLayer::TileOffsetY>("TileOffsetY"_hs)
		.prop(fq::reflect::prop::Name, "TileOffsetY");

	// Terrain
	entt::meta<Terrain>()
		.type("Terrain"_hs)
		.prop(fq::reflect::prop::Name, "Terrain")
		.prop(fq::reflect::prop::Label, "Rendering")
		.data<&Terrain::SetIsUseNavMesh, &Terrain::GetIsUseNavMesh>("IsUseNavMesh"_hs)
		.prop(fq::reflect::prop::Name, "IsUseNavMesh")
		.data<&Terrain::SetWidth, &Terrain::GetWidth>("TerrainWidth"_hs)
		.prop(fq::reflect::prop::Name, "TerrainWidth")
		.data<&Terrain::SetHeight, &Terrain::GetHeight>("TerrainHeight"_hs)
		.prop(fq::reflect::prop::Name, "TerrainHeight")
		.data<&Terrain::SetHeightScale, &Terrain::GetHeightScale>("HeightScale"_hs)
		.prop(fq::reflect::prop::Name, "HeightScale")
		.data<&Terrain::SetHeightMap, &Terrain::GetHeightMap>("HeightMap"_hs)
		.prop(fq::reflect::prop::Name, "HeightMap")
		.prop(fq::reflect::prop::RelativePath)
		.prop(fq::reflect::prop::DragDrop, ".raw")
		.data<&Terrain::SetTextureWidth, &Terrain::GetTextureWidth>("TextureWidth"_hs)
		.prop(fq::reflect::prop::Name, "TextureWidth")
		.data<&Terrain::SetTextureHeight, &Terrain::GetTextureHeight>("TextureHeight"_hs)
		.prop(fq::reflect::prop::Name, "TextureHeight")
		.data<&Terrain::SetAlphaMap, &Terrain::GetAlphaMap>("AlphaMap"_hs)
		.prop(fq::reflect::prop::Name, "AlphaMap")
		.prop(fq::reflect::prop::RelativePath)
		.prop(fq::reflect::prop::DragDrop, ".png/.jpg/.dds")
		.data<&Terrain::SetTerrainLayers, &Terrain::GetTerrainLayers>("Layers"_hs)
		.prop(fq::reflect::prop::Name, "Layers")
		.prop(fq::reflect::prop::Comment, u8"Layer�� 4�� ���� �� �̻� �ʿ��ϴٸ� ��ȭ�� �ʿ���")
		.base<Component>();


	//////////////////////////////////////////////////////////////////////////
	//                              Physics                                 //
	//////////////////////////////////////////////////////////////////////////

	// ColliderType Enum
	entt::meta<fq::physics::EColliderType>()
		.type("ColliderType"_hs)
		.prop(fq::reflect::prop::Name, "ColliderType")
		.data<fq::physics::EColliderType::COLLISION>("Collision"_hs)
		.prop(fq::reflect::prop::Name, "Collision")
		.data<fq::physics::EColliderType::TRIGGER>("Trigger"_hs)
		.prop(fq::reflect::prop::Name, "Trigger");

	// BodyType
	entt::meta<RigidBody::EBodyType>()
		.type("ColliderBodyType"_hs)
		.prop(fq::reflect::prop::Name, "ColliderBodyType")
		.data<RigidBody::EBodyType::Static>("Static"_hs)
		.prop(fq::reflect::prop::Name, "Static")
		.data<RigidBody::EBodyType::Dynamic>("Dynamic"_hs)
		.prop(fq::reflect::prop::Name, "Dynamic")
		.data<RigidBody::EBodyType::Kinematic>("Kinematic"_hs)
		.prop(fq::reflect::prop::Name, "Kinematic");

	// Rigid Body
	entt::meta<RigidBody>()
		.type("RigidBody"_hs)
		.prop(fq::reflect::prop::Name, "RigidBody")
		.prop(fq::reflect::prop::Label, "Physcis")
		.data<&RigidBody::SetBodyType, &RigidBody::GetBodyType>("BodyType"_hs)
		.prop(fq::reflect::prop::Name, "BodyType")
		.data<&RigidBody::SetLinearVelocity, &RigidBody::GetLinearVelocity>("LinearVelocity"_hs)
		.prop(fq::reflect::prop::Name, "LinearVelocity")
		.data<&RigidBody::SetAngularVelocity, &RigidBody::GetAngularVelocity>("AgularVelocity"_hs)
		.prop(fq::reflect::prop::Name, "AgularVelocity")
		.base<Component>();

	// BoxCollider
	entt::meta<BoxCollider>()
		.type("BoxCollider"_hs)
		.prop(fq::reflect::prop::Name, "BoxCollider")
		.prop(fq::reflect::prop::Label, "Physcis")
		.data<&BoxCollider::SetType, &BoxCollider::GetType>("Type"_hs)
		.prop(fq::reflect::prop::Name, "Type")
		.data<&BoxCollider::SetExtent, &BoxCollider::GetExtent>("Extent"_hs)
		.prop(fq::reflect::prop::Name, "Extent")
		.prop(fq::reflect::prop::Comment, u8"�ڽ��� ũ��")
		.data<&BoxCollider::SetOffset, &BoxCollider::GetOffset>("Offset"_hs)
		.prop(fq::reflect::prop::Name, "Offset")
		.data<&BoxCollider::SetStaticFriction, &BoxCollider::GetStaticFriction>("StaticFriction"_hs)
		.prop(fq::reflect::prop::Name, "StaticFriction")
		.data<&BoxCollider::SetDynamicFriction, &BoxCollider::GetDynamicFriction>("DynamicFriction"_hs)
		.prop(fq::reflect::prop::Name, "DynamicFriction")
		.data<&BoxCollider::SetRestituion, &BoxCollider::GetRestituion>("Restitution"_hs)
		.prop(fq::reflect::prop::Name, "Restitution")
		.data<&BoxCollider::SetDensity, &BoxCollider::GetDensity>("Density"_hs)
		.prop(fq::reflect::prop::Name, "Density")
		.base<Component>();

	// SphereCollider
	entt::meta<SphereCollider>()
		.type("SphereCollider"_hs)
		.prop(fq::reflect::prop::Name, "SphereCollider")
		.prop(fq::reflect::prop::Label, "Physcis")
		.data<&SphereCollider::SetType, &SphereCollider::GetType>("Type"_hs)
		.prop(fq::reflect::prop::Name, "Type")
		.data<&SphereCollider::SetRadius, &SphereCollider::GetRadius>("Radius"_hs)
		.prop(fq::reflect::prop::Name, "Radius")
		.data<&SphereCollider::SetOffset, &SphereCollider::GetOffset>("Offset"_hs)
		.prop(fq::reflect::prop::Name, "Offset")
		.data<&SphereCollider::SetStaticFriction, &SphereCollider::GetStaticFriction>("StaticFriction"_hs)
		.prop(fq::reflect::prop::Name, "StaticFriction")
		.data<&SphereCollider::SetDynamicFriction, &SphereCollider::GetDynamicFriction>("DynamicFriction"_hs)
		.prop(fq::reflect::prop::Name, "DynamicFriction")
		.data<&SphereCollider::SetRestituion, &SphereCollider::GetRestituion>("Restitution"_hs)
		.prop(fq::reflect::prop::Name, "Restitution")
		.data<&SphereCollider::SetDensity, &SphereCollider::GetDensity>("Density"_hs)
		.prop(fq::reflect::prop::Name, "Density")
		.base<Component>();

	entt::meta<CapsuleCollider::EDirection>()
		.type("CapsuleDirection"_hs)
		.prop(fq::reflect::prop::Name, "CapsuleDirection")
		.data<CapsuleCollider::EDirection::XAxis>("XAxis"_hs)
		.prop(fq::reflect::prop::Name, "XAxis")
		.data<CapsuleCollider::EDirection::YAxis>("YAxis"_hs)
		.prop(fq::reflect::prop::Name, "YAxis");

	// CapsuleCollider
	entt::meta<CapsuleCollider>()
		.type("CapsuleCollider"_hs)
		.prop(fq::reflect::prop::Name, "CapsuleCollider")
		.prop(fq::reflect::prop::Label, "Physcis")
		.data<&CapsuleCollider::SetType, &CapsuleCollider::GetType>("Type"_hs)
		.prop(fq::reflect::prop::Name, "Type")
		.data<&CapsuleCollider::SetRadius, &CapsuleCollider::GetRadius>("Radius"_hs)
		.prop(fq::reflect::prop::Name, "Radius")
		.data<&CapsuleCollider::SetHalfHegiht, &CapsuleCollider::GetHalfHeight>("HalfHeight"_hs)
		.prop(fq::reflect::prop::Name, "HalfHeight")
		.data<&CapsuleCollider::SetOffset, &CapsuleCollider::GetOffset>("Offset"_hs)
		.prop(fq::reflect::prop::Name, "Offset")
		.data<&CapsuleCollider::SetStaticFriction, &CapsuleCollider::GetStaticFriction>("StaticFriction"_hs)
		.prop(fq::reflect::prop::Name, "StaticFriction")
		.data<&CapsuleCollider::SetDynamicFriction, &CapsuleCollider::GetDynamicFriction>("DynamicFriction"_hs)
		.prop(fq::reflect::prop::Name, "DynamicFriction")
		.data<&CapsuleCollider::SetRestituion, &CapsuleCollider::GetRestituion>("Restitution"_hs)
		.prop(fq::reflect::prop::Name, "Restitution")
		.data<&CapsuleCollider::SetDensity, &CapsuleCollider::GetDensity>("Density"_hs)
		.prop(fq::reflect::prop::Name, "Density")
		.data<&CapsuleCollider::SetDirection, &CapsuleCollider::GetDirection>("Direction"_hs)
		.prop(fq::reflect::prop::Name, "Direction")
		.base<Component>();


	// MeshCollider
	entt::meta<MeshCollider>()
		.type("MeshCollider"_hs)
		.prop(fq::reflect::prop::Name, "MeshCollider")
		.prop(fq::reflect::prop::Label, "Physcis")
		.data<&MeshCollider::SetType, &MeshCollider::GetType>("Type"_hs)
		.prop(fq::reflect::prop::Name, "Type")
		.data<&MeshCollider::SetOffset, &MeshCollider::GetOffset>("Offset"_hs)
		.prop(fq::reflect::prop::Name, "Offset")
		.data<&MeshCollider::SetStaticFriction, &MeshCollider::GetStaticFriction>("StaticFriction"_hs)
		.prop(fq::reflect::prop::Name, "StaticFriction")
		.data<&MeshCollider::SetDynamicFriction, &MeshCollider::GetDynamicFriction>("DynamicFriction"_hs)
		.prop(fq::reflect::prop::Name, "DynamicFriction")
		.data<&MeshCollider::SetRestituion, &MeshCollider::GetRestituion>("Restitution"_hs)
		.prop(fq::reflect::prop::Name, "Restitution")
		.data<&MeshCollider::SetDensity, &MeshCollider::GetDensity>("Density"_hs)
		.prop(fq::reflect::prop::Name, "Density")
		.data<&MeshCollider::SetConvexPolygonLimit, &MeshCollider::GetConvexPolygonLimit>("PolygonLimit"_hs)
		.prop(fq::reflect::prop::Name, "PolygonLimit")
		.base<Component>();


	entt::meta<fq::physics::CharacterControllerInfo>()
		.type("CharacterControllerInfo"_hs)
		.prop(fq::reflect::prop::Name, "CharacterControllerInfo")
		.prop(fq::reflect::prop::POD)
		.data<&fq::physics::CharacterControllerInfo::height>("Height"_hs)
		.prop(fq::reflect::prop::Name, "Height")
		.prop(fq::reflect::prop::Comment, u8"ĳ���� ��Ʈ�ѷ�(ĸ��)�� ����")
		.data<&fq::physics::CharacterControllerInfo::radius>("Radius"_hs)
		.prop(fq::reflect::prop::Name, "Radius")
		.prop(fq::reflect::prop::Comment, u8" ĳ���� ��Ʈ�ѷ�(ĸ��)�� ������")
		.data<&fq::physics::CharacterControllerInfo::stepOffset>("StepOffset"_hs)
		.prop(fq::reflect::prop::Name, "StepOffset")
		.prop(fq::reflect::prop::Comment, u8" ĳ���� ��Ʈ�ѷ��� ������ �� �ִ� ")
		.data<&fq::physics::CharacterControllerInfo::slopeLimit>("SlopeLimit"_hs)
		.prop(fq::reflect::prop::Name, "SlopeLimit")
		.prop(fq::reflect::prop::Comment, u8" ĳ���Ͱ� �ɾ� �ö� �� �ִ� �ִ� ����")
		.data<&fq::physics::CharacterControllerInfo::contactOffset>("ContactOffset"_hs)
		.prop(fq::reflect::prop::Name, "ContactOffset")
		.prop(fq::reflect::prop::Comment, u8" ��Ʈ�ѷ��� ���� ������ : ��ġ ���е� ������ �����ϱ� ���� ����մϴ�.");

	entt::meta<fq::physics::CharacterMovementInfo>()
		.type("CharacterMovementInfo"_hs)
		.prop(fq::reflect::prop::Name, "CharacterMovementInfo")
		.prop(fq::reflect::prop::POD)
		.data<&fq::physics::CharacterMovementInfo::maxSpeed>("MaxSpeed"_hs)
		.prop(fq::reflect::prop::Name, "MaxSpeed")
		.prop(fq::reflect::prop::Comment, u8"�̵� �ִ� �ӵ� : ĳ���Ͱ� ������ �� �ִ� �ִ� �ӵ�")
		.data<&fq::physics::CharacterMovementInfo::acceleration>("Acceleration"_hs)
		.prop(fq::reflect::prop::Name, "Acceleration")
		.prop(fq::reflect::prop::Comment, u8"���ӵ� : ĳ���Ͱ� �Է� ���� ���� �� �̵� ���ӵ�")
		.data<&fq::physics::CharacterMovementInfo::staticFriction>("StaticFriction"_hs)
		.prop(fq::reflect::prop::Name, "StaticFriction")
		.prop(fq::reflect::prop::Comment, u8"���� ���� ��� : ĳ���Ͱ� �̵� �� ������ �� ĳ���Ͱ� �޴� ������ ( 0.0f ~ 1.f )")
		.data<&fq::physics::CharacterMovementInfo::dynamicFriction>("DynamicFriction"_hs)
		.prop(fq::reflect::prop::Name, "DynamicFriction")
		.prop(fq::reflect::prop::Comment, u8"���� ���� ��� : �̵� �߿� ĳ���Ͱ� �޴� ������ ( 0.0f ~ 1.f )")
		.data<&fq::physics::CharacterMovementInfo::jumpSpeed>("JumpSpeed"_hs)
		.prop(fq::reflect::prop::Name, "JumpSpeed")
		.prop(fq::reflect::prop::Comment, u8"����(y��) �ӵ�")
		.data<&fq::physics::CharacterMovementInfo::jumpXZAcceleration>("JumpXZAcceleration"_hs)
		.prop(fq::reflect::prop::Name, "JumpXZAcceleration")
		.prop(fq::reflect::prop::Comment, u8"���� �߿� �̵�(XZ��) ���ӵ� ��")
		.data<&fq::physics::CharacterMovementInfo::jumpXZDeceleration>("JumpXZDeceleration"_hs)
		.prop(fq::reflect::prop::Name, "JumpXZDeceleration")
		.prop(fq::reflect::prop::Comment, u8"���� �߿� �̵�(XZ��) ���� �� ( 0.0 ~ 1.0 )")
		.data<&fq::physics::CharacterMovementInfo::gravityWeight>("GravityWeight"_hs)
		.prop(fq::reflect::prop::Name, "GravityWeight")
		.prop(fq::reflect::prop::Comment, u8"�⺻ �߷� ���� �� �� ������ ����ġ�� �� �ְ� ���� �� ���� �ٸ��� ������ �� �ֽ��ϴ�.");


	// CharaterController
	entt::meta<CharacterController>()
		.type("CharacterController"_hs)
		.prop(fq::reflect::prop::Name, "CharacterController")
		.prop(fq::reflect::prop::Label, "Physcis")
		.data<&CharacterController::SetControllerID, &CharacterController::GetControllerID>("ControllerID"_hs)
		.prop(fq::reflect::prop::Name, "ControllerID")
		.prop(fq::reflect::prop::Comment, u8" 0 ~ 3")
		.data<&CharacterController::SetOffset, &CharacterController::GetOffset>("Offset"_hs)
		.prop(fq::reflect::prop::Name, "Offset")
		.data<&CharacterController::SetMovementInfo, &CharacterController::GetMovementInfo>("MoveInfo"_hs)
		.prop(fq::reflect::prop::Name, "MoveInfo")
		.data<&CharacterController::SetControllerInfo, &CharacterController::GetControllerInfo>("ControllerInfo"_hs)
		.prop(fq::reflect::prop::Name, "ControllerInfo")
		.base<Component>();

	//////////////////////////////////////////////////////////////////////////
	//                              Sound                                   //
	//////////////////////////////////////////////////////////////////////////

	entt::meta<SoundClip>()
		.type("SoundClip"_hs)
		.prop(fq::reflect::prop::Name, "SoundClip")
		.prop(fq::reflect::prop::Label, "Audio")
		.data<&SoundClip::SetSounds, &SoundClip::GetSounds>("Sounds"_hs)
		.prop(fq::reflect::prop::Name, "Sounds")
		.prop(fq::reflect::prop::Comment, u8"�ε��ϴ� ���带 �巡�׵������ �߰��մϴ�")
		.prop(fq::reflect::prop::RelativePath)
		.prop(fq::reflect::prop::DragDrop, ".mp3/.wav")
		.base<Component>();


	//////////////////////////////////////////////////////////////////////////
	//                            Animation                                 //
	//////////////////////////////////////////////////////////////////////////

	entt::meta<AnimationStateNode::Type>()
		.type("AnimationStateNodeType"_hs)
		.prop(fq::reflect::prop::Name, "AnimationStateNodeType")
		.data<AnimationStateNode::Type::Entry>("Entry"_hs)
		.prop(fq::reflect::prop::Name, "Entry")
		.data<AnimationStateNode::Type::AnyState>("AnyState"_hs)
		.prop(fq::reflect::prop::Name, "AnyState")
		.data<AnimationStateNode::Type::Exit>("Exit"_hs)
		.prop(fq::reflect::prop::Name, "Exit")
		.data<AnimationStateNode::Type::State>("State"_hs)
		.prop(fq::reflect::prop::Name, "State");

	entt::meta<AnimationStateNode>()
		.type("AnimationStateNode"_hs)
		.prop(fq::reflect::prop::Name, "AnimationStateNode")
		.data<&AnimationStateNode::SetType, &AnimationStateNode::GetType>("Type"_hs)
		.prop(fq::reflect::prop::Name, "Type")
		.data<&AnimationStateNode::SetModelPath, &AnimationStateNode::GetModelPath>("ModelPath"_hs)
		.prop(fq::reflect::prop::Name, "ModelPath")
		.prop(fq::reflect::prop::RelativePath)
		.data<&AnimationStateNode::SetAnimationName, &AnimationStateNode::GetAnimationName>("AnimationName"_hs)
		.prop(fq::reflect::prop::Name, "AnimationName")
		.data<&AnimationStateNode::SetAnimationKey, &AnimationStateNode::GetAnimationKey>("AnimationKey"_hs)
		.prop(fq::reflect::prop::Name, "AnimationKey")
		.data<&AnimationStateNode::SetPlayBackSpeed, &AnimationStateNode::GetPlayBackSpeed>("PlayBackSpeed"_hs)
		.prop(fq::reflect::prop::Name, "PlayBackSpeed");

	entt::meta<Animator>()
		.type("Animator"_hs)
		.prop(fq::reflect::prop::Name, "Animator")
		.prop(fq::reflect::prop::Label, "Miscellaneous")
		.data<&Animator::SetControllerPath, &Animator::GetControllerPath>("ControllerPath"_hs)
		.prop(fq::reflect::prop::Name, "ControllerPath")
		.prop(fq::reflect::prop::DragDrop, ".controller")
		.prop(fq::reflect::prop::RelativePath)
		.base<Component>();

	entt::meta<LogStateBehaviour>()
		.type("LogStateBehaviour"_hs)
		.prop(fq::reflect::prop::Name, "LogStateBehaviour")
		.data<&LogStateBehaviour::SetExitCount, &LogStateBehaviour::GetExitCount>("ExitCount"_hs)
		.prop(fq::reflect::prop::Name, "ExitCount")
		.data<&LogStateBehaviour::SetEnterCount, &LogStateBehaviour::GetEnterCount>("EnterCount"_hs)
		.prop(fq::reflect::prop::Name, "EnterCount")
		.base<IStateBehaviour>();

	//////////////////////////////////////////////////////////////////////////
	//                            Socket	                                 //
	//////////////////////////////////////////////////////////////////////////

	entt::meta<Socket>()
		.type("Socket"_hs)
		.prop(fq::reflect::prop::Name, "Socket")
		.prop(fq::reflect::prop::Label, "Miscellaneous")
		.data<&Socket::SetBoneName, &Socket::GetBoneName>("BoneName"_hs)
		.prop(fq::reflect::prop::Name, "BoneName")
		.base<Component>();

	//////////////////////////////////////////////////////////////////////////
	//                              Prefab                                  //
	//////////////////////////////////////////////////////////////////////////

	entt::meta<PrefabTest>()
		.type("PrefabTest"_hs)
		.prop(fq::reflect::prop::Name, "PrefabTest")
		.prop(fq::reflect::prop::Label, "Miscellaneous")
		.data<&PrefabTest::SetFireObject, &PrefabTest::GetFireObject>("FireObject"_hs)
		.prop(fq::reflect::prop::Name, "FireObject")
		.prop(fq::reflect::prop::Comment, u8"�������� �߰��ϸ� �ڵ� �߻�!")
		.data<&PrefabTest::SetCreateTime, &PrefabTest::GetCreateTime>("CreateTime"_hs)
		.prop(fq::reflect::prop::Name, "CreateTime")
		.base<Component>();

	//////////////////////////////////////////////////////////////////////////
	//                             Particle                                 //
	//////////////////////////////////////////////////////////////////////////

	{
		using namespace graphics;

		entt::meta<ParticleInfo::EOption>()
			.prop(fq::reflect::prop::Name, "ParticleOption")
			.conv<std::underlying_type_t<ParticleInfo::EOption>>()
			.data<ParticleInfo::EOption::Constant>("Contant"_hs)
			.prop(fq::reflect::prop::Name, "Constant")
			.data<ParticleInfo::EOption::RandomBetweenTwoConstant>("RandomBetweenTwoConstant"_hs)
			.prop(fq::reflect::prop::Name, "RandomBetweenTwoConstant");

		entt::meta<ParticleInfo::Shape::EShape>()
			.prop(fq::reflect::prop::Name, "ParticleShapeType")
			.conv<std::underlying_type_t<ParticleInfo::Shape::EShape>>()
			.data<ParticleInfo::Shape::EShape::Sphere>("Sphere"_hs)
			.prop(fq::reflect::prop::Name, "Sphere")
			.data<ParticleInfo::Shape::EShape::Hemisphere>("Hemisphere"_hs)
			.prop(fq::reflect::prop::Name, "Hemisphere")
			.data<ParticleInfo::Shape::EShape::Cone>("Cone"_hs)
			.prop(fq::reflect::prop::Name, "Cone")
			.data<ParticleInfo::Shape::EShape::Donut>("Donut"_hs)
			.prop(fq::reflect::prop::Name, "Donut")
			.data<ParticleInfo::Shape::EShape::Box>("Box"_hs)
			.prop(fq::reflect::prop::Name, "Box")
			.data<ParticleInfo::Shape::EShape::Circle>("Circle"_hs)
			.prop(fq::reflect::prop::Name, "Circle")
			.data<ParticleInfo::Shape::EShape::Rectangle>("Rectangle"_hs)
			.prop(fq::reflect::prop::Name, "Rectangle");

		entt::meta<ParticleInfo::Shape::EMode>()
			.prop(fq::reflect::prop::Name, "ParticleShapeMode")
			.conv<std::underlying_type_t<ParticleInfo::Shape::EMode>>()
			.data<ParticleInfo::Shape::EMode::Random>("Random"_hs)
			.prop(fq::reflect::prop::Name, "Random");

		entt::meta<ParticleInfo::Main>()
			.type("ParticleMainInfo"_hs)
			.prop(fq::reflect::prop::Name, "ParticleMainInfo")
			.prop(fq::reflect::prop::POD)
			.data<&ParticleInfo::Main::Duration>("Duration"_hs)
			.prop(fq::reflect::prop::Name, "Duration")
			.prop(fq::reflect::prop::Comment, u8"�� ����ð�")
			.data<&ParticleInfo::Main::bIsLooping>("IsLooping"_hs)
			.prop(fq::reflect::prop::Name, "IsLooping")

			.data<&ParticleInfo::Main::StartDelayOption>("StartDelayOption"_hs)
			.prop(fq::reflect::prop::Name, "StartDelayOption")
			.data<&ParticleInfo::Main::StartDelay>("StartDelay"_hs)
			.prop(fq::reflect::prop::Name, "StartDelay")
			.prop(fq::reflect::prop::Comment, u8"���� ���� �ð�")

			.data<&ParticleInfo::Main::StartLifeTimeOption>("StartLifeTimeOption"_hs)
			.prop(fq::reflect::prop::Name, "StartLifeTimeOption")
			.data<&ParticleInfo::Main::StartLifeTime>("StartLifeTime"_hs)
			.prop(fq::reflect::prop::Name, "StartLifeTime")
			.prop(fq::reflect::prop::Comment, u8"��ƼŬ ���� �ð�")

			.data<&ParticleInfo::Main::StartSpeedOption>("StartSpeedOption"_hs)
			.prop(fq::reflect::prop::Name, "StartSpeedOption")
			.data<&ParticleInfo::Main::StartSpeed>("StartSpeed"_hs)
			.prop(fq::reflect::prop::Name, "StartSpeed")
			.prop(fq::reflect::prop::Comment, u8"���� �ӵ�")

			.data<&ParticleInfo::Main::StartSizeOption>("StartSizeOption"_hs)
			.prop(fq::reflect::prop::Name, "StartSizeOption")
			.data<&ParticleInfo::Main::StartSize>("StartSize"_hs)
			.prop(fq::reflect::prop::Name, "StartSize")
			.prop(fq::reflect::prop::Comment, u8"��ƼŬ ũ��")

			.data<&ParticleInfo::Main::StartRotationOption>("StartRotationOption"_hs)
			.prop(fq::reflect::prop::Name, "StartRotationOption")
			.data<&ParticleInfo::Main::StartRotation>("StartRotation"_hs)
			.prop(fq::reflect::prop::Name, "StartRotation")
			.prop(fq::reflect::prop::Comment, u8"��ƼŬ ȸ��")

			.data<&ParticleInfo::Main::StartColorOption>("StartColorOption"_hs)
			.prop(fq::reflect::prop::Name, "StartColorOption")
			.data<&ParticleInfo::Main::StartColor0>("StartColor0"_hs)
			.prop(fq::reflect::prop::Name, "StartColor0")
			.prop(fq::reflect::prop::Comment, u8"��ƼŬ ����")
			.data<&ParticleInfo::Main::StartColor1>("StartColor1"_hs)
			.prop(fq::reflect::prop::Name, "StartColor1")
			.prop(fq::reflect::prop::Comment, u8"��ƼŬ ����")

			.data<&ParticleInfo::Main::GravityModifierOption>("GravityModifierOption"_hs)
			.prop(fq::reflect::prop::Name, "GravityModifierOption")
			.data<&ParticleInfo::Main::GravityModifier>("GravityModifier"_hs)
			.prop(fq::reflect::prop::Name, "GravityModifier")
			.prop(fq::reflect::prop::Comment, u8"��ƼŬ �߷�")

			.data<&ParticleInfo::Main::SimulationSpeed>("SimulationSpeed"_hs)
			.prop(fq::reflect::prop::Name, "SimulationSpeed")

			.data<&ParticleInfo::Main::MaxParticleCount>("MaxParticleCount"_hs)
			.prop(fq::reflect::prop::Name, "MaxParticleCount")
			.prop(fq::reflect::prop::Comment, u8"�ִ� 1024");


		entt::meta<ParticleInfo::Shape>()
			.type("ShapeInfo"_hs)
			.prop(fq::reflect::prop::Name, "ShapeInfo")
			.prop(fq::reflect::prop::POD)
			.data<&ParticleInfo::Shape::ShapeType>("ShapeType"_hs)
			.prop(fq::reflect::prop::Name, "ShapeType")
			.prop(fq::reflect::prop::Comment, u8"���� ���")
			.data<&ParticleInfo::Shape::ModeType>("ModeType"_hs)
			.prop(fq::reflect::prop::Name, "ModeType")
			.prop(fq::reflect::prop::Comment, u8"���� ���")
			.data<&ParticleInfo::Shape::AngleInDegree>("AngleInDegree"_hs)
			.prop(fq::reflect::prop::Name, "AngleInDegree")
			.prop(fq::reflect::prop::Comment, u8"���� ����")
			.data<&ParticleInfo::Shape::Radius>("Radius"_hs)
			.prop(fq::reflect::prop::Name, "Radius")
			.prop(fq::reflect::prop::Comment, u8"������")
			.data<&ParticleInfo::Shape::DountRadius>("DountRadius"_hs)
			.prop(fq::reflect::prop::Name, "DountRadius")
			.prop(fq::reflect::prop::Comment, u8"���� ������")
			.data<&ParticleInfo::Shape::ArcInDegree>("ArcInDegree"_hs)
			.prop(fq::reflect::prop::Name, "ArcInDegree")
			.prop(fq::reflect::prop::Comment, u8"ȣ ũ��")
			.data<&ParticleInfo::Shape::RadiusThickness>("RadiusThickness"_hs)
			.prop(fq::reflect::prop::Name, "RadiusThickness")
			.prop(fq::reflect::prop::Comment, u8" �Թ�ü�� ��� �κ� ���� ����� ������ 0 ~ 1f")
			.data<&ParticleInfo::Shape::Speed>("Speed"_hs)
			.prop(fq::reflect::prop::Name, "Speed")
			.data<&ParticleInfo::Shape::Spread>("Spread"_hs)
			.prop(fq::reflect::prop::Name, "Spread")
			.data<&ParticleInfo::Shape::Position>("Position"_hs)
			.prop(fq::reflect::prop::Name, "Position")
			.data<&ParticleInfo::Shape::Rotation>("Rotation"_hs)
			.prop(fq::reflect::prop::Name, "Rotation")
			.data<&ParticleInfo::Shape::Scale>("Scale"_hs)
			.prop(fq::reflect::prop::Name, "Scale");

		entt::meta<ParticleInfo::Emission::Burst>()
			.type("EmissionBurst"_hs)
			.prop(fq::reflect::prop::Name, "EmissionBurst")
			.prop(fq::reflect::prop::POD)
			.data<&ParticleInfo::Emission::Burst::TimePos>("TimePos"_hs)
			.prop(fq::reflect::prop::Name, "TimePos")
			.prop(fq::reflect::prop::Comment, u8" ó���� �ð�")
			.data<&ParticleInfo::Emission::Burst::Count>("Count"_hs)
			.prop(fq::reflect::prop::Name, "Count")
			.prop(fq::reflect::prop::Comment, u8" ����� ��ƼŬ ��")
			.data<&ParticleInfo::Emission::Burst::Cycles>("Cycles"_hs)
			.prop(fq::reflect::prop::Name, "Cycles")
			.prop(fq::reflect::prop::Comment, u8"  ����Ʈ �ݺ� Ƚ��")
			.data<&ParticleInfo::Emission::Burst::Interval>("Interval"_hs)
			.prop(fq::reflect::prop::Name, "Interval")
			.prop(fq::reflect::prop::Comment, u8"����Ʈ ���� ����")
			.data<&ParticleInfo::Emission::Burst::Probability>("Probability"_hs)
			.prop(fq::reflect::prop::Name, "Probability")
			.prop(fq::reflect::prop::Comment, u8"0 ~ 1 ������ Ȯ�� ���� ���");

		entt::meta<ParticleInfo::VelocityOverLifetime>()
			.type("VelocityOverLifetime"_hs)
			.prop(fq::reflect::prop::Name, "VelocityOverLifetime")
			.prop(fq::reflect::prop::POD)
			.data<&ParticleInfo::VelocityOverLifetime::Velocity>("Velocity"_hs)
			.prop(fq::reflect::prop::Name, "Velocity")
			.data<&ParticleInfo::VelocityOverLifetime::Orbital>("Orbital"_hs)
			.prop(fq::reflect::prop::Name, "Orbital")
			.data<&ParticleInfo::VelocityOverLifetime::Offset>("Offset"_hs)
			.prop(fq::reflect::prop::Name, "Offset")
			.data<&ParticleInfo::VelocityOverLifetime::bIsUsed>("IsUsed"_hs)
			.prop(fq::reflect::prop::Name, "IsUsed");

		entt::meta<ParticleInfo::LimitVelocityOverLifetime>()
			.type("LimitVelocityOverLifetime"_hs)
			.prop(fq::reflect::prop::Name, "LimitVelocityOverLifetime")
			.prop(fq::reflect::prop::POD)
			.data<&ParticleInfo::LimitVelocityOverLifetime::Speed>("Speed"_hs)
			.prop(fq::reflect::prop::Name, "Speed")
			.data<&ParticleInfo::LimitVelocityOverLifetime::Dampen>("Dampen"_hs)
			.prop(fq::reflect::prop::Name, "Dampen")
			.prop(fq::reflect::prop::Comment, u8"���� �ӵ� �ʰ� �� ���� ����")
			.data<&ParticleInfo::LimitVelocityOverLifetime::bIsUsed>("IsUsed"_hs)
			.prop(fq::reflect::prop::Name, "IsUsed");

		entt::meta<ParticleInfo::ForceOverLifetime>()
			.type("ForceOverLifetime"_hs)
			.prop(fq::reflect::prop::Name, "ForceOverLifetime")
			.prop(fq::reflect::prop::POD)
			.data<&ParticleInfo::ForceOverLifetime::Force>("Force"_hs)
			.prop(fq::reflect::prop::Name, "Force")
			.data<&ParticleInfo::ForceOverLifetime::bIsUsed>("IsUsed"_hs)
			.prop(fq::reflect::prop::Name, "IsUsed");

		entt::meta<ParticleInfo::SizeOverLifetime>()
			.type("SizeOverLifetime"_hs)
			.prop(fq::reflect::prop::Name, "SizeOverLifetime")
			.prop(fq::reflect::prop::POD)
			.data<&ParticleInfo::SizeOverLifetime::PointA>("PointA"_hs)
			.prop(fq::reflect::prop::Name, "PointA")
			.data<&ParticleInfo::SizeOverLifetime::PointB>("PointB"_hs)
			.prop(fq::reflect::prop::Name, "PointB")
			.data<&ParticleInfo::SizeOverLifetime::PointC>("PointC"_hs)
			.prop(fq::reflect::prop::Name, "PointC")
			.data<&ParticleInfo::SizeOverLifetime::PointD>("PointD"_hs)
			.prop(fq::reflect::prop::Name, "PointD")
			.data<&ParticleInfo::SizeOverLifetime::bIsUsed>("IsUsed"_hs)
			.prop(fq::reflect::prop::Name, "IsUsed");

		entt::meta<ParticleInfo::RotationOverLifetime>()
			.type("RotationOverLifetime"_hs)
			.prop(fq::reflect::prop::Name, "RotationOverLifetime")
			.prop(fq::reflect::prop::POD)
			.data<&ParticleInfo::RotationOverLifetime::AngularVelocityInDegree>("AngularVelocityInDegree"_hs)
			.prop(fq::reflect::prop::Name, "AngularVelocityInDegree")
			.data<&ParticleInfo::RotationOverLifetime::bIsUsed>("IsUsed"_hs)
			.prop(fq::reflect::prop::Name, "IsUsed");

		entt::meta<ParticleInfo::Render::ERenderMode>()
			.prop(fq::reflect::prop::Name, "ParticleRenderMode")
			.conv<std::underlying_type_t<ParticleInfo::Render::ERenderMode>>()
			.data<ParticleInfo::Render::ERenderMode::Billboard>("Billboard"_hs)
			.prop(fq::reflect::prop::Name, "Billboard");

		entt::meta<ParticleInfo::Render::EBlendMode>()
			.prop(fq::reflect::prop::Name, "ParticleBlendMode")
			.conv<std::underlying_type_t<ParticleInfo::Render::EBlendMode>>()
			.data<ParticleInfo::Render::EBlendMode::Additive>("Additive"_hs)
			.prop(fq::reflect::prop::Name, "Additive")
			.data<ParticleInfo::Render::EBlendMode::Subtractive>("Subtractive"_hs)
			.prop(fq::reflect::prop::Name, "Subtractive")
			.data<ParticleInfo::Render::EBlendMode::Moudulate>("Moudulate"_hs)
			.prop(fq::reflect::prop::Name, "Moudulate");

		entt::meta<ParticleInfo::Render>()
			.type("ParticleRenderData"_hs)
			.prop(fq::reflect::prop::Name, "ParticleRenderData")
			.prop(fq::reflect::prop::POD)
			.data<&ParticleInfo::Render::RenderMode>("RenderMode"_hs)
			.prop(fq::reflect::prop::Name, "RenderMode")
			.data<&ParticleInfo::Render::BlendMode>("BlendMode"_hs)
			.prop(fq::reflect::prop::Name, "BlendMode")
			.data<&ParticleInfo::Render::TexturePath>("TexturePath"_hs)
			.prop(fq::reflect::prop::Name, "TexturePath")
			.prop(fq::reflect::prop::RelativePath)
			.prop(fq::reflect::prop::DragDrop, ".png/.jpg");

		entt::meta<Particle>()
			.type("Particle"_hs)
			.prop(fq::reflect::prop::Name, "Particle")
			.prop(fq::reflect::prop::Label, "Miscellaneous")
			.data<&Particle::SetMainParticleInfo, &Particle::GetMainParticleInfo>("ParticleMainInfo"_hs)
			.prop(fq::reflect::prop::Name, "ParticleMainInfo")
			.data<&Particle::SetShapeInfo, &Particle::GetShapeInfo>("ShapeInfo"_hs)
			.prop(fq::reflect::prop::Name, "ShapeInfo")
			.data<&Particle::SetParticlesPerSecond, &Particle::GetParticlesPerSecond>("ParticlesPerSecond"_hs)
			.prop(fq::reflect::prop::Name, "ParticlesPerSecond")
			.data<&Particle::SetEmissionBursts, &Particle::GetEmissionBursts>("EmissionBursts"_hs)
			.prop(fq::reflect::prop::Name, "EmissionBursts")
			.data<&Particle::SetVelocityOverLifetimeData, &Particle::GetVelocityOverLifetimeData>("VelocityOverLifetime"_hs)
			.prop(fq::reflect::prop::Name, "VelocityOverLifetime")
			.data<&Particle::SetLimitVelocityOverLifeTimeData, &Particle::GetLimitVelocityOverLifeTimeData>("LimitVelocityOverLifetime"_hs)
			.prop(fq::reflect::prop::Name, "LimitVelocityOverLifetime")
			.data<&Particle::SetForceOverLifetimeData, &Particle::GetForceOverLifetimeData>("ForceOverLifetime"_hs)
			.prop(fq::reflect::prop::Name, "ForceOverLifetime")
			.data<&Particle::SetSizeOverLifetime, &Particle::GetSizeOverLifetime>("SizeOverLifetime"_hs)
			.prop(fq::reflect::prop::Name, "SizeOverLifetime")
			.data<&Particle::SetRotationOverLifetimeData, &Particle::GetRotationOverLifetimeData>("RotationOverLifetimeData"_hs)
			.prop(fq::reflect::prop::Name, "RotationOverLifetimeData")
			.data<&Particle::SetRenderData, &Particle::GetRenderData>("ParticleRenderData"_hs)
			.prop(fq::reflect::prop::Name, "ParticleRenderData")
			.base<Component>();

		entt::meta<fq::common::Material>()
			.type("Material"_hs)
			.prop(fq::reflect::prop::Name, "Material")
			.prop(fq::reflect::prop::POD)
			.data<&fq::common::Material::BaseColor>("BaseColor"_hs)
			.prop(fq::reflect::prop::Name, "BaseColor")
			.data<&fq::common::Material::Metalness>("Metalness"_hs)
			.prop(fq::reflect::prop::Name, "Metalness")
			.data<&fq::common::Material::Roughness>("Roughness"_hs)
			.prop(fq::reflect::prop::Name, "Roughness")
			.data<&fq::common::Material::BaseColorFileName>("BaseColorFileName"_hs)
			.prop(fq::reflect::prop::Name, "BaseColorFileName")
			.prop(fq::reflect::prop::RelativePath)
			.prop(fq::reflect::prop::DragDrop, ".png/.jpg")
			.data<&fq::common::Material::MetalnessFileName>("MetalnessFileName"_hs)
			.prop(fq::reflect::prop::Name, "MetalnessFileName")
			.prop(fq::reflect::prop::RelativePath)
			.prop(fq::reflect::prop::DragDrop, ".png/.jpg")
			.data<&fq::common::Material::RoughnessFileName>("RoughnessFileName"_hs)
			.prop(fq::reflect::prop::Name, "RoughnessFileName")
			.prop(fq::reflect::prop::RelativePath)
			.prop(fq::reflect::prop::DragDrop, ".png/.jpg")
			.data<&fq::common::Material::NormalFileName>("NormalFileName"_hs)
			.prop(fq::reflect::prop::Name, "NormalFileName")
			.prop(fq::reflect::prop::RelativePath)
			.prop(fq::reflect::prop::DragDrop, ".png/.jpg")
			.data<&fq::common::Material::EmissiveFileName>("EmissiveFileName"_hs)
			.prop(fq::reflect::prop::Name, "EmissiveFileName")
			.prop(fq::reflect::prop::RelativePath)
			.prop(fq::reflect::prop::DragDrop, ".png/.jpg")
			.data<&fq::common::Material::OpacityFileName>("OpacityFileName"_hs)
			.prop(fq::reflect::prop::Name, "OpacityFileName")
			.prop(fq::reflect::prop::RelativePath)
			.prop(fq::reflect::prop::DragDrop, ".png/.jpg");

		entt::meta<DecalInfo>()
			.type("DecalInfo"_hs)
			.prop(fq::reflect::prop::Name, "DecalInfo")
			.prop(fq::reflect::prop::POD)
			.data<&DecalInfo::MatrialData>("MaterialData"_hs)
			.prop(fq::reflect::prop::Name, "MaterialData")
			.data<&DecalInfo::TexTransform>("TexTransform"_hs)
			.prop(fq::reflect::prop::Name, "TexTransform")
			.data<&DecalInfo::NormalThresholdInRadian>("NormalThresholdInRadian"_hs)
			.prop(fq::reflect::prop::Name, "NormalThresholdInRadian")
			.data<&DecalInfo::bUseMultiplyAlpha>("bUseMultiplyAlpha"_hs)
			.prop(fq::reflect::prop::Name, "bUseMultiplyAlpha")
			.data<&DecalInfo::bUseAlphaClip>("bUseAlphaClip"_hs)
			.prop(fq::reflect::prop::Name, "bUseAlphaClip")
			.data<&DecalInfo::AlphaClipThreshold>("AlphaClipThreshold"_hs)
			.prop(fq::reflect::prop::Name, "AlphaClipThreshold")
			.data<&DecalInfo::bUseDebugRender>("IsbUsedDebugRender"_hs)
			.prop(fq::reflect::prop::Name, "IsbUsedDebugRender")
			.data<&DecalInfo::DebugRenderColor>("DebugRenderColor"_hs)
			.prop(fq::reflect::prop::Name, "DebugRenderColor");

		entt::meta<Decal>()
			.type("Decal"_hs)
			.prop(fq::reflect::prop::Name, "Decal")
			.prop(fq::reflect::prop::Label, "Miscellaneous")
			.data<&Decal::SetDecalInfo, &Decal::GetDecalInfo>("DecalInfo"_hs)
			.prop(fq::reflect::prop::Name, "DecalInfo")
			.base<Component>();
	}

	//////////////////////////////////////////////////////////////////////////
	//                             ��ã��									//
	//////////////////////////////////////////////////////////////////////////
	entt::meta<NavigationAgent>()
		.type("NavigationAgent"_hs)
		.prop(fq::reflect::prop::Name, "NavigationAgent")
		.base<fq::game_module::Component>();

	entt::meta<NavigationMeshLoader>()
		.type("NavigationMeshLoader"_hs)
		.prop(fq::reflect::prop::Name, "NavigationMeshLoader")
		.data<&NavigationMeshLoader::SetNavigationMeshPath, &NavigationMeshLoader::GetNavigationMeshPath>("NavigationMeshPath"_hs)
		.prop(fq::reflect::prop::RelativePath)
		.prop(fq::reflect::prop::Name, "NavigationMeshPath")
		.prop(fq::reflect::prop::DragDrop, ".Nav")
		.base<fq::game_module::Component>();
}
