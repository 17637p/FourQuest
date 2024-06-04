#include "FQGraphics.h"
#include "D3D11Device.h"
#include "ManagementCommon.h"

// temp - 컬링 테스트할 때 transform 잠깐 쓰려고
#include "../FQCommon/FQCommon.h"

using namespace fq::graphics;

FQGraphics::~FQGraphics()
{
	mRenderManager = nullptr;
}

FQGraphics::FQGraphics()
	: mDevice(std::make_shared<D3D11Device>())
	, mResourceManager(nullptr)
	, mObjectManager(std::make_shared<D3D11ObjectManager>())
	, mJobManager(std::make_shared<D3D11JobManager>())
	, mRenderManager(std::make_shared<D3D11RenderManager>())
	, mCameraManager(std::make_shared<D3D11CameraManager>())
	, mModelManager(std::make_shared<D3D11ModelManager>())
	, mLightManager(std::make_shared<D3D11LightManager>())
	, mDebugDrawManager(std::make_shared<D3D11DebugDrawManager>())
	, mPickingManager(std::make_shared<D3D11PickingManager>())
	, mCullingManager(std::make_shared<D3D11CullingManager>())
	, mParticleManager(std::make_shared<D3D11ParticleManager>())
	, mUIManager(std::make_shared<UIManager>())
{
}

bool fq::graphics::FQGraphics::Initialize(const HWND hWnd, const unsigned short width, const unsigned short height, EPipelineType pipelineType)
{
	Finalize();

	mDevice->Initialize(hWnd, width, height);
	mResourceManager = std::make_shared<D3D11ResourceManager>(mDevice);
	mObjectManager;
	mJobManager;
	mCameraManager->Initialize(width, height);
	mLightManager->Initialize(mDevice);
	mDebugDrawManager->Initialize(mDevice);
	mRenderManager->Initialize(mDevice, mJobManager, mCameraManager, mLightManager, mResourceManager, mDebugDrawManager, mParticleManager, width, height, pipelineType);
	mPickingManager->Initialize(mDevice, mResourceManager, width, height);
	mUIManager->Initialize(hWnd, mDevice, width, height);
	mParticleManager->Initialize(mDevice, mResourceManager, mCameraManager);

	return true;
}

bool fq::graphics::FQGraphics::Update(float deltaTime)
{
	return true;
}

void fq::graphics::FQGraphics::SetSkyBox(const std::wstring& path)
{
	mRenderManager->SetSkyBox(path);
}

void fq::graphics::FQGraphics::SetIBLTexture(const std::wstring& diffuse, const std::wstring& specular, const std::wstring& brdfLUT)
{
	mRenderManager->SetIBLTexture(diffuse, specular, brdfLUT);
}

void FQGraphics::SetTerrainMeshObject(ITerrainMeshObject* meshObject, const TerrainMaterialInfo& material)
{
	mObjectManager->SetTerrainMeshObject(mDevice, meshObject, material);
}

void FQGraphics::DeleteTerrainMeshObject(ITerrainMeshObject* meshObject)
{
	mObjectManager->DeleteTerrainMeshObject(meshObject);
}

fq::graphics::ITerrainMeshObject* FQGraphics::CreateTerrainMeshObject(const MeshObjectInfo& info)
{
	return mObjectManager->CreateTerrainMeshObject(mModelManager, info);
}

void FQGraphics::DrawText(const std::wstring& text, const DirectX::SimpleMath::Rectangle& drawRect, unsigned short fontSize /*= 50*/, const std::wstring& fontPath /*= L"Verdana"*/, const DirectX::SimpleMath::Color& color /*= { 1, 0, 0, 1 }*/)
{
	mUIManager->DrawText(text, drawRect, fontSize, fontPath, color);
}

void FQGraphics::SetDefaultFontSize(const unsigned short fontSize)
{
	mUIManager->SetDefaultFontSize(fontSize);
}

void FQGraphics::SetDefaultFontColor(const DirectX::SimpleMath::Color& color)
{
	mUIManager->SetDefaultFontColor(color);
}

void FQGraphics::SetDefaultFont(const std::wstring& path)
{
	mUIManager->SetDefaultFont(path);
}

