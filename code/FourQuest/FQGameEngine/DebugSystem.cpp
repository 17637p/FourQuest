#include "DebugSystem.h"

#include "../FQGraphics/IFQGraphics.h"
#include "GameProcess.h"
#include "WindowSystem.h"

fq::game_engine::DebugSystem::DebugSystem()
	:mGameProcess(nullptr)
	, mbOnGrid(true)
	, mbOnLight(true)
	, mScene(nullptr)
{}

fq::game_engine::DebugSystem::~DebugSystem()
{}

void fq::game_engine::DebugSystem::Initialize(GameProcess* game)
{
	using namespace DirectX::SimpleMath;

	mGameProcess = game;
	mScene = mGameProcess->mSceneManager->GetCurrentScene();

	//////////////////////////////////////////////////////////////////////////
	//						    그리드 설정									//
	//////////////////////////////////////////////////////////////////////////
	mGridInfo.Color = { 0.8f,0.8f,0.8f,1.f };
	mGridInfo.GridSize = 10000.f;
	mGridInfo.XDivision = 100;
	mGridInfo.YDivision = 100;
	mGridInfo.Origin = Vector3::Zero;
	mGridInfo.XAxis = Vector3::UnitX;
	mGridInfo.YAxis = Vector3::UnitZ;
}

void fq::game_engine::DebugSystem::Render()
{
	renderGrid();
	renderBoxCollider();
	renderSphereCollider();
	renderCapsuleCollider();
}

void fq::game_engine::DebugSystem::renderGrid()
{
	if (mbOnGrid) mGameProcess->mGraphics->DrawGrid(mGridInfo);
}

void fq::game_engine::DebugSystem::SetGrid(fq::graphics::debug::GridInfo info)
{
	mGridInfo = info;
}

void fq::game_engine::DebugSystem::RenderPointLight(fq::game_module::Light& light)
{
	auto lightinfo = light.GetLightInfomation();

	fq::graphics::debug::SphereInfo sphere;

	sphere.Color = lightinfo.color;
	sphere.Sphere.Center = lightinfo.position;
	sphere.Sphere.Radius = lightinfo.range;

	mGameProcess->mGraphics->DrawSphere(sphere);
}

void fq::game_engine::DebugSystem::RenderDirLight(fq::game_module::Light& light)
{
	auto lightinfo = light.GetLightInfomation();

	fq::graphics::debug::RayInfo ray;
	ray.Color = lightinfo.color;
	ray.Normalize = false;
	ray.Direction = lightinfo.direction * 100.f;
	ray.Origin = lightinfo.position;

	mGameProcess->mGraphics->DrawRay(ray);

	fq::graphics::debug::SphereInfo sphere;

	sphere.Color = lightinfo.color;
	sphere.Sphere.Center = lightinfo.position;
	sphere.Sphere.Radius = 5.f;

	mGameProcess->mGraphics->DrawSphere(sphere);
}

void fq::game_engine::DebugSystem::RenderSpotLight(fq::game_module::Light& light)
{
	auto lightinfo = light.GetLightInfomation();

	fq::graphics::debug::RayInfo ray;
	ray.Color = lightinfo.color;
	ray.Normalize = false;
	ray.Direction = lightinfo.direction * lightinfo.range;;
	ray.Origin = lightinfo.position;

	mGameProcess->mGraphics->DrawRay(ray);
}


void fq::game_engine::DebugSystem::RenderBoxCollier(fq::game_module::Transform& transform, fq::game_module::BoxCollider& collider)
{
	using DirectX::SimpleMath::Color;

	fq::graphics::debug::OBBInfo obb;

	obb.Color = (collider.GetCollisionCount() == 0) ? Color{0.f,1.f,0.f} : Color{ 1.f,0.f,0.f };

	obb.OBB.Center = transform.GetWorldPosition();
	obb.OBB.Extents = collider.GetExtent();
	obb.OBB.Orientation = transform.GetWorldRotation();

	mGameProcess->mGraphics->DrawOBB(obb);
}

void fq::game_engine::DebugSystem::renderBoxCollider()
{
	using namespace fq::game_module;

	mScene->ViewComponents< Transform, BoxCollider>(
		[this](GameObject& object, Transform& transform, BoxCollider& box)
		{
			RenderBoxCollier(transform, box);
		});
}

void fq::game_engine::DebugSystem::RenderSphereCollier(fq::game_module::Transform& transform, fq::game_module::SphereCollider& collider)
{
	fq::graphics::debug::SphereInfo info;
	info.Color = { 0.f,1.f,0.f,1.f };
	info.Sphere.Center = transform.GetWorldPosition();
	info.Sphere.Radius = collider.GetRadius();
	mGameProcess->mGraphics->DrawSphere(info);
}

void fq::game_engine::DebugSystem::renderSphereCollider()
{
	using namespace fq::game_module;

	mScene->ViewComponents< Transform, SphereCollider>(
		[this](GameObject& object, Transform& transform, SphereCollider& sphere)
		{
			RenderSphereCollier(transform, sphere);
		});
}

void fq::game_engine::DebugSystem::RenderCapsuleCollier(fq::game_module::Transform& transform, fq::game_module::CapsuleCollider& collider)
{
	auto up = transform.GetWorldMatrix().Up();
	up.Normalize();
	auto calpsuleInfo = collider.GetCapsuleInfomation();

	// UpSphere
	fq::graphics::debug::SphereInfo info;
	info.Color = { 0.f,1.f,0.f,1.f };
	info.Sphere.Center = transform.GetWorldPosition() + up * calpsuleInfo.halfHeight;
	info.Sphere.Radius = calpsuleInfo.raidus;
	mGameProcess->mGraphics->DrawSphere(info);

	// DownSphere
	info.Sphere.Center = transform.GetWorldPosition() - up * calpsuleInfo.halfHeight;
	mGameProcess->mGraphics->DrawSphere(info);

	// BodyRay 
	fq::graphics::debug::RayInfo ray;

	ray.Direction = up * calpsuleInfo.halfHeight * 2.f;
	ray.Color = { 0.f,1.f,0.f,1.f };
	ray.Normalize = false;
	auto orgin = info.Sphere.Center;
	auto foward = transform.GetWorldMatrix().Forward();
	foward.Normalize();
	foward *= calpsuleInfo.raidus;

	auto right = transform.GetWorldMatrix().Right();
	right.Normalize();
	right *= calpsuleInfo.raidus;

	ray.Origin = orgin + foward;
	mGameProcess->mGraphics->DrawRay(ray);

	ray.Origin = orgin - foward;
	mGameProcess->mGraphics->DrawRay(ray);

	ray.Origin = orgin + right;
	mGameProcess->mGraphics->DrawRay(ray);

	ray.Origin = orgin - right;
	mGameProcess->mGraphics->DrawRay(ray);


}

void fq::game_engine::DebugSystem::renderCapsuleCollider()
{
	using namespace fq::game_module;

	mScene->ViewComponents< Transform, CapsuleCollider>(
		[this](GameObject& object, Transform& transform, CapsuleCollider& capsule)
		{
			RenderCapsuleCollier(transform, capsule);
		});
}
