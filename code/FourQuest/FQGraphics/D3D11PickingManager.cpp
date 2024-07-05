#include "D3D11PickingManager.h"

#include "D3D11Device.h"
#include "D3D11ResourceManager.h"
#include "D3D11View.h"

#include "IFQRenderObject.h"

// temp
#include "D3D11JobManager.h"
#include "d3d11cameraManager.h"

#include "RenderObject.h"
#include "Mesh.h"
#include "d3d11Shader.h"
#include "ConstantBufferStructure.h"

fq::graphics::D3D11PickingManager::D3D11PickingManager()
	:mR(0),
	mG(0),
	mB()
{
}

void fq::graphics::D3D11PickingManager::Initialize(const std::shared_ptr<D3D11Device> device, const std::shared_ptr<D3D11ResourceManager> resourceManager, const unsigned short width, const unsigned short height)
{
	mPickingDrawRTV = resourceManager->Create<D3D11RenderTargetView>(ED3D11RenderTargetViewType::Picking, width, height);

	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = width;
	textureDesc.Height = height;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_STAGING;
	textureDesc.BindFlags = 0;
	textureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	textureDesc.MiscFlags = 0;

	device->GetDevice()->CreateTexture2D(&textureDesc, nullptr, &mCopyTexture);

	// 쉐이더 설정
	D3D_SHADER_MACRO macroSkinning[] =
	{
		{"SKINNING", ""},
		{ NULL, NULL}
	};

	mStaticMeshVS = std::make_shared<D3D11VertexShader>(device, L"PickingVS.cso");
	mSkinnedMeshVS = std::make_shared<D3D11VertexShader>(device, L"PickingVS_SKINNING.cso");
	mMeshPS = std::make_shared<D3D11PixelShader>(device, L"PickingPS.cso");

	mStaticMeshLayout = std::make_shared<D3D11InputLayout>(device, mStaticMeshVS->GetBlob().Get());
	mSkinnedMeshLayout = std::make_shared<D3D11InputLayout>(device, mSkinnedMeshVS->GetBlob().Get());

	mConstantBuffer = std::make_shared<D3D11ConstantBuffer<ModelTransform>>(device, ED3D11ConstantBuffer::Transform);
	mBoneTransformCB = std::make_shared<D3D11ConstantBuffer<BoneTransform>>(device, ED3D11ConstantBuffer::Transform);

	mDSV = resourceManager->Create<D3D11DepthStencilView>(ED3D11DepthStencilViewType::Picking, width, height);

	mBackBufferDSV = resourceManager->Get<D3D11DepthStencilView>(ED3D11DepthStencilViewType::None);
	mBackBufferRTV = resourceManager->Get<D3D11RenderTargetView>(ED3D11RenderTargetViewType::Default);
}

void fq::graphics::D3D11PickingManager::MakeObjectsHashColor(const std::set<IStaticMeshObject*>& staticMeshObjects,
	const std::set<ISkinnedMeshObject*>& skinnedMeshObjects,
	const std::set<ITerrainMeshObject*>& terrainMeshObjects)
{
	for (const auto& meshObject : staticMeshObjects)
	{
		NextColor();
		mStaticMeshObjects[meshObject] = DirectX::SimpleMath::Color{ mR / 255.0f, mG / 255.0f, mB / 255.0f };
	}
	for (const auto& meshObject : skinnedMeshObjects)
	{
		NextColor();
		mSkinnedMeshObjects[meshObject] = DirectX::SimpleMath::Color{ mR / 255.0f, mG / 255.0f, mB / 255.0f };
	}
	for (const auto& meshObject : terrainMeshObjects)
	{
		NextColor();
		mTerrainMeshObjects[meshObject] = DirectX::SimpleMath::Color{ mR / 255.0f, mG / 255.0f, mB / 255.0f };
	}
}

void fq::graphics::D3D11PickingManager::NextColor()
{
	if (mB == 255)
	{
		mB++;
		if (mG == 255)
		{
			mG++;
			if (mR == 255)
			{
				MessageBox(NULL, L"오브젝트의 개수가 1600만 개를 넘은 것 같습니다.", L"에러", MB_ICONERROR);
			}
			else
			{
				mR++;
			}
		}
		else
		{
			mG++;
		}
	}
	else
	{
		mB++;
	}
}

