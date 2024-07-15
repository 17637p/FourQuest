#pragma once

#include "IFQGraphics.h"

#include <map>
#include <memory>
#include <string>
#include <filesystem>

namespace fq::graphics
{
	class FQGraphics : public IFQGraphics
	{
	public:
		FQGraphics();
		virtual ~FQGraphics();

		/// Framework
		virtual bool Initialize(const HWND hWnd, const unsigned short width, const unsigned short height, EPipelineType pipelineType) override;
		virtual bool Update(float deltaTime) override;

		virtual bool BeginRender() override;
		virtual bool Render() override;
		virtual bool EndRender() override;

		virtual bool Finalize() override;

		/// 창 크기 변환
		virtual bool SetWindowSize(const unsigned short width, const unsigned short height) override; // 툴의 에디터를 포함한 전체 윈도우 영역
		virtual bool SetViewportSize(const unsigned short width, const unsigned short height) override; // 툴에서 씬을 그리는 영역

		// Model Data Helper
		virtual void WriteModel(const std::string& path, const fq::common::Model& modelData) override;
		virtual fq::common::Model ReadModel(const std::string& path) override;
		virtual fq::common::Model ConvertModel(const std::string& fbxFile) override;

		// Model Resource Control
		virtual const fq::common::Model& CreateModelResource(const std::string& path, std::filesystem::path textureBasePath = "") override;
		virtual bool TryCreateModelResource(const std::string& path, std::filesystem::path textureBasePath, fq::common::Model* outDataOrNull) override;
		virtual const fq::common::Model& GetModel(const std::string& path) override;
		virtual void DeleteModelResource(const std::string& path) override;

		virtual std::shared_ptr<INodeHierarchy> GetNodeHierarchyByModelPathOrNull(std::string modelPath) override;
		virtual std::shared_ptr<ISkinnedMesh> GetSkinnedMeshByModelPathOrNull(std::string modelPath, std::string meshName) override;
		virtual std::shared_ptr<IStaticMesh> GetStaticMeshByModelPathOrNull(std::string modelPath, std::string meshName) override;
		virtual std::shared_ptr<IMaterial> GetMaterialByModelPathOrNull(std::string modelPath, std::string materialName) override;
		virtual std::shared_ptr<IAnimation> GetAnimationByModelPathOrNull(std::string modelPath, std::string animationName) override;

		// Render Resource Control
		virtual std::shared_ptr<IStaticMesh> CreateStaticMesh(const fq::common::Mesh& meshData) override;
		virtual std::shared_ptr<ISkinnedMesh> CreateSkinnedMesh(const fq::common::Mesh& meshData) override;
		virtual std::shared_ptr<INodeHierarchy> CreateNodeHierarchy(const fq::common::Model& model) override;
		virtual std::shared_ptr<INodeHierarchy> CreateNodeHierarchy(const std::vector<fq::common::Node> nodes) override;
		virtual std::shared_ptr<IAnimation> CreateAnimation(const fq::common::AnimationClip& animationClip) override;
		virtual std::shared_ptr<IMaterial> CreateMaterial(const MaterialInfo& materialInfo) override;
		virtual std::shared_ptr<IParticleMaterial> CreateParticleMaterial(const ParticleMaterialInfo& materialInfo) override;
		virtual std::shared_ptr<IDecalMaterial> CreateDecalMaterial(const DecalMaterialInfo& decalMaterialInfo) override;

		virtual std::shared_ptr<IStaticMesh> CreateStaticMesh(std::string key, const fq::common::Mesh& meshData) override;
		virtual std::shared_ptr<ISkinnedMesh> CreateSkinnedMesh(std::string key, const fq::common::Mesh& meshData) override;
		virtual std::shared_ptr<INodeHierarchy> CreateNodeHierarchy(std::string key, const fq::common::Model& model) override;
		virtual std::shared_ptr<INodeHierarchy> CreateNodeHierarchy(std::string key, const std::vector<fq::common::Node> nodes) override;
		virtual std::shared_ptr<IAnimation> CreateAnimation(std::string key, const fq::common::AnimationClip& animationClip) override;
		virtual std::shared_ptr<IMaterial> CreateMaterial(const std::string& key, const MaterialInfo& materialInfo) override;
		virtual std::shared_ptr<IParticleMaterial> CreateParticleMaterial(const std::string& key, const ParticleMaterialInfo& materialInfo) override;
		virtual std::shared_ptr<IDecalMaterial> CreateDecalMaterial(const std::string& key, const DecalMaterialInfo& decalMaterialInfo) override;

