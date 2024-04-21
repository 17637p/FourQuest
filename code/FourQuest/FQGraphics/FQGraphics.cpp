#include "FQGraphics.h"

#include "Renderer.h"

#include "D3D11Device.h"
#include "ManagementCommon.h"

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
{
}

bool fq::graphics::FQGraphics::Initialize(const HWND hWnd, const unsigned short width, const unsigned short height)
{
	mDevice->Initialize(hWnd, width, height);
	mResourceManager = std::make_shared<D3D11ResourceManager>(mDevice);
	mObjectManager;
	mJobManager;
	mRenderManager->Initialize(mDevice, mResourceManager, width, height);

	return true;
}

bool fq::graphics::FQGraphics::Update(float deltaTime)
{
	return true;
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
	return true;
}

bool FQGraphics::CreateStaticMesh(std::string key, const fq::common::Mesh& meshData)
{
	return mObjectManager->CreateStaticMesh(mDevice, key, meshData);
}

bool FQGraphics::CreateSkinnedMesh(std::string key, const fq::common::Mesh& meshData)
{
	return mObjectManager->CreateSkinnedMesh(mDevice, key, meshData);
}

bool FQGraphics::CreateMaterial(std::string key, const fq::common::Material& matrialData, std::filesystem::path basePath)
{
	return mObjectManager->CreateMaterial(mDevice, key, matrialData, basePath);
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
