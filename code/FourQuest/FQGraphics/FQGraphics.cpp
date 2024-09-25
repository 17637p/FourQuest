#include "FQGraphics.h"
#include "D3D11Device.h"
#include "ManagementCommon.h"
#include "D3D11Shader.h"
#include "D3D11Texture.h"
#include "../FQLoader/ModelLoader.h"

// temp - 컬링 테스트할 때 transform 잠깐 쓰려고
#include "../FQCommon/FQCommon.h"

#include "D3D11Texture.h"

using namespace fq::graphics;

FQGraphics::~FQGraphics()
{
	mRenderManager = nullptr;
	mObjectManager = nullptr;
	mModelManager = nullptr;
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
	, mLightProbeManager(std::make_shared<D3D11LightProbeManager>())
	, mPostProcessingManager(std::make_shared<D3D11PostProcessingManager>())
	, mIsOnPostProcessing(true)
	, mIsRenderObjects(true)

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
	mRenderManager->Initialize(mDevice, mJobManager, mCameraManager, mLightManager, mResourceManager, mDebugDrawManager, mParticleManager, mObjectManager, mLightProbeManager, width, height, pipelineType);
	mPickingManager->Initialize(mDevice, mResourceManager, width, height);
	mParticleManager->Initialize(mDevice, mResourceManager, mCameraManager);
	mLightProbeManager->Initialize(mDevice, mResourceManager);
	mUIManager->Initialize(hWnd, mDevice, mResourceManager, width, height);
	mPostProcessingManager->Initialize(mDevice, mResourceManager, mCameraManager, width, height);

	return true;
}

bool fq::graphics::FQGraphics::Update(float deltaTime)
{
	return true;
}

void fq::graphics::FQGraphics::SetSkyBox(const std::wstring& path, bool bUseIBL, float envScale)
{
	auto skybox = std::make_shared<D3D11CubeTexture>(mDevice, path);
	mLightManager->SetSkybox(skybox);

	if (bUseIBL)
	{
		IBLTexture iblTexture = mLightManager->CreateIBLTexture(mDevice, skybox, EEnvironmentFormat::RGBA32, EEnvironmentResoulution::Size1024x1024, EEnvironmentResoulution::Size32x32, envScale);
		mLightManager->SetIBLTexture(iblTexture);
	}
}
void fq::graphics::FQGraphics::SetIBLTexture(const std::wstring& diffusePath, const std::wstring& specularPath, const std::wstring& brdfLUTPath)
{
	IBLTexture iblTexture;
	iblTexture.SpecularIBL = std::make_shared<D3D11CubeTexture>(mDevice, specularPath);
	iblTexture.DiffuseIrradiance = std::make_shared<D3D11CubeTexture>(mDevice, diffusePath);
	iblTexture.SpecularBRDF = std::make_shared<D3D11Texture>(mDevice, brdfLUTPath);
	mLightManager->SetIBLTexture(iblTexture);
}

void FQGraphics::LoadLightProbes(const std::string& fileName)
{
	bool isSucceedLoad = mLightProbeManager->LoadLightProbes(fileName);
	if (isSucceedLoad)
	{
		mLightProbeManager->SetIsUsedLightProbe(true);
	}
}

void FQGraphics::SaveLightProbes(const std::string& fileName)
{
	mLightProbeManager->SaveLightProbes(fileName);
}

void FQGraphics::DeleteLightProbe(int index)
{
	mLightProbeManager->DeleteLightProbe(index);
	// 삭제하면 바로 사면체를 만들기 때문에 안 꺼도 됨
	//mLightProbeManager->SetIsUsedLightProbe(false);
}

void FQGraphics::BakeLightProbe(bool isAll)
{
	mLightProbeManager->BakeAllLightProbeCoefficient(isAll);
	mLightProbeManager->MakeTetrahedron();
	mLightProbeManager->SetIsUsedLightProbe(true);
}

int FQGraphics::AddLightProbe(const DirectX::SimpleMath::Vector3& position)
{
	return mLightProbeManager->AddLightProbe(position);
}

void FQGraphics::DeleteCubeProbe(unsigned short index)
{

}