		virtual std::vector<std::shared_ptr<IStaticMesh>> GetStaticMeshes() override;
		virtual std::vector<std::shared_ptr<ISkinnedMesh>> GetSkinnedMeshes() override;
		virtual std::vector<std::shared_ptr<INodeHierarchy>> GetNodeHierarchies() override;
		virtual std::vector<std::shared_ptr<IAnimation>> GetAnimations() override;
		virtual std::vector<std::shared_ptr<IMaterial>> GetMaterials() override;
		virtual std::vector<std::shared_ptr<IParticleMaterial>> GetParticleMaterials() override;
		virtual std::vector<std::shared_ptr<IDecalMaterial>> GetDecalMaterials() override;

		virtual std::shared_ptr<IStaticMesh> GetStaticMeshOrNull(std::string key) override;
		virtual std::shared_ptr<ISkinnedMesh> GetSkinnedMeshOrNull(std::string key) override;
		virtual std::shared_ptr<INodeHierarchy> GetNodeHierarchyOrNull(std::string key) override;
		virtual std::shared_ptr<IAnimation> GetAnimationOrNull(std::string key) override;
		virtual std::shared_ptr<IMaterial> GetMaterialOrNull(const std::string& key) override;
		virtual std::shared_ptr<IParticleMaterial> GetParticleMaterialOrNull(const std::string& key) override;
		virtual std::shared_ptr<IDecalMaterial> GetDecalMaterialOrNull(const std::string& key) override;

		virtual void DeleteStaticMesh(std::string key) override;
		virtual void DeleteSkinnedMesh(std::string key) override;
		virtual void DeleteNodeHierarchy(std::string key) override;
		virtual void DeleteAnimation(std::string key) override;
		virtual void DeleteMaterial(const std::string& key) override;
		virtual void DeleteParticleMaterial(const std::string& key) override;
		virtual void DeleteDecalMaterial(const std::string& key) override;

		// RenderObject
		virtual IStaticMeshObject* CreateStaticMeshObject(std::shared_ptr<IStaticMesh> staticMesh, std::vector<std::shared_ptr<IMaterial>> materials, const MeshObjectInfo& meshObjectInfo, const DirectX::SimpleMath::Matrix& transform) override;
		virtual ISkinnedMeshObject* CreateSkinnedMeshObject(std::shared_ptr<ISkinnedMesh> skinnedMesh, std::vector<std::shared_ptr<IMaterial>> materials, const MeshObjectInfo& meshObjectInfo, const DirectX::SimpleMath::Matrix& transform) override;
		virtual ITerrainMeshObject* CreateTerrainMeshObject(std::shared_ptr<IStaticMesh> staticMesh, const DirectX::SimpleMath::Matrix& transform) override;
		virtual IParticleObject* CreateParticleObject(std::shared_ptr<IParticleMaterial> iParticleMaterial, const ParticleInfo& particleInfo, const DirectX::SimpleMath::Matrix& transform) override;
		virtual IDecalObject* CreateDecalObject(std::shared_ptr<IDecalMaterial> iDecalMaterial, const DecalInfo& decalInfo, const DirectX::SimpleMath::Matrix& transform) override;
		virtual ITrailObject* CreateTrailObject(std::shared_ptr<IParticleMaterial> iParticleMaterial, const TrailInfo& trailInfo, const DirectX::SimpleMath::Matrix& transform) override;
		virtual IProbeObject* CreateProbeObject(std::shared_ptr<IStaticMesh> staticMesh, const DirectX::SimpleMath::Matrix& transform, int index) override;

		virtual void DeleteStaticMeshObject(IStaticMeshObject* staticMeshObject) override;
		virtual void DeleteSkinnedMeshObject(ISkinnedMeshObject* skinnedMeshObject) override;
		virtual void DeleteTerrainMeshObject(ITerrainMeshObject* meshObject) override;
		virtual void DeleteParticleObject(IParticleObject* particleObject) override;
		virtual void DeleteDecalObject(IDecalObject* decalObject) override;
		virtual void DeleteTrailObject(ITrailObject* trailObject) override;
		virtual void DeleteProbeObject(IProbeObject* probeObject) override;

		virtual void SetTerrainMeshObject(ITerrainMeshObject* meshObject, const TerrainMaterialInfo& material) override;

		/// Primitive
		// Debug Draw
		virtual void DrawSphere(const debug::SphereInfo& sphereInfo) override;
		virtual void DrawBox(const debug::AABBInfo& aabbInfo) override;
		virtual void DrawOBB(const debug::OBBInfo& obbInfo) override;
		virtual void DrawFrustum(const debug::FrustumInfo& frustumInfo) override;
		virtual void DrawGrid(const debug::GridInfo& gridInfo) override;
		virtual void DrawRing(const debug::RingInfo& ringInfo) override;
		virtual void DrawRay(const debug::RayInfo& rayInfo) override;
		virtual void DrawPolygon(const debug::PolygonInfo& polygonInfo) override;
		virtual void DrawSphereEx(const debug::SphereInfoEx& sphereInfoEx) override;
		virtual void DrawRingEx(const debug::RingInfoEx& ringInfoEx) override;

