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

// Physics
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
		.prop(fq::reflect::prop::Name, "Wall");

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
		.data<&Transform::SetLocalPosition, &Transform::GetLocalPosition>("Position"_hs)
		.prop(fq::reflect::prop::Name, "Position")
		.prop(fq::reflect::prop::Comment, u8"로컬 좌표")
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

	//////////////////////////////////////////////////////////////////////////
	//                                Mesh                                  //
	//////////////////////////////////////////////////////////////////////////

	// StaticMeshRenderer
	entt::meta<StaticMeshRenderer>()
		.type("StaticMeshRenderer"_hs)
		.prop(fq::reflect::prop::Name, "StaticMeshRenderer")
		.data<&StaticMeshRenderer::SetModelPath, &StaticMeshRenderer::GetModelPath>("ModelPath"_hs)
		.prop(fq::reflect::prop::Name, "ModelPath")
		.prop(fq::reflect::prop::RelativePath)
		.data<&StaticMeshRenderer::SetMeshName, &StaticMeshRenderer::GetMeshName>("MeshName"_hs)
		.prop(fq::reflect::prop::Name, "MeshName")
		.data<&StaticMeshRenderer::SetMaterials, &StaticMeshRenderer::GetMaterials>("Materials"_hs)
		.prop(fq::reflect::prop::Name, "Materials")
		.prop(fq::reflect::prop::ReadOnly)
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
		.prop(fq::reflect::prop::ReadOnly)
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
		.data<&Light::SetLightType, &Light::GetLightType>("Type"_hs)
		.prop(fq::reflect::prop::Name, "Type")
		.data<&Light::SetLightColor, &Light::GetLightColor>("Color"_hs)
		.prop(fq::reflect::prop::Name, "Color")
		.data<&Light::SetIntensity, &Light::GetIntensity>("Intensity"_hs)
		.prop(fq::reflect::prop::Name, "Intensity")
		.data<&Light::SetAttenuation, &Light::GetAttenuation>("Attenuation"_hs)
		.prop(fq::reflect::prop::Name, "Attenuation")
		.prop(fq::reflect::prop::Comment, u8"감쇠 상수 (Point, Spot)")
		.data<&Light::SetRange, &Light::GetRange>("Range"_hs)
		.prop(fq::reflect::prop::Name, "Range")
		.prop(fq::reflect::prop::Comment, u8"거리 (Point, Spot)")
		.data<&Light::SetSpot, &Light::GetSpot>("Spot"_hs)
		.prop(fq::reflect::prop::Name, "Spot")
		.prop(fq::reflect::prop::Comment, u8"Spot 설정 (Spot)")
		.data<&Light::SetShadow, &Light::OnShadow>("OnShadow"_hs)
		.prop(fq::reflect::prop::Name, "OnShadow")
		.prop(fq::reflect::prop::Comment, u8"Directional Light 3개 제한")
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

	// Rigid Body
	entt::meta<RigidBody>()
		.type("RigidBody"_hs)
		.prop(fq::reflect::prop::Name, "RigidBody")
		.data<&RigidBody::SetStatic, &RigidBody::IsStatic>("IsStatic"_hs)
		.prop(fq::reflect::prop::Name, "IsStatic")
		.prop(fq::reflect::prop::Comment, "static or dynamic")
		.data<&RigidBody::SetLinearVelocity, &RigidBody::GetLinearVelocity>("LinearVelocity"_hs)
		.prop(fq::reflect::prop::Name, "LinearVelocity")
		.data<&RigidBody::SetAngularVelocity, &RigidBody::GetAngularVelocity>("AgularVelocity"_hs)
		.prop(fq::reflect::prop::Name, "AgularVelocity")
		.base<Component>();

	// BoxCollider
	entt::meta<BoxCollider>()
		.type("BoxCollider"_hs)
		.prop(fq::reflect::prop::Name, "BoxCollider")
		.data<&BoxCollider::SetType, &BoxCollider::GetType>("Type"_hs)
		.prop(fq::reflect::prop::Name, "Type")
		.data<&BoxCollider::SetExtent, &BoxCollider::GetExtent>("Extent"_hs)
		.prop(fq::reflect::prop::Name, "Extent")
		.prop(fq::reflect::prop::Comment, u8"박스의 크기")
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
		.data<&SphereCollider::SetType, &SphereCollider::GetType>("Type"_hs)
		.prop(fq::reflect::prop::Name, "Type")
		.data<&SphereCollider::SetRadius, &SphereCollider::GetRadius>("Radius"_hs)
		.prop(fq::reflect::prop::Name, "Radius")
		.data<&SphereCollider::SetStaticFriction, &SphereCollider::GetStaticFriction>("StaticFriction"_hs)
		.prop(fq::reflect::prop::Name, "StaticFriction")
		.data<&SphereCollider::SetDynamicFriction, &SphereCollider::GetDynamicFriction>("DynamicFriction"_hs)
		.prop(fq::reflect::prop::Name, "DynamicFriction")
		.data<&SphereCollider::SetRestituion, &SphereCollider::GetRestituion>("Restitution"_hs)
		.prop(fq::reflect::prop::Name, "Restitution")
		.data<&SphereCollider::SetDensity, &SphereCollider::GetDensity>("Density"_hs)
		.prop(fq::reflect::prop::Name, "Density")
		.base<Component>();

	// CapsuleCollider
	entt::meta<CapsuleCollider>()
		.type("CapsuleCollider"_hs)
		.prop(fq::reflect::prop::Name, "CapsuleCollider")
		.data<&CapsuleCollider::SetType, &CapsuleCollider::GetType>("Type"_hs)
		.prop(fq::reflect::prop::Name, "Type")
		.data<&CapsuleCollider::SetRadius, &CapsuleCollider::GetRadius>("Radius"_hs)
		.prop(fq::reflect::prop::Name, "Radius")
		.data<&CapsuleCollider::SetHalfHegiht, &CapsuleCollider::GetHalfHeight>("HalfHeight"_hs)
		.prop(fq::reflect::prop::Name, "HalfHeight")
		.data<&CapsuleCollider::SetStaticFriction, &CapsuleCollider::GetStaticFriction>("StaticFriction"_hs)
		.prop(fq::reflect::prop::Name, "StaticFriction")
		.data<&CapsuleCollider::SetDynamicFriction, &CapsuleCollider::GetDynamicFriction>("DynamicFriction"_hs)
		.prop(fq::reflect::prop::Name, "DynamicFriction")
		.data<&CapsuleCollider::SetRestituion, &CapsuleCollider::GetRestituion>("Restitution"_hs)
		.prop(fq::reflect::prop::Name, "Restitution")
		.data<&CapsuleCollider::SetDensity, &CapsuleCollider::GetDensity>("Density"_hs)
		.prop(fq::reflect::prop::Name, "Density")
		.base<Component>();


	// MeshCollider
	entt::meta<MeshCollider>()
		.type("MeshCollider"_hs)
		.prop(fq::reflect::prop::Name, "MeshCollider")
		.data<&MeshCollider::SetType, &MeshCollider::GetType>("Type"_hs)
		.prop(fq::reflect::prop::Name, "Type")
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
		.prop(fq::reflect::prop::Comment, u8"캐릭터 컨트롤러(캡슐)의 높이")
		.data<&fq::physics::CharacterControllerInfo::radius>("Radius"_hs)
		.prop(fq::reflect::prop::Name, "Radius")
		.prop(fq::reflect::prop::Comment, u8" 캐릭터 컨트롤러(캡슐)의 반지름")
		.data<&fq::physics::CharacterControllerInfo::stepOffset>("StepOffset"_hs)
		.prop(fq::reflect::prop::Name, "StepOffset")
		.prop(fq::reflect::prop::Comment, u8" 캐릭터 컨트롤러가 지나갈 수 있는 ")
		.data<&fq::physics::CharacterControllerInfo::slopeLimit>("SlopeLimit"_hs)
		.prop(fq::reflect::prop::Name, "SlopeLimit")
		.prop(fq::reflect::prop::Comment, u8" 캐릭터가 걸어 올라갈 수 있는 최대 기울기")
		.data<&fq::physics::CharacterControllerInfo::contactOffset>("ContactOffset"_hs)
		.prop(fq::reflect::prop::Name, "ContactOffset")
		.prop(fq::reflect::prop::Comment, u8" 컨트롤러의 접촉 오프셋 : 수치 정밀도 문제를 방지하기 위해 사용합니다.");

	entt::meta<fq::physics::CharacterMovementInfo>()
		.type("CharacterMovementInfo"_hs)
		.prop(fq::reflect::prop::Name, "CharacterMovementInfo")
		.prop(fq::reflect::prop::POD)
		.data<&fq::physics::CharacterMovementInfo::maxSpeed>("MaxSpeed"_hs)
		.prop(fq::reflect::prop::Name, "MaxSpeed")
		.prop(fq::reflect::prop::Comment, u8"이동 최대 속도 : 캐릭터가 움직일 수 있는 최대 속도")
		.data<&fq::physics::CharacterMovementInfo::acceleration>("Acceleration"_hs)
		.prop(fq::reflect::prop::Name, "Acceleration")
		.prop(fq::reflect::prop::Comment, u8"가속도 : 캐릭터가 입력 값을 받을 때 이동 가속도")
		.data<&fq::physics::CharacterMovementInfo::staticFriction>("StaticFriction"_hs)
		.prop(fq::reflect::prop::Name, "StaticFriction")
		.prop(fq::reflect::prop::Comment, u8"정적 마찰 계수 : 캐릭터가 이동 중 멈췄을 때 캐릭터가 받는 마찰력 ( 0.0f ~ 1.f )")
		.data<&fq::physics::CharacterMovementInfo::dynamicFriction>("DynamicFriction"_hs)
		.prop(fq::reflect::prop::Name, "DynamicFriction")
		.prop(fq::reflect::prop::Comment, u8"동적 마찰 계수 : 이동 중에 캐릭터가 받는 마찰력 ( 0.0f ~ 1.f )")
		.data<&fq::physics::CharacterMovementInfo::jumpSpeed>("JumpSpeed"_hs)
		.prop(fq::reflect::prop::Name, "JumpSpeed")
		.prop(fq::reflect::prop::Comment, u8"점프(y축) 속도")
		.data<&fq::physics::CharacterMovementInfo::jumpXZAcceleration>("JumpXZAcceleration"_hs)
		.prop(fq::reflect::prop::Name, "JumpXZAcceleration")
		.prop(fq::reflect::prop::Comment, u8"점프 중에 이동(XZ축) 가속도 값")
		.data<&fq::physics::CharacterMovementInfo::jumpXZDeceleration>("JumpXZDeceleration"_hs)
		.prop(fq::reflect::prop::Name, "JumpXZDeceleration")
		.prop(fq::reflect::prop::Comment, u8"점프 중에 이동(XZ축) 감속 값 ( 0.0 ~ 1.0 )")
		.data<&fq::physics::CharacterMovementInfo::gravityWeight>("GravityWeight"_hs)
		.prop(fq::reflect::prop::Name, "GravityWeight")
		.prop(fq::reflect::prop::Comment, u8"기본 중력 값을 줄 수 있지만 가중치를 더 주고 싶을 때 값을 다르게 세팅할 수 있습니다.");


	// CharaterController
	entt::meta<CharacterController>()
		.type("CharacterController"_hs)
		.prop(fq::reflect::prop::Name, "CharacterController")
		.data<&CharacterController::SetControllerInfo, &CharacterController::GetControllerInfo>("ControllerInfo"_hs)
		.prop(fq::reflect::prop::Name, "ControllerInfo")
		.data<&CharacterController::SetMoventInfo, &CharacterController::GetMoventInfo>("MoveInfo"_hs)
		.prop(fq::reflect::prop::Name, "MoveInfo")
		.base<Component>();


	//////////////////////////////////////////////////////////////////////////
	//                              Sound                                   //
	//////////////////////////////////////////////////////////////////////////

	entt::meta<SoundClip>()
		.type("SoundClip"_hs)
		.prop(fq::reflect::prop::Name, "SoundClip")
		.data<&SoundClip::SetSounds, &SoundClip::GetSounds>("Sounds"_hs)
		.prop(fq::reflect::prop::Name, "Sounds")
		.prop(fq::reflect::prop::Comment, u8"로드하는 사운드를 드래그드랍으로 추가합니다")
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
		.data<&Animator::SetControllerPath, &Animator::GetControllerPath>("ControllerPath"_hs)
		.prop(fq::reflect::prop::Name, "ControllerPath")
		.prop(fq::reflect::prop::DragDrop, ".controller")
		.prop(fq::reflect::prop::RelativePath)
		.base<Component>();


		//////////////////////////////////////////////////////////////////////////
		//                              Prefab                                  //
		//////////////////////////////////////////////////////////////////////////

		entt::meta<PrefabTest>()
		.type("PrefabTest"_hs)
		.prop(fq::reflect::prop::Name, "PrefabTest")
		.data<&PrefabTest::SetFireObject, &PrefabTest::GetFireObject>("FireObject"_hs)
		.prop(fq::reflect::prop::Name, "FireObject")
		.prop(fq::reflect::prop::Comment, u8"프리팹을 추가하면 자동 발사!")
		.data<&PrefabTest::SetCreateTime, &PrefabTest::GetCreateTime>("CreateTime"_hs)
		.prop(fq::reflect::prop::Name, "CreateTime")
		.base<Component>();
}