void FQGraphics::AddFont(const std::wstring& path)
{
	mUIManager->AddFont(path);
}

void FQGraphics::DeleteFont(const std::wstring& path)
{
	mUIManager->DeleteFont(path);
}

void FQGraphics::UpdateColCamera(const fq::common::Transform& cameraTransform)
{
	mCullingManager->UpdateCameraFrustum(cameraTransform.worldPosition, cameraTransform.worldRotation, mCameraManager->GetProjectionMatrix(ECameraType::Player));
}

void* FQGraphics::GetPickingObject(const short mouseX, const short mouseY)
{
	return mPickingManager->GetPickedObject(mouseX, mouseY, mDevice, mCameraManager, mJobManager, mObjectManager->GetStaticMeshObjects(), mObjectManager->GetSkinnedMeshObjects());
}

std::shared_ptr<spdlog::logger> FQGraphics::SetUpLogger(std::vector<spdlog::sink_ptr> sinks)
{
	auto logger = std::make_shared<spdlog::logger>("Graphics",
		std::begin(sinks),
		std::end(sinks));

	spdlog::set_default_logger(logger);
	return logger;
}


void FQGraphics::UpdateLight(const unsigned int id, const LightInfo& lightInfo)
{
	mLightManager->SetLight(id, lightInfo);
}

void FQGraphics::AddLight(const unsigned int id, const LightInfo& lightInfo)
{
	mLightManager->AddLight(id, lightInfo);
}

void FQGraphics::DeleteLight(const unsigned int id)
{
	mLightManager->DeleteLight(id);
}

void FQGraphics::UseShadow(const unsigned int id, bool bUseShadow)
{
	mLightManager->UseShadow(id, bUseShadow);
}

void FQGraphics::UpdateCamera(const fq::common::Transform& cameraTransform)
{
	mCameraManager->Update(ECameraType::Player, cameraTransform);

	mCullingManager->UpdateCameraFrustum(mCameraManager->GetPosition(ECameraType::Player), mCameraManager->GetRotation(ECameraType::Player), mCameraManager->GetProjectionMatrix(ECameraType::Player));
}

void FQGraphics::SetCamera(const CameraInfo& cameraInfo)
{
	mCameraManager->SetCamera(ECameraType::Player, cameraInfo);
}

bool FQGraphics::BeginRender()
{
	mRenderManager->BeginRender();

	return true;
}

bool FQGraphics::Render()
{
	std::set<IStaticMeshObject*> staticMeshesToRender = mObjectManager->GetStaticMeshObjects();
	std::set<ISkinnedMeshObject*> skinnedMeshesToRender = mObjectManager->GetSkinnedMeshObjects();

	// 컬링 추가해야 됨 
	std::set<ITerrainMeshObject*> terrainMeshesToRender = mObjectManager->GetTerrainMeshObjects();

	staticMeshesToRender = mCullingManager->GetInFrustumStaticObjects(staticMeshesToRender);
	skinnedMeshesToRender = mCullingManager->GetInFrustumSkinnedObjects(skinnedMeshesToRender);

	mJobManager->CreateStaticMeshJobs(staticMeshesToRender);
	mJobManager->CreateSkinnedMeshJobs(skinnedMeshesToRender);
	mJobManager->CreateTerrainMeshJobs(terrainMeshesToRender);

	mRenderManager->Render();
	return true;
}

bool FQGraphics::EndRender()
{
	mUIManager->Render();
	mRenderManager->EndRender();
	mJobManager->ClearAll();

	return true;
}

bool FQGraphics::Finalize()
{
	return true;
}

bool FQGraphics::SetViewportSize(const unsigned short width, const unsigned short height)
{
	mCameraManager->OnResize(width, height);

	return true;
}

bool FQGraphics::SetWindowSize(const unsigned short width, const unsigned short height)
{
	mUIManager->ReleaseRenderTarget();
	mRenderManager->OnResize(width, height);
	mCameraManager->OnResize(width, height);
	mPickingManager->OnResize(width, height, mDevice);
	mUIManager->OnResize(mDevice, width, height);

	return true;
}

const fq::common::Model& FQGraphics::CreateModel(std::string path, std::filesystem::path textureBasePath)
{
	return mModelManager->CreateModel(mDevice, path, textureBasePath);
}

