#include "DebugSystem.h"

#define NOMINMAX
#include <algorithm>

#include "../FQphysics/IFQPhysics.h"
#include "../FQGraphics/IFQGraphics.h"
#include "GameProcess.h"
#include "WindowSystem.h"
#include "PhysicsSystem.h"
#include "PathFindingSystem.h"
#include "LightProbeSystem.h"

fq::game_engine::DebugSystem::DebugSystem()
	:mGameProcess(nullptr)
	, mbOnGrid(true)
	, mbOnLight(true)
	, mScene(nullptr)
	, mbOnBoxCollider(false)
	, mbOnCapsuleCollider(false)
	, mbOnSphereCollider(false)
	, mbOnConvexMeshCollider(false)
	, mbOnCharaterController(false)
	, mbOnNavigationMesh(false)
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
	mGridInfo.GridSize = 1000.f;
	mGridInfo.XDivision = 1000;
	mGridInfo.YDivision = 1000;
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
	renderConvexMeshCollider();
	renderCharaterController();
	renderNavigationMesh();
	renderLightProbe();
}

void fq::game_engine::DebugSystem::renderGrid()
{
	if (GetOnGrid()) mGameProcess->mGraphics->DrawGrid(mGridInfo);
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
	ray.Direction = lightinfo.direction;
	ray.Origin = lightinfo.position;

	mGameProcess->mGraphics->DrawRay(ray);

	fq::graphics::debug::SphereInfo sphere;

	sphere.Color = lightinfo.color;
	sphere.Sphere.Center = lightinfo.position;
	sphere.Sphere.Radius = 0.1f;

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
	using namespace DirectX::SimpleMath;

	fq::graphics::debug::OBBInfo obb;

	obb.Color = (collider.GetCollisionCount() == 0) ? Color{ 0.f,1.f,0.f } : Color{ 1.f,0.f,0.f };

	auto worldM = transform.GetWorldMatrix();
	worldM._41 = 0.f;
	worldM._42 = 0.f;
	worldM._43 = 0.f;
	auto offset = collider.GetOffset();
	obb.OBB.Center = transform.GetWorldPosition() + Vector3::Transform(offset, worldM);

	auto scale = transform.GetWorldScale();
	obb.OBB.Extents = collider.GetExtent();
	obb.OBB.Extents.x *= scale.x;
	obb.OBB.Extents.y *= scale.y;
	obb.OBB.Extents.z *= scale.z;

	obb.OBB.Orientation = transform.GetWorldRotation();

	mGameProcess->mGraphics->DrawOBB(obb);
}

void fq::game_engine::DebugSystem::renderBoxCollider()
{
	if (!GetOnBoxCollider()) return;

	using namespace fq::game_module;

	mScene->ViewComponents< Transform, BoxCollider>(
		[this](GameObject& object, Transform& transform, BoxCollider& box)
		{
			RenderBoxCollier(transform, box);
		});
}

void fq::game_engine::DebugSystem::RenderSphereCollier(fq::game_module::Transform& transform, fq::game_module::SphereCollider& collider)
{
	using namespace DirectX::SimpleMath;

	fq::graphics::debug::SphereInfo info;
	info.Color = (collider.GetCollisionCount() == 0) ? Color{ 0.f,1.f,0.f } : Color{ 1.f,0.f,0.f };

	auto worldM = transform.GetWorldMatrix();
	worldM._41 = 0.f;
	worldM._42 = 0.f;
	worldM._43 = 0.f;
	auto offset = collider.GetOffset();

	info.Sphere.Center = transform.GetWorldPosition() + Vector3::Transform(offset, worldM);

	auto scale = transform.GetWorldScale();
	float max = std::max(scale.x, std::max(scale.y, scale.z));
	info.Sphere.Radius = collider.GetRadius() * max;
	mGameProcess->mGraphics->DrawSphere(info);
}

void fq::game_engine::DebugSystem::renderSphereCollider()
{
	if (!GetOnSphereCollider()) return;

	using namespace fq::game_module;

	mScene->ViewComponents< Transform, SphereCollider>(
		[this](GameObject& object, Transform& transform, SphereCollider& sphere)
		{
			RenderSphereCollier(transform, sphere);
		});
}

