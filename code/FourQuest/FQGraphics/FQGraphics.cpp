#include "FQGraphics.h"

#include "Renderer.h"

#include "D3D11ObjectManager.h"
#include "D3D11JobManager.h"
#include "D3D11RenderManager.h"

using namespace fq::graphics;

FQGraphics::~FQGraphics()
{

}

FQGraphics::FQGraphics()
	:mRenderer(nullptr)
{
	mRenderer = std::make_shared<Renderer>();
}

bool fq::graphics::FQGraphics::Initialize(const HWND hWnd, const unsigned short width, const unsigned short height)
{
	mRenderer->Initialize(hWnd, width, height);

	mObjectManager = std::make_shared<D3D11ObjectManager>();
	mJobManager = std::make_shared<D3D11JobManager>();
	mRenderManager = std::make_shared<D3D11RenderManager>(mRenderer->GetDevice());

	return true;
}

bool fq::graphics::FQGraphics::Update(float deltaTime)
{
	return true;
}

bool FQGraphics::BeginRender()
{
	mRenderer->BeginRender();

	return true;
}

bool FQGraphics::Render()
{
	const std::set<IStaticMeshObject*>& staticMeshObjects = mObjectManager->GetStaticMeshObjects();

	for (IStaticMeshObject* staticMeshObject : staticMeshObjects)
	{
		mJobManager->CreateStaticMeshJob(staticMeshObject);
	}

	mRenderManager->Render(mRenderer->GetDevice(), mJobManager->GetStaticMeshJobs());

	mJobManager->ClearStaticMeshJobs();

	return true;
}

bool FQGraphics::EndRender()
{
	mRenderer->EndRender();

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
	return mObjectManager->CreateStaticMesh(mRenderer->GetDevice(), key, meshData);
}

bool FQGraphics::CreateMaterial(std::string key, const fq::common::Material& matrialData, std::filesystem::path basePath)
{
	return mObjectManager->CreateMaterial(mRenderer->GetDevice(), key, matrialData, basePath);
}

IStaticMeshObject* FQGraphics::CreateStaticMeshObject(MeshObjectInfo info)
{
	return mObjectManager->CreateStaticMeshObject(info);
}

void FQGraphics::DeleteMeshObject(IStaticMeshObject* meshObject)
{
	mObjectManager->DeleteMeshObject(meshObject);
}