void FQGraphics::SaveCubeProbeTexture(bool isAll, const unsigned short width, const unsigned short height)
{
	mLightProbeManager->SetIsUsedLightProbe(false);

	DirectX::SimpleMath::Quaternion front =
		DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(0, 0, 0);
	DirectX::SimpleMath::Quaternion back =
		DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(DirectX::XMConvertToRadians(180), 0, 0);
	DirectX::SimpleMath::Quaternion up =
		DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(0, DirectX::XMConvertToRadians(-90), 0);
	DirectX::SimpleMath::Quaternion bottom =
		DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(0, DirectX::XMConvertToRadians(90), 0);
	DirectX::SimpleMath::Quaternion left =
		DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(DirectX::XMConvertToRadians(-90), 0, 0);
	DirectX::SimpleMath::Quaternion right =
		DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(DirectX::XMConvertToRadians(90), 0, 0);

	DirectX::SimpleMath::Quaternion directionQuaternions[] = { right, left, up, bottom, front, back };
	std::vector<std::wstring> directions = { L"right", L"left", L"up", L"bottom", L"front", L"back" };

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
	mIsOnPostProcessing = false;
	//std::vector<CubeProbe*> cubeProbes = mLightProbeManager->GetCubeProbes();
	//for (const auto& cubeProbe : cubeProbes)
	//{
	//	fq::common::Transform probeTransform;
	//
	//	probeTransform.worldPosition = cubeProbe->position;
	//	probeTransform.worldScale = { 1, 1, 1 };
	//
	//	for (unsigned int i = 0; i < 6; i++)
	//	{
	//		probeTransform.worldRotation = directionQuaternions[i];
	//		probeTransform.worldMatrix =
	//			DirectX::SimpleMath::Matrix::CreateScale(probeTransform.worldScale) *
	//			DirectX::SimpleMath::Matrix::CreateFromQuaternion(probeTransform.worldRotation) *
	//			DirectX::SimpleMath::Matrix::CreateTranslation(probeTransform.worldPosition);
	//
	//		UpdateCamera(probeTransform);
	//
	//		// 이 부분 나중에 개선해야함, 그리는 건 항상 같은데 job 을 다시 만들 필요가 없음 
	//		BeginRender();
	//		Render();
	//
	//		mJobManager->ClearAll();
	//		//EndRender();
	//
	//		std::wstring path = mLightProbeManager->SaveProbe1DirectionTexture(cubeProbe->index, directions[i]);
	//		paths.push_back(path);
	//	}
	//	D3D11Texture cubeMap{ mDevice, paths };
	//	std::wstring cubeMapFileName = L"CubeProbe" + std::to_wstring(cubeProbe->index) + L".dds";
	//	cubeMap.Save(mDevice, cubeMapFileName);
	//}

	std::vector<LightProbe*> lightProbes = mLightProbeManager->GetLightProbes();
	for (int lightProbeIndex = 0; lightProbeIndex < lightProbes.size(); lightProbeIndex++)
	{
		if (lightProbes[lightProbeIndex]->isBaked && !isAll)
		{
			continue;
		}

		fq::common::Transform probeTransform;

		probeTransform.worldPosition = lightProbes[lightProbeIndex]->position;
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

			std::wstring path = mLightProbeManager->SaveProbe1DirectionTexture(lightProbeIndex, directions[i]);
			paths.push_back(path);
		}
		D3D11Texture cubeMap{ mDevice, paths };
		std::wstring cubeMapFileName = L"LightProbe" + std::to_wstring(lightProbeIndex) + L".dds";
		cubeMap.Save(mDevice, cubeMapFileName);
		paths.clear();
	}

	// 카메라 재설정 
	cameraInfo.fieldOfView = curFieldOfView;

	SetCamera(cameraInfo);
	SetWindowSize(curWidth, curHeight);

	mIsOnPostProcessing = true;
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
	return mPickingManager->GetPickedObject(mouseX, mouseY, mDevice, mCameraManager, mJobManager,
		mObjectManager->GetStaticMeshObjects(),
		mObjectManager->GetSkinnedMeshObjects(),
		mObjectManager->GetTerrainMeshObjects(),
		mObjectManager->GetProbeObjects());
}

bool fq::graphics::FQGraphics::GetIsUsePostProcessing() const
{
	return mIsOnPostProcessing;
}

void fq::graphics::FQGraphics::SetIsUsePostProcessing(bool bUsePostProcessing)
{
	mIsOnPostProcessing = bUsePostProcessing;
}

void fq::graphics::FQGraphics::SetPostProcessingInfo(const PostProcessingInfo& info)
{
	mPostProcessingManager->SetPostProcessingInfo(info);
}

const PostProcessingInfo& fq::graphics::FQGraphics::GetPostProcessingInfo() const
{
	return mPostProcessingManager->GetPostProcessingInfo();
}

std::shared_ptr<spdlog::logger> FQGraphics::SetUpLogger(std::vector<spdlog::sink_ptr> sinks)
{
	auto logger = std::make_shared<spdlog::logger>("Graphics",
		std::begin(sinks),
		std::end(sinks));

	spdlog::set_default_logger(logger);
	return logger;
}