		/// Option (그래픽 옵션 On/Off, 불가능하면 선택 못하게 하는 등 이제 그런 게 필요하지 않을까)
		virtual void SetPipelineType(EPipelineType pipelineType) override;
		virtual void SetIsDrawDebugLightProbe(bool isDrawDebugLightProbe) override;

		/// UI
		virtual void SetDefaultFontSize(const unsigned short fontSize) override;
		virtual void SetDefaultFontColor(const DirectX::SimpleMath::Color& color) override;
		virtual void SetDefaultFont(const std::wstring& path) override;
		virtual void AddFont(const std::wstring& path) override;
		virtual void DrawText(const std::wstring& text, const DirectX::SimpleMath::Rectangle& drawRect, unsigned short fontSize /*= 50*/, const std::wstring& fontPath /*= L"Verdana"*/, const DirectX::SimpleMath::Color& color /*= { 1, 0, 0, 1 }*/) override;
		virtual void DeleteFont(const std::wstring& path) override;
		// image
		virtual IImageObject* CreateImageObject(const UIInfo& uiInfo) override;
		virtual void DeleteImageObject(IImageObject* imageObject) override;

		/// SkyBox
		virtual void SetSkyBox(const std::wstring& path, bool bUseIBL = false, float envScale = 1.f) override;
		virtual void SetIBLTexture(const std::wstring& diffuse, const std::wstring& specular, const std::wstring& brdfLUT) override;

		/// Light
		// Todo: 동적 라이트, static 라이트 구별을 하면 좋을 것 같다.
		virtual void AddLight(const unsigned int id, const LightInfo& lightInfo) override;
		virtual void UpdateLight(const unsigned int id, const LightInfo& lightInfo) override;
		virtual void DeleteLight(const unsigned int id) override;

		virtual void UseShadow(const unsigned int id, bool bUseShadow);

		// Light Probe
		virtual int AddLightProbe(const DirectX::SimpleMath::Vector3& position) override;
		virtual void DeleteLightProbe(int index) override;
		virtual void SetLightProbe(int index, const DirectX::SimpleMath::Vector3& position) override;

		virtual void BakeLightProbe() override;
		virtual void SaveLightProbes(const std::string& fileName) override;
		virtual void LoadLightProbes(const std::string& fileName) override;

		virtual unsigned short AddCubeProbe(const DirectX::SimpleMath::Vector3& position) override;
		virtual void DeleteCubeProbe(unsigned short index) override;
		virtual void SaveCubeProbeTexture(const unsigned short width, const unsigned short height) override;

		virtual void SetLightProbeIntensity(float intensity) override;

		/// Camera
		virtual void SetCamera(const CameraInfo& cameraInfo) override;
		virtual void UpdateCamera(const fq::common::Transform& cameraTransform) override;
		virtual void UpdateColCamera(const fq::common::Transform& cameraTransform) override;

		/// Picking
		virtual void* GetPickingObject(const short mouseX, const short mouseY) override;

		// PostProcessing
		virtual void  SetPostProcessingInfo(const PostProcessingInfo& info) override;
		virtual const PostProcessingInfo& GetPostProcessingInfo() const override;

		/// For IMGUI(D3D11)
		ID3D11Device* GetDivice() override;
		ID3D11DeviceContext* GetDeviceContext() override;
		ID3D11ShaderResourceView* GetSRV() override;

		/// spdlog 설정
		virtual FQ_GRAPHICS std::shared_ptr<spdlog::logger> SetUpLogger(std::vector<spdlog::sink_ptr> sinks) override;

	private:
		std::shared_ptr<class D3D11Device> mDevice;
		std::shared_ptr<class D3D11ResourceManager> mResourceManager;
		std::shared_ptr<class D3D11ModelManager> mModelManager;
		std::shared_ptr<class D3D11ObjectManager> mObjectManager;
		std::shared_ptr<class D3D11JobManager> mJobManager;
		std::shared_ptr<class D3D11RenderManager> mRenderManager;
		std::shared_ptr<class D3D11CameraManager> mCameraManager;
		std::shared_ptr<class D3D11LightManager> mLightManager;
		std::shared_ptr<class D3D11DebugDrawManager> mDebugDrawManager;
		std::shared_ptr<class D3D11ParticleManager> mParticleManager;
		std::shared_ptr<class D3D11LightProbeManager> mLightProbeManager;
		std::shared_ptr<class RenderResourceManager> mRenderResourceManager;

		std::shared_ptr<class D3D11PickingManager> mPickingManager;
		std::shared_ptr<class D3D11CullingManager> mCullingManager;
		std::shared_ptr<class UIManager> mUIManager;

		std::shared_ptr<class D3D11PostProcessingManager> mPostProcessingManager;
	};
}

