#include "FQGraphics.h"
#include "D3D11Device.h"
#include "ManagementCommon.h"
#include "D3D11Shader.h"

// temp - 컬링 테스트할 때 transform 잠깐 쓰려고
#include "../FQCommon/FQCommon.h"

#include "D3D11Texture.h"

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
	, mDecalManager(std::make_shared<D3D11DecalManager>())
	, mLightProbeManager(std::make_shared<D3D11LightProbeManager>())
{
}

bool fq::graphics::FQGraphics::Initialize(const HWND hWnd, const unsigned short width, const unsigned short height, EPipelineType pipelineType)
{
	Finalize();

#ifdef _DEBUG
	D3D11Shader::SetBasePath(L"../CompiledShader/Debug/");
#else
	D3D11Shader::SetBasePath(L"../CompiledShader/Release/");
#endif // _DEBUG

	mDevice->Initialize(hWnd, width, height);
	mResourceManager = std::make_shared<D3D11ResourceManager>(mDevice);
	mObjectManager;
	mJobManager;
	mModelManager->Initialize(mDevice, mResourceManager);
	mCameraManager->Initialize(width, height);
	mLightManager->Initialize(mDevice);
	mDebugDrawManager->Initialize(mDevice);
	mRenderManager->Initialize(mDevice, mJobManager, mCameraManager, mLightManager, mResourceManager, mDebugDrawManager, mParticleManager, mDecalManager, width, height, pipelineType);
	mPickingManager->Initialize(mDevice, mResourceManager, width, height);
	mParticleManager->Initialize(mDevice, mResourceManager, mCameraManager);
	mDecalManager->Initialize(mDevice, mResourceManager);
	mLightProbeManager->Initialize(mDevice, mResourceManager);

	mUIManager->Initialize(hWnd, mDevice, mResourceManager, width, height);

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

void FQGraphics::DeleteCubeProbe(unsigned short index)
{

}

void FQGraphics::SaveCubeProbeTexture(const unsigned short width, const unsigned short height)
{
	DirectX::SimpleMath::Quaternion front = 
		DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(0, 0, 0);
		//DirectX::SimpleMath::Quaternion::CreateFromAxisAngle({0, 0, 0}, 1.0f);
	DirectX::SimpleMath::Quaternion back =
		DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(DirectX::XMConvertToRadians(180), 0, 0);
		//DirectX::SimpleMath::Quaternion::CreateFromAxisAngle({ 0, 180, 0 }, 1.0f);
	DirectX::SimpleMath::Quaternion up =
		DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(0, DirectX::XMConvertToRadians(-90), 0);
		//DirectX::SimpleMath::Quaternion::CreateFromAxisAngle({ -90, 0, 0 }, 1.0f);
	DirectX::SimpleMath::Quaternion bottom =
		DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(0, DirectX::XMConvertToRadians(90), 0);
		//DirectX::SimpleMath::Quaternion::CreateFromAxisAngle({ 90, 0, 0 }, 1.0f);
	DirectX::SimpleMath::Quaternion left =
		DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(DirectX::XMConvertToRadians(-90), 0, 0);
		//DirectX::SimpleMath::Quaternion::CreateFromAxisAngle({ -0, -90, 0 }, 1.0f);
	DirectX::SimpleMath::Quaternion right =
		DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(DirectX::XMConvertToRadians(90), 0, 0);
		//DirectX::SimpleMath::Quaternion::CreateFromAxisAngle({ 0, 90, 0 }, 1.0f);

	DirectX::SimpleMath::Quaternion directionQuaternions[] = { right, left, up, bottom, front, back };

	// 카메라 Probe 기준으로 설정
	unsigned short curWidth = mDevice->GetWidth();
	unsigned short curHeight = mDevice->GetHeight();

	float curFieldOfView = mCameraManager->GetFovY(ECameraType::Player);

	CameraInfo cameraInfo;
	cameraInfo.isPerspective = true;
	cameraInfo.fieldOfView = 90 * (3.14 / 180);
	cameraInfo.nearPlain = mCameraManager->GetNearPlane(ECameraType::Player);
	cameraInfo.farPlain = mCameraManager->GetFarPlane(ECameraType::Player);

	SetCamera(cameraInfo);
	SetWindowSize(width, height);

	// 프로브를 가져와서 카메라 위치 설정 하나당 6방향으로
	std::vector<std::wstring> paths{};

	std::unordered_map<unsigned short, CubeProbe*> cubeProbes = mLightProbeManager->GetCubeProbes();
	for (const auto& cubeProbe : cubeProbes)
	{
		fq::common::Transform probeTransform;

		probeTransform.worldPosition = cubeProbe.second->position;
		probeTransform.worldScale = { 1, 1, 1 };

		for (unsigned int i = 0; i < 6; i++)
		{
			probeTransform.worldRotation = directionQuaternions[i];
			probeTransform.worldMatrix = 
				DirectX::SimpleMath::Matrix::CreateScale(probeTransform.worldScale) *
				DirectX::SimpleMath::Matrix::CreateFromQuaternion(probeTransform.worldRotation) *
				DirectX::SimpleMath::Matrix::CreateTranslation(probeTransform.worldPosition);
			
			UpdateCamera(probeTransform);

			// 이 부분 나중에 개선해야함, 그리는 건 항상 같은데 job 을 다시 만들 필요가 없음 
			BeginRender();
			Render();

			mJobManager->ClearAll();
			//EndRender();

			std::wstring path = mLightProbeManager->SaveCubeProbeTexture();
			paths.push_back(path);
		}
		D3D11Texture cubeMap{ mDevice, paths };
	}

	// 카메라 재설정 
	cameraInfo.fieldOfView = curFieldOfView;

	SetCamera(cameraInfo);
	SetWindowSize(curWidth, curHeight);
	// 드로우 6면 일단 각각 다른 파일로 저장하고 나중에는 6면을 한장에 저장하자
	// 파일 저장
}

unsigned short FQGraphics::AddCubeProbe(const DirectX::SimpleMath::Vector3& position)
{
	return mLightProbeManager->AddCubeProbe(position);
}

void FQGraphics::DeleteImageObject(IImageObject* imageObject)
{
	mUIManager->DeleteImage(imageObject);
}

fq::graphics::IImageObject* FQGraphics::CreateImageObject(const UIInfo& uiInfo)
{
	return mUIManager->CreateImageObject(uiInfo);
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
	return mObjectManager->CreateTerrainMeshObject(mDevice, mModelManager, info);
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
	return mPickingManager->GetPickedObject(mouseX, mouseY, mDevice, mCameraManager, mJobManager, mObjectManager->GetStaticMeshObjects(), mObjectManager->GetSkinnedMeshObjects(), mObjectManager->GetTerrainMeshObjects());
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

	mJobManager->CreateStaticMeshJobs(mObjectManager->GetStaticMeshObjects());
	mJobManager->CreateSkinnedMeshJobs(mObjectManager->GetSkinnedMeshObjects());
	mJobManager->CreateTerrainMeshJobs(terrainMeshesToRender);

	mRenderManager->Render();
	mUIManager->Render();
	mRenderManager->RenderFullScreen();
	return true;
}

bool FQGraphics::EndRender()
{
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

std::vector<std::shared_ptr<IMaterial>> FQGraphics::GetMaterials() const
{
	return mModelManager->GetMaterials();
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

IDecalObject* FQGraphics::CreateDecalObject(const DecalInfo& decalInfo)
{
	return mDecalManager->CreateDecalObject(decalInfo);
}
void FQGraphics::DeleteDecalObject(IDecalObject* decalObjectInterface)
{
	mDecalManager->DeleteDecalObject(decalObjectInterface);
}

void FQGraphics::SetPipelineType(EPipelineType pipelineType)
{
	mRenderManager->Initialize(mDevice, mJobManager, mCameraManager, mLightManager, mResourceManager, mDebugDrawManager, mParticleManager, mDecalManager, mDevice->GetWidth(), mDevice->GetHeight(), pipelineType);
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