void fq::game_engine::DebugSystem::RenderCapsuleCollier(fq::game_module::Transform& transform, fq::game_module::CapsuleCollider& collider)
{
	using  namespace DirectX::SimpleMath;

	Color color = (collider.GetCollisionCount() == 0) ? Color{ 0.f,1.f,0.f } : Color{ 1.f,0.f,0.f };

	auto calpsuleInfo = collider.GetCapsuleInfomation();
	auto offset = collider.GetOffset();
	auto eDirection = collider.GetDirection();

	if (eDirection == game_module::CapsuleCollider::EDirection::XAxis)
	{
		auto right = transform.GetWorldMatrix().Right();
		right.Normalize();
		auto worldM = transform.GetWorldMatrix();
		worldM._41 = 0.f;
		worldM._42 = 0.f;
		worldM._43 = 0.f;
		auto offset = Vector3::Transform(collider.GetOffset(), worldM);

		// UpSphere
		fq::graphics::debug::SphereInfo info;
		info.Color = color;
		info.Sphere.Center = transform.GetWorldPosition() + right * calpsuleInfo.halfHeight + offset;
		info.Sphere.Radius = calpsuleInfo.raidus;
		mGameProcess->mGraphics->DrawSphere(info);

		// DownSphere
		info.Sphere.Center = transform.GetWorldPosition() - right * calpsuleInfo.halfHeight + offset;
		mGameProcess->mGraphics->DrawSphere(info);

		// BodyRay 
		fq::graphics::debug::RayInfo ray;

		ray.Direction = right * calpsuleInfo.halfHeight * 2.f;
		ray.Color = color;
		ray.Normalize = false;
		auto orgin = info.Sphere.Center;
		auto foward = transform.GetWorldMatrix().Forward();
		foward.Normalize();
		foward *= calpsuleInfo.raidus;

		auto up = transform.GetWorldMatrix().Up();
		up.Normalize();
		up *= calpsuleInfo.raidus;

		ray.Origin = orgin + foward;
		mGameProcess->mGraphics->DrawRay(ray);

		ray.Origin = orgin - foward;
		mGameProcess->mGraphics->DrawRay(ray);

		ray.Origin = orgin + up;
		mGameProcess->mGraphics->DrawRay(ray);

		ray.Origin = orgin - up;
		mGameProcess->mGraphics->DrawRay(ray);
	}
	else if (eDirection == game_module::CapsuleCollider::EDirection::YAxis)
	{
		auto right = transform.GetWorldMatrix().Up();
		right.Normalize();
		auto worldM = transform.GetWorldMatrix();
		worldM._41 = 0.f;
		worldM._42 = 0.f;
		worldM._43 = 0.f;
		auto offset = Vector3::Transform(collider.GetOffset(), worldM);

		// UpSphere
		fq::graphics::debug::SphereInfo info;
		info.Color = color;
		info.Sphere.Center = transform.GetWorldPosition() + right * calpsuleInfo.halfHeight + offset;
		info.Sphere.Radius = calpsuleInfo.raidus;
		mGameProcess->mGraphics->DrawSphere(info);

		// DownSphere
		info.Sphere.Center = transform.GetWorldPosition() - right * calpsuleInfo.halfHeight + offset;
		mGameProcess->mGraphics->DrawSphere(info);

		// BodyRay 
		fq::graphics::debug::RayInfo ray;

		ray.Direction = right * calpsuleInfo.halfHeight * 2.f;
		ray.Color = color;
		ray.Normalize = false;
		auto orgin = info.Sphere.Center;
		auto foward = transform.GetWorldMatrix().Forward();
		foward.Normalize();
		foward *= calpsuleInfo.raidus;

		auto up = transform.GetWorldMatrix().Right();
		up.Normalize();
		up *= calpsuleInfo.raidus;

		ray.Origin = orgin + foward;
		mGameProcess->mGraphics->DrawRay(ray);

		ray.Origin = orgin - foward;
		mGameProcess->mGraphics->DrawRay(ray);

		ray.Origin = orgin + up;
		mGameProcess->mGraphics->DrawRay(ray);

		ray.Origin = orgin - up;
		mGameProcess->mGraphics->DrawRay(ray);
	}
}

