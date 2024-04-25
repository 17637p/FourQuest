#include "FQGraphics.h"

#include "D3D11Device.h"
#include "ManagementCommon.h"

#include "D3D11CameraManager.h"
#include "D3D11LightManager.h"

using namespace fq::graphics;

FQGraphics::~FQGraphics()
{

}

FQGraphics::FQGraphics()
	: mDevice(std::make_shared<D3D11Device>())
	, mResourceManager(nullptr)
	, mObjectManager(std::make_shared<D3D11ObjectManager>())
	, mJobManager(std::make_shared<D3D11JobManager>())
	, mRenderManager(std::make_shared<D3D11RenderManager>())
	, mCameraManager(std::make_shared<D3D11CameraManager>())
	, mLightManager(std::make_shared<D3D11LightManager>())
{
}

bool fq::graphics::FQGraphics::Initialize(const HWND hWnd, const unsigned short width, const unsigned short height)
{
	mDevice->Initialize(hWnd, width, height);
	mResourceManager = std::make_shared<D3D11ResourceManager>(mDevice);
	mObjectManager;
	mJobManager;
	mRenderManager->Initialize(mDevice, mResourceManager, width, height);
	mCameraManager->Initialize(width, height);
	mLightManager->Initialize(mDevice);

	return true;
}

bool fq::graphics::FQGraphics::Update(float deltaTime)
{
	return true;
}

void FQGraphics::DeleteLight(const unsigned int id)
{
	mLightManager->DeleteLight(id);
}

void FQGraphics::SetLight(const unsigned int id, const LightInfo& lightInfo)
{
	mLightManager->SetLight(id, lightInfo);
}

void FQGraphics::AddLight(const unsigned int id, const LightInfo& lightInfo)
{
	mLightManager->AddLight(id, lightInfo);
}

void FQGraphics::UpdateCamera(const fq::common::Transform& cameraTransform)
{
	mCameraManager->Update(ECameraType::Player, cameraTransform);
}

void FQGraphics::SetCamera(const CameraInfo& cameraInfo)
{
	mCameraManager->SetCamera(ECameraType::Player, cameraInfo);
}

bool FQGraphics::BeginRender()
{
	mRenderManager->BeginRender(mDevice);

	return true;
}

bool FQGraphics::Render()
{
	mJobManager->CreateStaticMeshJobs(mObjectManager->GetStaticMeshObjects());
	mJobManager->CreateSkinnedMeshJobs(mObjectManager->GetSkinnedMeshObjects());

	mRenderManager->Render(mDevice, mJobManager->GetStaticMeshJobs());
	mRenderManager->Render(mDevice, mJobManager->GetSkinnedMeshJobs());
	mRenderManager->RenderBackBuffer(mDevice);

	return true;
}

bool FQGraphics::EndRender()
{
	mRenderManager->EndRender(mDevice);
	mJobManager->ClearAll();

	return true;
}

bool FQGraphics::Finalize()
{
	return true;
}

bool FQGraphics::SetViewportSize(const unsigned short width, const unsigned short height)
{
	return true;
}

bool FQGraphics::SetWindowSize(const unsigned short width, const unsigned short height)
{
	mRenderManager->OnResize(mDevice, mResourceManager, width, height);
	mCameraManager->OnResize(width, height);

	return true;
}

const fq::common::Model& FQGraphics::CreateModel(std::string path, std::filesystem::path textureBasePath)
{
	return mObjectManager->CreateModel(mDevice, path, textureBasePath);
}

const fq::common::Model& FQGraphics::GetModel(std::string path)
{
	return mObjectManager->GetModel(path);
}

void FQGraphics::DeleteModel(std::string path)
{
	mObjectManager->DeleteModel(path);
}

void FQGraphics::ConvertModel(std::string fbxFile, std::string fileName)
{
	mObjectManager->ConvertModel(fbxFile, fileName);
}

IStaticMeshObject* FQGraphics::CreateStaticMeshObject(MeshObjectInfo info)
{
	return mObjectManager->CreateStaticMeshObject(info);
}

void FQGraphics::DeleteStaticMeshObject(IStaticMeshObject* meshObject)
{
	mObjectManager->DeleteStaticMeshObject(meshObject);
}

ISkinnedMeshObject* FQGraphics::CreateSkinnedMeshObject(MeshObjectInfo info)
{
	return mObjectManager->CreateSkinnedMeshObject(info);
}

void FQGraphics::DeleteSkinnedMeshObject(ISkinnedMeshObject* iSkinnedMeshObject)
{
	mObjectManager->DeleteSkinnedMeshObject(iSkinnedMeshObject);
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