unsigned int fq::graphics::D3D11PickingManager::GetHashColor(const std::shared_ptr<D3D11Device> device, const short x, const short y)
{
	ID3D11DeviceContext* deviceContext = device->GetDeviceContext().Get();

	ID3D11Resource* pickingDrawResource = nullptr;
	mPickingDrawRTV->GetRTV()->GetResource(&pickingDrawResource);

	//deviceContext->CopyResource(mCopyTexture.Get(), pickingDrawResource);

	D3D11_BOX box;

	box.left = x;
	box.right = x + 1;
	box.top = y;
	box.bottom = y + 1;
	box.front = 0;
	box.back = 1;

	deviceContext->CopySubresourceRegion(mCopyTexture.Get(), 0, 0, 0, 0, pickingDrawResource, 0, &box);

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	deviceContext->Map(mCopyTexture.Get(), 0, D3D11_MAP_READ, 0, &mappedResource);

	if (mappedResource.pData == nullptr)
	{
		return 0;
	}

	unsigned int pickedObjectColor = ((UINT*)mappedResource.pData)[0];

	deviceContext->Unmap(mCopyTexture.Get(), 0);

	pickingDrawResource->Release();

	return pickedObjectColor;
}

unsigned int fq::graphics::D3D11PickingManager::MakeRGBAUnsignedInt(DirectX::SimpleMath::Color color)
{
	unsigned int rgba = 0;

	rgba = 255 << 24; //A
	rgba += (unsigned int)(color.z * 255) << 16; //B
	rgba += (unsigned int)(color.y * 255) << 8; //G
	rgba += (unsigned int)(color.x * 255); //R

	return rgba;
}


void* fq::graphics::D3D11PickingManager::GetPickedObject(const short x, const short y, const std::shared_ptr<D3D11Device>& device,
	const std::shared_ptr<D3D11CameraManager>& cameraManager,
	const std::shared_ptr<D3D11JobManager>& jobManager,
	const std::set<IStaticMeshObject*>& staticMeshObjects,
	const std::set<ISkinnedMeshObject*>& skinnedMeshObjects,
	const std::set<ITerrainMeshObject*>& terrainMeshObjects)
{
	mR = 0;
	mG = 0;
	mB = 0;

	MakeObjectsHashColor(staticMeshObjects, skinnedMeshObjects, terrainMeshObjects);
	DrawObject(device, cameraManager, jobManager, staticMeshObjects, skinnedMeshObjects, terrainMeshObjects);

	unsigned int pickedhashColor = GetHashColor(device, x, y);

	void* object = nullptr;

	// static
	for (auto it = mStaticMeshObjects.begin(); it != mStaticMeshObjects.end(); ++it) {
		if (MakeRGBAUnsignedInt(it->second) == pickedhashColor) {
			object = it->first;
		}
	}

	// skinned
	for (auto it = mSkinnedMeshObjects.begin(); it != mSkinnedMeshObjects.end(); ++it) {
		if (MakeRGBAUnsignedInt(it->second) == pickedhashColor) {
			object = it->first;
		}
	}

	// Terrain
	for (auto it = mTerrainMeshObjects.begin(); it != mTerrainMeshObjects.end(); ++it) {
		if (MakeRGBAUnsignedInt(it->second) == pickedhashColor) {
			object = it->first;
		}
	}

	EndRender(device);

	return object;
}