const fq::common::Model& FQGraphics::GetModel(std::string path)
{
	return mModelManager->FindModel(path);
}

void FQGraphics::DeleteModel(std::string path)
{
	mModelManager->DeleteModel(path);
}

void FQGraphics::WriteModel(std::string path, const fq::common::Model& modelData)
{
	mModelManager->WriteModel(path, modelData);
}

void FQGraphics::ConvertModel(std::string fbxFile, std::string fileName)
{
	mModelManager->ConvertModel(fbxFile, fileName);
}

IStaticMeshObject* FQGraphics::CreateStaticMeshObject(MeshObjectInfo info)
{
	return mObjectManager->CreateStaticMeshObject(mModelManager, info);
}

void FQGraphics::AddAnimation(IStaticMeshObject* iStaticMeshObject, AnimationInfo info)
{
	mObjectManager->AddAnimation(mModelManager, iStaticMeshObject, info);
}

void FQGraphics::DeleteStaticMeshObject(IStaticMeshObject* meshObject)
{
	mObjectManager->DeleteStaticMeshObject(meshObject);
}

ISkinnedMeshObject* FQGraphics::CreateSkinnedMeshObject(MeshObjectInfo info)
{
	return mObjectManager->CreateSkinnedMeshObject(mModelManager, info);
}

void FQGraphics::AddAnimation(ISkinnedMeshObject* iSkinnedMeshObject, AnimationInfo info)
{
	mObjectManager->AddAnimation(mModelManager, iSkinnedMeshObject, info);
}

void FQGraphics::DeleteSkinnedMeshObject(ISkinnedMeshObject* iSkinnedMeshObject)
{
	mObjectManager->DeleteSkinnedMeshObject(iSkinnedMeshObject);
}

void FQGraphics::DrawSphere(const debug::SphereInfo& sphereInfo)
{
	mDebugDrawManager->Submit(sphereInfo);
}
void FQGraphics::DrawBox(const debug::AABBInfo& aabbInfo)
{
	mDebugDrawManager->Submit(aabbInfo);
}
void FQGraphics::DrawOBB(const debug::OBBInfo& obbInfo)
{
	mDebugDrawManager->Submit(obbInfo);
}
void FQGraphics::DrawFrustum(const debug::FrustumInfo& frustumInfo)
{
	mDebugDrawManager->Submit(frustumInfo);
}
void FQGraphics::DrawGrid(const debug::GridInfo& gridInfo)
{
	mDebugDrawManager->Submit(gridInfo);
}
void FQGraphics::DrawRing(const debug::RingInfo& ringInfor)
{
	mDebugDrawManager->Submit(ringInfor);
}
void FQGraphics::DrawRay(const debug::RayInfo& rayInfo)
{
	mDebugDrawManager->Submit(rayInfo);
}
void FQGraphics::DrawPolygon(const debug::PolygonInfo& polygonInfo)
{
	if (polygonInfo.Points.size() >= 3)
	{
		mDebugDrawManager->Submit(polygonInfo);
	}
}

IParticleObject* FQGraphics::CreateParticleObject(const ParticleInfo& particleInfo)
{
	return mParticleManager->CreateParticleObject(particleInfo);
}

void FQGraphics::DeleteParticleObject(IParticleObject* particleObject)
{
	mParticleManager->DeleteParticleObject(particleObject);
}

void FQGraphics::SetPipelineType(EPipelineType pipelineType)
{
	mRenderManager->Initialize(mDevice, mJobManager, mCameraManager, mLightManager, mResourceManager, mDebugDrawManager, mParticleManager, mDevice->GetWidth(), mDevice->GetHeight(), pipelineType);
}

ID3D11Device* FQGraphics::GetDivice()
{
	assert(mDevice != nullptr);
	return mDevice->GetDevice().Get();
}
ID3D11DeviceContext* FQGraphics::GetDeviceContext()
{
	assert(mDevice != nullptr);
	return mDevice->GetDeviceContext().Get();
}
ID3D11ShaderResourceView* FQGraphics::GetSRV()
{
	return mRenderManager->GetBackBufferSRV();
}