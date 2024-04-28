#include "DebugSystem.h"

#include "../FQGraphics/IFQGraphics.h"
#include "GameProcess.h"
#include "WindowSystem.h"

fq::game_engine::DebugSystem::DebugSystem()
	:mGameProcess(nullptr)
	, mbOnGrid(true)
	, mbOnLight(true)
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
	mGridInfo.XDivision = 10;
	mGridInfo.YDivision = 10;
	mGridInfo.Origin = Vector3::Zero;
	mGridInfo.XAxis = Vector3::UnitX;
	mGridInfo.YAxis = Vector3::UnitZ;
}

void fq::game_engine::DebugSystem::Render()
{
	renderGrid();
	renderLight();
	renderCamera();
	renderBoxCollider();
}

void fq::game_engine::DebugSystem::renderGrid()
{
	if (mbOnGrid) mGameProcess->mGraphics->DrawGrid(mGridInfo);
}

void fq::game_engine::DebugSystem::SetGrid(fq::graphics::debug::GridInfo info)
{
	mGridInfo = info;
}

void fq::game_engine::DebugSystem::renderLight()
{
	using namespace fq::game_module;

	mScene->ViewComponents<Light>(
		[this](GameObject& object, Light& light)
		{
			auto type = light.GetLightType();
			if (type == fq::graphics::ELightType::Directional) RenderDirLight(light);
			else if (type == fq::graphics::ELightType::Point) RenderPointLight(light);
			else if (type == fq::graphics::ELightType::Spot) RenderSpotLight(light);
		});
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

void fq::game_engine::DebugSystem::RenderCamera(fq::game_module::Transform& transform
	, fq::game_module::Camera& camera)
{
	fq::graphics::debug::FrustumInfo frustum;

	frustum.Color = DirectX::SimpleMath::Color{ 0.f,0.f,0.f,1.f };

	frustum.Frustum.Near = camera.GetNearPlain();
	frustum.Frustum.Far = camera.GetFarPlain();
	frustum.Frustum.Orientation = transform.GetWorldRotation();
	frustum.Frustum.Origin = transform.GetWorldPosition();

	float halfVerticalFOV = DirectX::XMConvertToRadians( camera.GetFieldOfView())/ 2.f;
	float aspectRatio = mGameProcess->mWindowSystem->GetScreenWidth() / mGameProcess->mWindowSystem->GetScreenHeight();
	float horizontalFOV = 2.f* atanf(tanf(halfVerticalFOV) * aspectRatio);

	frustum.Frustum.TopSlope = tanf(halfVerticalFOV);
	frustum.Frustum.BottomSlope = -frustum.Frustum.TopSlope;
	frustum.Frustum.RightSlope = tanf(horizontalFOV);
	frustum.Frustum.LeftSlope = -frustum.Frustum.RightSlope;

	mGameProcess->mGraphics->DrawFrustum(frustum);
}

void fq::game_engine::DebugSystem::renderCamera()
{
	using namespace fq::game_module;

	mScene->ViewComponents< Transform, Camera>(
		[this](GameObject& object, Transform& transform, Camera& camera)
		{
			if (camera.IsMain())
			{
				RenderCamera(transform, camera);
			}

		});
}

void fq::game_engine::DebugSystem::RenderBoxCollier(fq::game_module::Transform& transform, fq::game_module::BoxCollider& collider)
{
	fq::graphics::debug::OBBInfo obb;
	obb.Color = { 0.f,1.f,0.f,1.f };
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