void fq::graphics::D3D11PickingManager::DrawObject(const std::shared_ptr<D3D11Device>& device,
	const std::shared_ptr<D3D11CameraManager>& cameraManager,
	const std::shared_ptr<D3D11JobManager>& jobManager,
	const std::set<IStaticMeshObject*>& staticMeshObjects,
	const std::set<ISkinnedMeshObject*>& skinnedMeshObjects,
	const std::set<ITerrainMeshObject*>& terrainMeshObjects)
{
	mPickingDrawRTV->Clear(device, { 0.f, 0.f, 0.f, 1.f });
	mDSV->Clear(device);
	mPickingDrawRTV->Bind(device, mDSV);

	// 나중에는 position 만 있는 걸로 수정해야함
	for (auto element : staticMeshObjects) { jobManager->CreateStaticMeshJob(element); }
	for (auto element : skinnedMeshObjects) { jobManager->CreateSkinnedMeshJob(element); }
	for (auto element : terrainMeshObjects) { jobManager->CreateTerrainMeshJob(element); }

	std::vector<StaticMeshJob> staticMeshJobs = jobManager->GetStaticMeshJobs();
	std::vector<SkinnedMeshJob> skinnedMeshJobs = jobManager->GetSkinnedMeshJobs();
	std::vector<TerrainMeshJob> terrainMeshJobs = jobManager->GetTerrainMeshJobs();

	device->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	mStaticMeshLayout->Bind(device);
	mStaticMeshVS->Bind(device);
	mMeshPS->Bind(device);
	mConstantBuffer->Bind(device, ED3D11ShaderType::VertexShader);

	for (const StaticMeshJob& job : staticMeshJobs)
	{
		StaticMeshObject* staticMeshObject = static_cast<StaticMeshObject*>(job.StaticMeshObject);
		std::shared_ptr<StaticMesh> staticMesh = std::static_pointer_cast<StaticMesh>(staticMeshObject->GetStaticMesh());

		staticMesh->Bind(device);

		ModelTransform modelTransform;
		modelTransform.color = mStaticMeshObjects[staticMeshObject]; //DirectX::SimpleMath::Color{ 0, 1, 0 };
		modelTransform.world = staticMeshObject->GetTransform().Transpose();
		modelTransform.ViewProj = (cameraManager->GetViewMatrix(ECameraType::Player) * cameraManager->GetProjectionMatrix(ECameraType::Player)).Transpose();
		mConstantBuffer->Update(device, modelTransform);

		staticMesh->Draw(device, job.SubsetIndex);
	}

	for (const TerrainMeshJob& job : terrainMeshJobs)
	{
		TerrainMeshObject* terrainObject = static_cast<TerrainMeshObject*>(job.TerrainMeshObject);
		std::shared_ptr<StaticMesh> staticMesh = std::static_pointer_cast<StaticMesh>(terrainObject->GetStaticMesh());
		staticMesh->Bind(device);

		ModelTransform modelTransform;
		modelTransform.color = mTerrainMeshObjects[terrainObject]; //DirectX::SimpleMath::Color{ 0, 1, 0 };
		modelTransform.world = (terrainObject->GetTransform()).Transpose();
		modelTransform.ViewProj = (cameraManager->GetViewMatrix(ECameraType::Player) * cameraManager->GetProjectionMatrix(ECameraType::Player)).Transpose();
		mConstantBuffer->Update(device, modelTransform);

		job.TerrainMesh->Draw(device, job.SubsetIndex);
	}

	mSkinnedMeshLayout->Bind(device);
	mSkinnedMeshVS->Bind(device);
	mBoneTransformCB->Bind(device, ED3D11ShaderType::VertexShader, 2);

	std::vector<DirectX::SimpleMath::Matrix> identityTransforms(BoneTransform::MAX_BOND_COUNT);
	for (const SkinnedMeshJob& job : skinnedMeshJobs)
	{
		SkinnedMeshObject* skinnedMeshObject = static_cast<SkinnedMeshObject*>(job.SkinnedMeshObject);
		std::shared_ptr<SkinnedMesh> skinnedMesh = std::static_pointer_cast<SkinnedMesh>(skinnedMeshObject->GetSkinnedMesh());
		std::shared_ptr<NodeHierarchyInstance> nodeHierarchyInstnace = std::static_pointer_cast<NodeHierarchyInstance>(skinnedMeshObject->GetNodeHierarchyInstance());
		skinnedMesh->Bind(device);

		ModelTransform modelTransform;
		modelTransform.color = mSkinnedMeshObjects[skinnedMeshObject]; //DirectX::SimpleMath::Color{ 0, 1, 0 };
		modelTransform.world = skinnedMeshObject->GetTransform().Transpose();
		modelTransform.ViewProj = (cameraManager->GetViewMatrix(ECameraType::Player) * cameraManager->GetProjectionMatrix(ECameraType::Player)).Transpose();
		mConstantBuffer->Update(device, modelTransform);

		if (nodeHierarchyInstnace != nullptr)
		{
			ConstantBufferHelper::UpdateBoneTransformCB(device, mBoneTransformCB, nodeHierarchyInstnace->GetTransposedFinalTransforms());
		}
		else
		{
			ConstantBufferHelper::UpdateBoneTransformCB(device, mBoneTransformCB, identityTransforms);
		}

		skinnedMesh->Draw(device, job.SubsetIndex);
	}

	jobManager->ClearAll();
}

void fq::graphics::D3D11PickingManager::OnResize(const short width, const short height, const std::shared_ptr<D3D11Device>& device)
{
	mPickingDrawRTV->OnResize(device, ED3D11RenderTargetViewType::Picking, width, height);
	mDSV->OnResize(device, ED3D11DepthStencilViewType::Picking, width, height);
}

void fq::graphics::D3D11PickingManager::EndRender(const std::shared_ptr<D3D11Device>& device)
{
	mBackBufferRTV->Bind(device, mBackBufferDSV);

	mStaticMeshObjects.clear();
	mSkinnedMeshObjects.clear();
}