void fq::game_engine::DebugSystem::RenderCharaterController(fq::game_module::Transform& transform
	, fq::game_module::CharacterController& cotroller)
{
	using DirectX::SimpleMath::Color;

	Color color = (cotroller.GetCollisionCount() == 0) ? Color{ 0.f,1.f,0.f } : Color{ 1.f,0.f,0.f };

	auto upDir = transform.GetWorldMatrix().Up();
	upDir.Normalize();
	auto controllerInfo = cotroller.GetControllerInfo();
	auto offset = cotroller.GetOffset();

	// UpSphere
	fq::graphics::debug::SphereInfo info;
	info.Color = color;
	info.Sphere.Center = transform.GetWorldPosition() + offset + upDir * controllerInfo.height * 0.5f;
	info.Sphere.Radius = controllerInfo.radius;
	mGameProcess->mGraphics->DrawSphere(info);

	// DownSphere
	info.Sphere.Center = transform.GetWorldPosition() + offset - upDir * controllerInfo.height * 0.5f;
	mGameProcess->mGraphics->DrawSphere(info);

	// BodyRay 
	fq::graphics::debug::RayInfo ray;

	ray.Direction = upDir * controllerInfo.height;
	ray.Color = color;
	ray.Normalize = false;
	auto orgin = info.Sphere.Center;
	auto rightDir = transform.GetWorldMatrix().Right();
	rightDir.Normalize();
	rightDir *= controllerInfo.radius;

	auto fowardDir = transform.GetWorldMatrix().Forward();
	fowardDir.Normalize();
	fowardDir *= controllerInfo.radius;

	ray.Origin = orgin + rightDir;
	mGameProcess->mGraphics->DrawRay(ray);

	ray.Origin = orgin - rightDir;
	mGameProcess->mGraphics->DrawRay(ray);

	ray.Origin = orgin + fowardDir;
	mGameProcess->mGraphics->DrawRay(ray);

	ray.Origin = orgin - fowardDir;
	mGameProcess->mGraphics->DrawRay(ray);
}

void fq::game_engine::DebugSystem::renderCapsuleCollider()
{
	if (!GetOnCapsuleCollider()) return;

	using namespace fq::game_module;

	mScene->ViewComponents< Transform, CapsuleCollider>(
		[this](GameObject& object, Transform& transform, CapsuleCollider& capsule)
		{
			RenderCapsuleCollier(transform, capsule);
		});
}

void fq::game_engine::DebugSystem::renderConvexMeshCollider()
{
	if (!GetOnConvexMeshCollider()) return;

	using DirectX::SimpleMath::Color;
	const auto& convexMeshs = mGameProcess->mPhysics->GetDebugPolygon();

	for (const auto& [id, mesh] : convexMeshs)
	{
		for (const auto& polygon : *mesh)
		{
			auto collider = static_cast<fq::game_module::MeshCollider*>(mGameProcess->mPhysicsSystem->GetCollider(id));
			if (!collider) continue;

			auto count = collider->GetCollisionCount();
			fq::graphics::debug::PolygonInfo info;
			info.Color = (count == 0) ? Color{ 0.f,1.f,0.f } : Color{ 1.f,0.f,0.f };
			info.Points = polygon;
			mGameProcess->mGraphics->DrawPolygon(info);
		}
	}
}

void fq::game_engine::DebugSystem::renderTerrainCollider()
{
	if (!GetOnTerrainCollider()) return;

	using namespace fq::game_module;
}

void fq::game_engine::DebugSystem::renderCharaterController()
{
	if (!GetOnCharaterController()) return;

	using namespace fq::game_module;

	mScene->ViewComponents<Transform, CharacterController>
		([this](GameObject& object, Transform& transform, CharacterController& cotroller)
			{
				RenderCharaterController(transform, cotroller);
			});
}


void fq::game_engine::DebugSystem::renderNavigationMesh()
{
	if (!mbOnNavigationMesh)
	{
		return;
	}

	std::vector<DirectX::SimpleMath::Vector3> navMeshVertices = 
		mGameProcess->mPathFindgingSystem->GetNavMeshVertices();

	if (navMeshVertices.size() == 0)
	{
		spdlog::error("Can't Draw NavMesh, No NavgationMesh was created");
		return;
	}

	for (UINT i = 0; i < navMeshVertices.size(); i++)
	{
		if (i % 3 == 0)
		{
			fq::graphics::debug::PolygonInfo info;

			info.Points.push_back(navMeshVertices[i + 0]);
			info.Points.push_back(navMeshVertices[i + 1]);
			info.Points.push_back(navMeshVertices[i + 2]);

			info.Color = { 1, 1, 0, 1 };

			mGameProcess->mGraphics->DrawPolygon(info);
		}

		fq::graphics::debug::SphereInfo info;

		info.Sphere.Center = navMeshVertices[i];
		info.Sphere.Radius = 0.05f;

		info.Color = { 0, 1, 1, 1 };

		mGameProcess->mGraphics->DrawSphere(info);
	}
}

void fq::game_engine::DebugSystem::renderLightProbe()
{
	std::vector<LightProbe*> lightProbes = mGameProcess->mLightProbeSystem->GetLightProbes();

	for (int i = 0; i < lightProbes.size(); i++)
	{
		fq::graphics::debug::SphereInfo info;

		info.Sphere.Center = lightProbes[i]->position;
		info.Sphere.Radius = 0.05f;

		info.Color = { 1, 1, 0, 1 };

		mGameProcess->mGraphics->DrawSphere(info);
	}
}

