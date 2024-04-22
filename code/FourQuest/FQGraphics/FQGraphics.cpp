#include "FQGraphics.h"

#include "D3D11Device.h"
#include "ManagementCommon.h"

#include "../FQLoader/ModelLoader.h"
#include "../FQLoader/ModelConverter.h"

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

const fq::common::Model& FQGraphics::CreateModel(std::string path, std::filesystem::path textureBasePath)
{
	auto find = mModels.find(path);

	if (find != mModels.end())
	{
		return find->second;
	}

	fq::common::Model model = fq::loader::ModelLoader::ReadModel(path);

	for (const auto& material : model.Materials)
	{
		if (material.Name.empty())
		{
			continue;
		}

		mObjectManager->CreateMaterial(mDevice, path + material.Name, material, textureBasePath);
	}

	for (const auto& nodeMeshPair : model.Meshes)
	{
		const auto& mesh = nodeMeshPair.second;

		if (mesh.Name.empty())
		{
			continue;
		}

		if (mesh.BoneVertices.empty())
		{
			mObjectManager->CreateStaticMesh(mDevice, path + mesh.Name, mesh);
		}
		else
		{
			mObjectManager->CreateSkinnedMesh(mDevice, path + mesh.Name, mesh);
		}
	}

	mModels.insert({ path, std::move(model) });

	return mModels[path];
}

const fq::common::Model& FQGraphics::GetModel(std::string path)
{
	auto find = mModels.find(path);
	assert(find != mModels.end());
	return find->second;
}

void FQGraphics::DeleteModel(std::string path)
{
	auto find = mModels.find(path);

	if (find == mModels.end())
	{
		return;
	}

	const fq::common::Model& model = find->second;

	for (const auto& material : model.Materials)
	{
		if (material.Name.empty())
		{
			continue;
		}

		mObjectManager->DeleteMaterial(path + material.Name);
	}

	for (const auto& nodeMeshPair : model.Meshes)
	{
		const auto& mesh = nodeMeshPair.second;

		if (mesh.Name.empty())
		{
			continue;
		}

		if (mesh.BoneVertices.empty())
		{
			mObjectManager->DeleteStaticMesh(path + mesh.Name);
		}
		else
		{
			mObjectManager->DeleteSkinnedMesh(path + mesh.Name);
		}
	}

	mModels.erase(find);
}

void FQGraphics::ConvertModel(std::string fbxFile, std::string path)
{
	fq::loader::ModelConverter converter;
	converter.ReadFBXFile(fbxFile);
	converter.WriteModel(path);
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