void fq::graphics::FQGraphics::CreateAnimationTexture(std::shared_ptr<INodeHierarchy> nodeHierarchyInterface)
{
	std::static_pointer_cast<NodeHierarchy>(nodeHierarchyInterface)->CreateAnimationTexture(mDevice);
}

void fq::graphics::FQGraphics::ReleaseAnimationTexture(std::shared_ptr<INodeHierarchy> nodeHierarchyInterface)
{
	std::static_pointer_cast<NodeHierarchy>(nodeHierarchyInterface)->ReleaseAnimationTexture();
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

void fq::graphics::FQGraphics::SetLightMapTexture(const std::vector<std::filesystem::path>& paths)
{
	mLightManager->CreateLightMapTextureArray(mDevice, paths);
}

void fq::graphics::FQGraphics::SetLightMapDirectionTexture(const std::vector<std::filesystem::path>& paths)
{
	mLightManager->CreateLightMapDirectionTextureArray(mDevice, paths);
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
	if (mIsRenderObjects)
	{
		mLightManager->UpdateConstantBuffer(mDevice, mCameraManager->GetPosition(ECameraType::Player), true);
		mLightManager->UpdateShadowConstantBuffer(mDevice, mCameraManager);
	}

	mRenderManager->BeginRender();

	return true;
}

bool FQGraphics::Render()
{
	if (mIsRenderObjects)
	{
		// 랜더링 오브젝트의 수정된 데이터를 기반으로 리소스 갱신
		mObjectManager->UpdateModifiedResources(mDevice);
		mObjectManager->CheckDecalLayer();

		// 컬링 추가해야 됨 
		std::set<IStaticMeshObject*> staticMeshesToRender = mObjectManager->GetStaticMeshObjects();
		std::set<ISkinnedMeshObject*> skinnedMeshesToRender = mObjectManager->GetSkinnedMeshObjects();
		std::set<ITerrainMeshObject*> terrainMeshesToRender = mObjectManager->GetTerrainMeshObjects();

		// staticMeshesToRender = mCullingManager->GetInFrustumStaticObjects(staticMeshesToRender);
		// skinnedMeshesToRender = mCullingManager->GetInFrustumSkinnedObjects(skinnedMeshesToRender);

		for (auto element : staticMeshesToRender) { mJobManager->CreateStaticMeshJob(element); }
		for (auto element : skinnedMeshesToRender) { mJobManager->CreateSkinnedMeshJob(element); }
		for (auto element : terrainMeshesToRender) { mJobManager->CreateTerrainMeshJob(element); }

		mJobManager->SortStaticMeshJob();
		mJobManager->SortSkinnedMeshJob();

		mRenderManager->Render();

		if (mIsOnPostProcessing)
		{
			mPostProcessingManager->CopyOffscreenBuffer(mDevice);
			mPostProcessingManager->Excute(mDevice);
			mPostProcessingManager->RenderFullScreen(mDevice);
		}
	}

	mUIManager->Render();
	mRenderManager->RenderFullScreen();

	return true;
}

bool FQGraphics::EndRender()
{
	mRenderManager->EndRender();

	if (mIsRenderObjects)
	{
		mJobManager->ClearAll();
		mObjectManager->ClearDeleteQueue();
	}

	return true;
}

bool FQGraphics::Finalize()
{
	mRenderManager->Finalize();

	return true;
}

bool FQGraphics::SetViewportSize(const unsigned short width, const unsigned short height)
{
	mCameraManager->OnResize(width, height);

	return true;
}

bool FQGraphics::SetWindowSize(const unsigned short width, const unsigned short height)
{
	// 16:9 비율 유지하기 위한 스케일링 계산
	float renderAspect = 16.0f / 9.0f;
	float windowAspect = (float)width / height;

	int fixed16_9Width = 0;
	int fixed16_9Height = 0;
	if (windowAspect > renderAspect)
	{
		// 창이 더 넓은 경우
		fixed16_9Width = height * renderAspect;
		fixed16_9Height = height;
	}
	else
	{
		// 창이 더 높은 경우
		fixed16_9Width = width;
		fixed16_9Height = width / renderAspect;
	}

	if (width < 10 || height < 10)
	{
		mUIManager->ReleaseRenderTarget();
		mRenderManager->OnResize(width, height, width, height);
		mCameraManager->OnResize(width, height);
		mPickingManager->OnResize(width, height, mDevice);
		mUIManager->OnResize(mDevice, width, height);
		mPostProcessingManager->OnResize(mDevice, mResourceManager, width, height);

		return true;
	}
	else
	{
		mUIManager->ReleaseRenderTarget();
		mRenderManager->OnResize(fixed16_9Width, fixed16_9Height, width, height);
		mCameraManager->OnResize(fixed16_9Width, fixed16_9Height);
		mPickingManager->OnResize(fixed16_9Width, fixed16_9Height, mDevice);
		mUIManager->OnResize(mDevice, fixed16_9Width, fixed16_9Height);
		mPostProcessingManager->OnResize(mDevice, mResourceManager, fixed16_9Width, fixed16_9Height);

		return true;
	}
}

void FQGraphics::WriteModel(const std::string& path, const fq::common::Model& modelData)
{
	mModelManager->WriteModel(path, modelData);
}
fq::common::Model fq::graphics::FQGraphics::ReadModel(const std::string& path)
{
	return mModelManager->ReadModel(path);
}
void fq::graphics::FQGraphics::WriteAnimation(const std::string& path, const fq::common::AnimationClip& animationClip)
{
	return fq::loader::AnimationLoader::Write(animationClip, path);
}

fq::common::AnimationClip fq::graphics::FQGraphics::ReadAnimation(const std::string& path)
{
	return fq::loader::AnimationLoader::Read(path);
}
void fq::graphics::FQGraphics::WriteUVAnimation(const std::string& path, const fq::common::UVAnimationClip& uvAnimationClip)
{
	fq::loader::UVAnimationLoader::Write(uvAnimationClip, path);
}
fq::common::UVAnimationClip fq::graphics::FQGraphics::ReadUVAnimation(const std::string& path)
{
	return fq::loader::UVAnimationLoader::Read(path);
}
void fq::graphics::FQGraphics::WriteNodeHierarchy(const std::string& path, const std::vector<fq::common::Node>& nodeHierarchy)
{
	fq::loader::NodeHierarchyLoader::Write(nodeHierarchy, path);
}
void fq::graphics::FQGraphics::WriteNodeHierarchy(const std::string& path, const fq::common::Model& modelData)
{
	fq::loader::NodeHierarchyLoader::Write(modelData, path);
}
std::vector<fq::common::Node> fq::graphics::FQGraphics::ReadNodeHierarchy(const std::string& path)
{
	return fq::loader::NodeHierarchyLoader::Read(path);
}
void fq::graphics::FQGraphics::WriteMaterialInfo(const std::string& path, const MaterialInfo& materialInfo)
{
	fq::loader::MaterialLoader::Write(path, materialInfo);
}
MaterialInfo fq::graphics::FQGraphics::ReadMaterialInfo(const std::string& path)
{
	std::string tempPath = path;

	for (auto& ch : tempPath)
	{
		if (ch == '|')
		{
			ch = 'a';
		}
	}

	return fq::loader::MaterialLoader::Read(tempPath);
}

fq::common::Model fq::graphics::FQGraphics::ConvertModel(const std::string& fbxFile)
{
	return mModelManager->ConvertModel(fbxFile);
}

const fq::common::Model& FQGraphics::CreateModelResource(unsigned int key, const std::string& path, std::filesystem::path textureBasePath)
{
	return mModelManager->CreateModelResource(mDevice, key, path, textureBasePath);
}

void fq::graphics::FQGraphics::CreateModelResource(unsigned int key, const fq::common::Model& modelData, std::filesystem::path textureBasePath)
{
	mModelManager->CreateModelResource(mDevice, key, modelData, textureBasePath);
}

bool FQGraphics::TryCreateModelResource(unsigned int key, const std::string& path, std::filesystem::path textureBasePath, fq::common::Model* outDataOrNull)
{
	return mModelManager->TryCreateModelResource(mDevice, key, path, textureBasePath, outDataOrNull);
}

const fq::common::Model& FQGraphics::GetModel(unsigned int key)
{
	return mModelManager->GetModel(key);
}

void FQGraphics::DeleteModelResource(unsigned int key)
{
	mModelManager->DeleteModelResource(key);
}

std::shared_ptr<INodeHierarchy> fq::graphics::FQGraphics::GetNodeHierarchyByModelPathOrNull(unsigned int key)
{
	return mModelManager->GetNodeHierarchyByModelPathOrNull(key);
}

std::shared_ptr<IStaticMesh> fq::graphics::FQGraphics::GetStaticMeshByModelPathOrNull(unsigned int key, std::string meshName)
{
	return mModelManager->GetStaticMeshByModelPathOrNull(key, meshName);
}
std::shared_ptr<ISkinnedMesh> fq::graphics::FQGraphics::GetSkinnedMeshByModelPathOrNull(unsigned int key, std::string meshName)
{
	return mModelManager->GetSkinnedMeshByModelPathOrNull(key, meshName);
}
std::shared_ptr<IMaterial> fq::graphics::FQGraphics::GetMaterialByModelPathOrNull(unsigned int key, std::string materialName)
{
	return mModelManager->GetMaterialByModelPathOrNull(key, materialName);
}

std::shared_ptr<IAnimation> fq::graphics::FQGraphics::GetAnimationByModelPathOrNull(unsigned int key, std::string animationName)
{
	return mModelManager->GetAnimationByModelPathOrNull(key, animationName);
}

std::shared_ptr<IStaticMesh> fq::graphics::FQGraphics::CreateStaticMesh(const fq::common::Mesh& meshData)
{
	return mModelManager->CreateStaticMesh(meshData);
}

std::shared_ptr<ISkinnedMesh> fq::graphics::FQGraphics::CreateSkinnedMesh(const fq::common::Mesh& meshData)
{
	return mModelManager->CreateSkinnedMesh(meshData);
}

std::shared_ptr<INodeHierarchy> fq::graphics::FQGraphics::CreateNodeHierarchy(const fq::common::Model& model)
{
	return mModelManager->CreateNodeHierarchy(model);
}

std::shared_ptr<INodeHierarchy> fq::graphics::FQGraphics::CreateNodeHierarchy(const std::vector<fq::common::Node> nodes)
{
	return mModelManager->CreateNodeHierarchy(nodes);
}

std::shared_ptr<IAnimation> fq::graphics::FQGraphics::CreateAnimation(const fq::common::AnimationClip& animationClip)
{
	return mModelManager->CreateAnimation(animationClip);
}

std::shared_ptr<IUVAnimation> fq::graphics::FQGraphics::CreateUVAnimation(const fq::common::UVAnimationClip& animationClip)
{
	return mModelManager->CreateUVAnimation(animationClip);
}

std::shared_ptr<IMaterial> FQGraphics::CreateMaterial(const MaterialInfo& materialInfo)
{
	return mModelManager->CreateMaterial(materialInfo);
}
std::shared_ptr<IParticleMaterial> fq::graphics::FQGraphics::CreateParticleMaterial(const ParticleMaterialInfo& materialInfo)
{
	return mModelManager->CreateParticleMaterial(materialInfo);
}
std::shared_ptr<IDecalMaterial> fq::graphics::FQGraphics::CreateDecalMaterial(const DecalMaterialInfo& decalMaterialInfo)
{
	return mModelManager->CreateDecalMaterial(decalMaterialInfo);
}
std::shared_ptr<IStaticMesh> fq::graphics::FQGraphics::CreateStaticMesh(std::string key, const fq::common::Mesh& meshData)
{
	return mModelManager->CreateStaticMesh(key, meshData);
}
std::shared_ptr<ISkinnedMesh> fq::graphics::FQGraphics::CreateSkinnedMesh(std::string key, const fq::common::Mesh& meshData)
{
	return mModelManager->CreateSkinnedMesh(key, meshData);
}
std::shared_ptr<INodeHierarchy> fq::graphics::FQGraphics::CreateNodeHierarchy(std::string key, const fq::common::Model& model)
{
	return mModelManager->CreateNodeHierarchy(key, model);
}
std::shared_ptr<INodeHierarchy> fq::graphics::FQGraphics::CreateNodeHierarchy(std::string key, const std::vector<fq::common::Node> nodes)
{
	return mModelManager->CreateNodeHierarchy(key, nodes);
}
std::shared_ptr<IAnimation> fq::graphics::FQGraphics::CreateAnimation(std::string key, const fq::common::AnimationClip& animationClip)
{
	return mModelManager->CreateAnimation(key, animationClip);
}
std::shared_ptr<IUVAnimation> fq::graphics::FQGraphics::CreateUVAnimation(std::string key, const fq::common::UVAnimationClip& animationClip)
{
	return mModelManager->CreateUVAnimation(key, animationClip);
}
std::shared_ptr<IMaterial> fq::graphics::FQGraphics::CreateMaterial(const std::string& key, const MaterialInfo& materialInfo)
{
	return mModelManager->CreateMaterial(key, materialInfo);
}
std::shared_ptr<IParticleMaterial> fq::graphics::FQGraphics::CreateParticleMaterial(const std::string& key, const ParticleMaterialInfo& materialInfo)
{
	return mModelManager->CreateParticleMaterial(key, materialInfo);
}
std::shared_ptr<IDecalMaterial> fq::graphics::FQGraphics::CreateDecalMaterial(const std::string& key, const DecalMaterialInfo& decalMaterialInfo)
{
	return mModelManager->CreateDecalMaterial(key, decalMaterialInfo);
}
std::vector<std::shared_ptr<IStaticMesh>> fq::graphics::FQGraphics::GetStaticMeshes()
{
	return mModelManager->GetStaticMeshes();
}
std::vector<std::shared_ptr<ISkinnedMesh>> fq::graphics::FQGraphics::GetSkinnedMeshes()
{
	return mModelManager->GetSkinnedMeshes();
}
std::vector<std::shared_ptr<INodeHierarchy>> fq::graphics::FQGraphics::GetNodeHierarchies()
{
	return mModelManager->GetNodeHierarchies();
}
std::vector<std::shared_ptr<IAnimation>> fq::graphics::FQGraphics::GetAnimations()
{
	return mModelManager->GetAnimations();
}
std::vector<std::shared_ptr<IUVAnimation>> fq::graphics::FQGraphics::GetUVAnimations()
{
	return mModelManager->GetUVAnimations();
}
std::vector<std::shared_ptr<IMaterial>> fq::graphics::FQGraphics::GetMaterials()
{
	return mModelManager->GetMaterials();
}
std::vector<std::shared_ptr<IParticleMaterial>> fq::graphics::FQGraphics::GetParticleMaterials()
{
	return mModelManager->GetParticleMaterials();
}
std::vector<std::shared_ptr<IDecalMaterial>> fq::graphics::FQGraphics::GetDecalMaterials()
{
	return mModelManager->GetDecalMaterials();
}
std::shared_ptr<IStaticMesh> fq::graphics::FQGraphics::GetStaticMeshOrNull(std::string key)
{
	return mModelManager->GetStaticMeshOrNull(key);
}
std::shared_ptr<ISkinnedMesh> fq::graphics::FQGraphics::GetSkinnedMeshOrNull(std::string key)
{
	return mModelManager->GetSkinnedMeshOrNull(key);
}
std::shared_ptr<INodeHierarchy> fq::graphics::FQGraphics::GetNodeHierarchyOrNull(std::string key)
{
	return mModelManager->GetNodeHierarchyOrNull(key);
}
std::shared_ptr<IAnimation> fq::graphics::FQGraphics::GetAnimationOrNull(std::string key)
{
	return mModelManager->GetAnimationOrNull(key);
}
std::shared_ptr<IUVAnimation> fq::graphics::FQGraphics::GetUVAnimationOrNull(std::string key)
{
	return mModelManager->GetUVAnimationOrNull(key);
}
std::shared_ptr<IMaterial> fq::graphics::FQGraphics::GetMaterialOrNull(const std::string& key)
{
	return mModelManager->GetMaterialOrNull(key);
}
std::shared_ptr<IParticleMaterial> fq::graphics::FQGraphics::GetParticleMaterialOrNull(const std::string& key)
{
	return mModelManager->GetParticleMaterialOrNull(key);
}
std::shared_ptr<IDecalMaterial> fq::graphics::FQGraphics::GetDecalMaterialOrNull(const std::string& key)
{
	return mModelManager->GetDecalMaterialOrNull(key);
}

void fq::graphics::FQGraphics::DeleteStaticMesh(std::string key)
{
	mModelManager->DeleteStaticMesh(key);
}
void fq::graphics::FQGraphics::DeleteSkinnedMesh(std::string key)
{
	mModelManager->DeleteSkinnedMesh(key);
}
void fq::graphics::FQGraphics::DeleteNodeHierarchy(std::string key)
{
	mModelManager->DeleteNodeHierarchy(key);
}
void fq::graphics::FQGraphics::DeleteAnimation(std::string key)
{
	mModelManager->DeleteAnimation(key);
}
void fq::graphics::FQGraphics::DeleteUVAnimation(std::string key)
{
	mModelManager->DeleteUVAnimation(key);
}
void fq::graphics::FQGraphics::DeleteMaterial(const std::string& key)
{
	mModelManager->DeleteMaterial(key);
}
void fq::graphics::FQGraphics::DeleteParticleMaterial(const std::string& key)
{
	mModelManager->DeleteParticleMaterial(key);
}
void fq::graphics::FQGraphics::DeleteDecalMaterial(const std::string& key)
{
	mModelManager->DeleteDecalMaterial(key);
}

IStaticMeshObject* fq::graphics::FQGraphics::CreateStaticMeshObject(std::shared_ptr<IStaticMesh> staticMesh, std::vector<std::shared_ptr<IMaterial>> materials, const MeshObjectInfo& meshObjectInfo, const DirectX::SimpleMath::Matrix& transform)
{
	assert(staticMesh != nullptr);
	auto staticMeshObject = mObjectManager->CreateStaticMeshObject(staticMesh, materials, meshObjectInfo, transform);
	mCullingManager->CreateBoundingBoxOfStaticObject(staticMeshObject);
	return staticMeshObject;
}
ISkinnedMeshObject* fq::graphics::FQGraphics::CreateSkinnedMeshObject(std::shared_ptr<ISkinnedMesh> skinnedMesh, std::vector<std::shared_ptr<IMaterial>> materials, const MeshObjectInfo& meshObjectInfo, const DirectX::SimpleMath::Matrix& transform)
{
	assert(skinnedMesh != nullptr);
	return mObjectManager->CreateSkinnedMeshObject(skinnedMesh, materials, meshObjectInfo, transform);
}
ITerrainMeshObject* fq::graphics::FQGraphics::CreateTerrainMeshObject(std::shared_ptr<IStaticMesh> staticMesh, const DirectX::SimpleMath::Matrix& transform)
{
	assert(staticMesh != nullptr);
	return mObjectManager->CreateTerrainMeshObject(mDevice, staticMesh, transform);
}
IParticleObject* fq::graphics::FQGraphics::CreateParticleObject(std::shared_ptr<IParticleMaterial> iParticleMaterial, const ParticleInfo& particleInfo, const DirectX::SimpleMath::Matrix& transform)
{
	return mObjectManager->CreateParticleObject(mDevice, iParticleMaterial, particleInfo, transform);
}
IDecalObject* fq::graphics::FQGraphics::CreateDecalObject(std::shared_ptr<IDecalMaterial> iDecalMaterial, const DecalInfo& decalInfo, const DirectX::SimpleMath::Matrix& transform)
{
	return mObjectManager->CreateDecalObject(iDecalMaterial, decalInfo, transform);
}
ITrailObject* fq::graphics::FQGraphics::CreateTrailObject(std::shared_ptr<IParticleMaterial> iParticleMaterial, const TrailInfo& trailInfo, const DirectX::SimpleMath::Matrix& transform)
{
	return mObjectManager->CreateTrailObject(iParticleMaterial, trailInfo, transform);
}

void fq::graphics::FQGraphics::DeleteStaticMeshObject(IStaticMeshObject* staticMeshObject)
{
	mObjectManager->DeleteStaticMeshObject(staticMeshObject);
	mCullingManager->DeleteBoundingBoxOfStaticObject(staticMeshObject);
}
void fq::graphics::FQGraphics::DeleteSkinnedMeshObject(ISkinnedMeshObject* skinnedMeshObject)
{
	mObjectManager->DeleteSkinnedMeshObject(skinnedMeshObject);
}
void fq::graphics::FQGraphics::DeleteTerrainMeshObject(ITerrainMeshObject* meshObject)
{
	mObjectManager->DeleteTerrainMeshObject(meshObject);
}
void fq::graphics::FQGraphics::DeleteParticleObject(IParticleObject* particleObject)
{
	mObjectManager->DeleteParticleObject(particleObject);
}
void fq::graphics::FQGraphics::DeleteDecalObject(IDecalObject* decalObject)
{
	mObjectManager->DeleteDecalObject(decalObject);
}
void fq::graphics::FQGraphics::DeleteTrailObject(ITrailObject* trailObject)
{
	mObjectManager->DeleteTrailObject(trailObject);
}

void fq::graphics::FQGraphics::SetTerrainMeshObject(ITerrainMeshObject* meshObject, const TerrainMaterialInfo& material)
{
	mObjectManager->SetTerrainMeshObject(mDevice, meshObject, material);
}

std::set<IStaticMeshObject*> fq::graphics::FQGraphics::GetStaticMeshObjects() const
{
	return mObjectManager->GetStaticMeshObjects();
}

DebugInfo fq::graphics::FQGraphics::GetDebugInfo() const
{
	DebugInfo debugInfo{ 0, };

	std::set<IStaticMeshObject*> staticMeshesToRender = mObjectManager->GetStaticMeshObjects();
	std::set<ISkinnedMeshObject*> skinnedMeshesToRender = mObjectManager->GetSkinnedMeshObjects();

	std::set<IStaticMeshObject*>  cullingStaticMeshesToRender = mCullingManager->GetInFrustumStaticObjects(staticMeshesToRender);
	std::set<ISkinnedMeshObject*> cullingSkinnedMeshesToRender = mCullingManager->GetInFrustumSkinnedObjects(skinnedMeshesToRender);

	debugInfo.StaticMeshObjectCount = staticMeshesToRender.size();
	debugInfo.SkinnedMeshObjectCount = skinnedMeshesToRender.size();
	for (const auto& object : staticMeshesToRender)
	{
		const auto& meshData = object->GetStaticMesh()->GetMeshData();
		debugInfo.StaticMeshObjectVertexCount += meshData.Vertices.size();
		debugInfo.StaticMeshObjectPolygonCount += meshData.Indices.size() / 3;
	}
	for (const auto& object : skinnedMeshesToRender)
	{
		const auto& meshData = object->GetSkinnedMesh()->GetMeshData();
		debugInfo.SkinnedMeshObjectVertexCount += meshData.Vertices.size();
		debugInfo.SkinnedMeshObjectPolygonCount += meshData.Indices.size() / 3;
	}

	debugInfo.CullingStaticMeshObjectCount = cullingStaticMeshesToRender.size();
	debugInfo.CullingSkinnedMeshObjectCount = cullingSkinnedMeshesToRender.size();
	for (const auto& object : cullingStaticMeshesToRender)
	{
		const auto& meshData = object->GetStaticMesh()->GetMeshData();
		debugInfo.CullingStaticMeshObjectVertexCount += meshData.Vertices.size();
		debugInfo.CullingStaticMeshObjectPolygonCount += meshData.Indices.size() / 3;
	}
	for (const auto& object : cullingSkinnedMeshesToRender)
	{
		const auto& meshData = object->GetSkinnedMesh()->GetMeshData();
		debugInfo.CullingSkinnedMeshObjectVertexCount += meshData.Vertices.size();
		debugInfo.CullingSkinnedMeshObjectPolygonCount += meshData.Indices.size() / 3;
	}

	return debugInfo;
}

void FQGraphics::DeleteProbeObject(IProbeObject* probeObject)
{
	mObjectManager->DeleteProbeObject(probeObject);
}

fq::graphics::IProbeObject* FQGraphics::CreateProbeObject(std::shared_ptr<IStaticMesh> staticMesh, const DirectX::SimpleMath::Matrix& transform, int index)
{
	assert(staticMesh != nullptr);
	return mObjectManager->CreateProbeObject(staticMesh, transform, index);
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
void fq::graphics::FQGraphics::DrawSphereEx(const debug::SphereInfoEx& sphereInfoEx)
{
	mDebugDrawManager->Submit(sphereInfoEx);
}
void fq::graphics::FQGraphics::DrawRingEx(const debug::RingInfoEx& ringInfoEx)
{
	mDebugDrawManager->Submit(ringInfoEx);
}

void FQGraphics::DeleteSpriteAnimation(fq::graphics::ISpriteAnimationObject* spriteAniamtionObject)
{
	mUIManager->DeleteSpriteAniamtion(spriteAniamtionObject);
}

fq::graphics::ISpriteAnimationObject* FQGraphics::CreateSpriteAnimation(SpriteInfo spriteInfo)
{
	return mUIManager->CreateSpriteAniamtion(spriteInfo);
}

void FQGraphics::SetIsRenderObjects(bool isRenderObjects)
{
	mUIManager->SetIsRenderObjects(isRenderObjects);
	mIsRenderObjects = isRenderObjects;
}

void FQGraphics::DeleteText(fq::graphics::ITextObject* textObject)
{
	mUIManager->DeleteText(textObject);
}

ITextObject* FQGraphics::CreateText(TextInfo textInfo)
{
	return mUIManager->CreateText(textInfo);
}

void FQGraphics::SetLightProbeIntensity(float intensity)
{
	mLightProbeManager->SetIntensity(intensity);
}

void FQGraphics::SetIsDrawDebugLightProbe(bool isDrawDebugLightProbe)
{
	mLightProbeManager->SetIsDrawDebugLightProbe(isDrawDebugLightProbe);
}
void FQGraphics::SetIsRenderDebug(bool bIsRenderDebug)
{
	mDebugDrawManager->SetIsRenderDebug(bIsRenderDebug);
}

void FQGraphics::SetLightProbe(int index, const DirectX::SimpleMath::Vector3& position)
{
	mLightProbeManager->SetLightProbe(index, position);
}

void FQGraphics::SetPipelineType(EPipelineType pipelineType)
{
	mRenderManager->Initialize(mDevice, mJobManager, mCameraManager, mLightManager, mResourceManager, mDebugDrawManager, mParticleManager, mObjectManager, mLightProbeManager, mDevice->GetWidth(), mDevice->GetHeight(), pipelineType);
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