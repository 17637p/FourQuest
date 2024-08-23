#pragma once

#include <windows.h>
#include <memory>
#include <string>
#include <filesystem>
#include <spdlog/spdlog.h>
#include <spdlog/logger.h>

#include "d3d11.h"
#include "EPipelineType.h"

#include "../FQCommon/IFQRenderObject.h"
#include "../FQCommon/IFQRenderResource.h"
#include "../FQCommon/FQCommonLoader.h"
#include "../FQCommon/FQCommonGraphics.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib") 
//#include <directxtk/SimpleMath.h>

#ifdef FQ_GRAPHICS_EXPORT
#define FQ_GRAPHICS __declspec(dllexport)
#else
#define FQ_GRAPHICS __declspec(dllimport)
#endif

#ifdef __cplusplus
extern "C" {
#endif

	namespace fq::common { struct Transform; }

	namespace fq::graphics
	{
		/// <summary>
		/// FPGraphics 인터페이스
		/// dll로 만들면 빌드 시간을 줄이고 종속성이 줄어들어 유지 보수가 편해진다.
		/// 
		/// 2024.05.03 
		/// </summary>
		class IFQGraphics
		{
		public:
			virtual ~IFQGraphics() {};

			/// Framework
			virtual FQ_GRAPHICS bool Initialize(const HWND hWnd, const unsigned short width, const unsigned short height, EPipelineType pipelineType = EPipelineType::Deferred) abstract;
			virtual FQ_GRAPHICS bool Update(float deltaTime) abstract;

			virtual FQ_GRAPHICS bool BeginRender() abstract;
			virtual FQ_GRAPHICS bool Render() abstract;
			virtual FQ_GRAPHICS bool EndRender() abstract;

			virtual FQ_GRAPHICS bool Finalize() abstract;

			/// 창 크기 변환
			virtual FQ_GRAPHICS bool SetWindowSize(const unsigned short width, const unsigned short height) abstract; // 툴의 에디터를 포함한 전체 윈도우 영역
			virtual FQ_GRAPHICS bool SetViewportSize(const unsigned short width, const unsigned short height) abstract; // 툴에서 씬을 그리는 영역

			// Model Data Helper
			virtual FQ_GRAPHICS fq::common::Model ConvertModel(const std::string& fbxFile) abstract;
			virtual FQ_GRAPHICS void WriteModel(const std::string& path, const fq::common::Model& modelData) abstract;
			virtual FQ_GRAPHICS fq::common::Model ReadModel(const std::string& path) abstract;
			virtual FQ_GRAPHICS void WriteAnimation(const std::string& path, const fq::common::AnimationClip& modelData) abstract;
			virtual FQ_GRAPHICS fq::common::AnimationClip ReadAnimation(const std::string& path) abstract;
			virtual FQ_GRAPHICS void WriteUVAnimation(const std::string& path, const fq::common::UVAnimationClip& modelData) abstract;
			virtual FQ_GRAPHICS fq::common::UVAnimationClip ReadUVAnimation(const std::string& path) abstract;
			virtual FQ_GRAPHICS void WriteNodeHierarchy(const std::string& path, const std::vector<fq::common::Node>& nodeHierarchy) abstract;
			virtual FQ_GRAPHICS void WriteNodeHierarchy(const std::string& path, const fq::common::Model& modelData) abstract;
			virtual FQ_GRAPHICS std::vector<fq::common::Node> ReadNodeHierarchy(const std::string& path) abstract;
			virtual FQ_GRAPHICS void WriteMaterialInfo(const std::string& path, const MaterialInfo& materialInfo) abstract;
			virtual FQ_GRAPHICS MaterialInfo ReadMaterialInfo(const std::string& path) abstract;

			// Model Resource Control
			virtual FQ_GRAPHICS const fq::common::Model& CreateModelResource(unsigned int key, const std::string& path, std::filesystem::path textureBasePath = "") abstract;
			virtual FQ_GRAPHICS void CreateModelResource(unsigned int key, const fq::common::Model& modelData, std::filesystem::path textureBasePath = "") abstract;
			virtual FQ_GRAPHICS bool TryCreateModelResource(unsigned int key, const std::string& path, std::filesystem::path textureBasePath = "", fq::common::Model* outDataOrNull = nullptr) abstract;
			virtual FQ_GRAPHICS const fq::common::Model& GetModel(unsigned int key) abstract;
			virtual FQ_GRAPHICS void DeleteModelResource(unsigned int key) abstract;

			virtual FQ_GRAPHICS std::shared_ptr<INodeHierarchy> GetNodeHierarchyByModelPathOrNull(unsigned int key) abstract;
			virtual FQ_GRAPHICS std::shared_ptr<IStaticMesh> GetStaticMeshByModelPathOrNull(unsigned int key, std::string meshName) abstract;
			virtual FQ_GRAPHICS std::shared_ptr<ISkinnedMesh> GetSkinnedMeshByModelPathOrNull(unsigned int key, std::string meshName) abstract;
			virtual FQ_GRAPHICS std::shared_ptr<IMaterial> GetMaterialByModelPathOrNull(unsigned int key, std::string materialName) abstract;
			virtual FQ_GRAPHICS std::shared_ptr<IAnimation> GetAnimationByModelPathOrNull(unsigned int key, std::string animationName) abstract;

			// Render Resource Control
			virtual FQ_GRAPHICS std::shared_ptr<IStaticMesh> CreateStaticMesh(const fq::common::Mesh& meshData) abstract;
			virtual FQ_GRAPHICS std::shared_ptr<ISkinnedMesh> CreateSkinnedMesh(const fq::common::Mesh& meshData) abstract;
			virtual FQ_GRAPHICS std::shared_ptr<INodeHierarchy> CreateNodeHierarchy(const fq::common::Model& model) abstract;
			virtual FQ_GRAPHICS std::shared_ptr<INodeHierarchy> CreateNodeHierarchy(const std::vector<fq::common::Node> nodes) abstract;
			virtual FQ_GRAPHICS std::shared_ptr<IAnimation> CreateAnimation(const fq::common::AnimationClip& animationClip) abstract;
			virtual FQ_GRAPHICS std::shared_ptr<IUVAnimation> CreateUVAnimation(const fq::common::UVAnimationClip& animationClip) abstract;
			virtual FQ_GRAPHICS std::shared_ptr<IMaterial> CreateMaterial(const MaterialInfo& materialInfo) abstract;
			virtual FQ_GRAPHICS std::shared_ptr<IParticleMaterial> CreateParticleMaterial(const ParticleMaterialInfo& materialInfo) abstract;
			virtual FQ_GRAPHICS std::shared_ptr<IDecalMaterial> CreateDecalMaterial(const DecalMaterialInfo& decalMaterialInfo) abstract;

			virtual FQ_GRAPHICS std::shared_ptr<IStaticMesh> CreateStaticMesh(std::string key, const fq::common::Mesh& meshData) abstract;
			virtual FQ_GRAPHICS std::shared_ptr<ISkinnedMesh> CreateSkinnedMesh(std::string key, const fq::common::Mesh& meshData) abstract;
			virtual FQ_GRAPHICS std::shared_ptr<INodeHierarchy> CreateNodeHierarchy(std::string key, const fq::common::Model& model) abstract;
			virtual FQ_GRAPHICS std::shared_ptr<INodeHierarchy> CreateNodeHierarchy(std::string key, const std::vector<fq::common::Node> nodes) abstract;
			virtual FQ_GRAPHICS std::shared_ptr<IAnimation> CreateAnimation(std::string key, const fq::common::AnimationClip& animationClip) abstract;
			virtual FQ_GRAPHICS std::shared_ptr<IUVAnimation> CreateUVAnimation(std::string key, const fq::common::UVAnimationClip& animationClip) abstract;
			virtual FQ_GRAPHICS std::shared_ptr<IMaterial> CreateMaterial(const std::string& key, const MaterialInfo& materialInfo) abstract;
			virtual FQ_GRAPHICS std::shared_ptr<IParticleMaterial> CreateParticleMaterial(const std::string& key, const ParticleMaterialInfo& materialInfo) abstract;
			virtual FQ_GRAPHICS std::shared_ptr<IDecalMaterial> CreateDecalMaterial(const std::string& key, const DecalMaterialInfo& decalMaterialInfo) abstract;

			virtual FQ_GRAPHICS std::vector<std::shared_ptr<IStaticMesh>> GetStaticMeshes() abstract;
			virtual FQ_GRAPHICS std::vector<std::shared_ptr<ISkinnedMesh>> GetSkinnedMeshes() abstract;
			virtual FQ_GRAPHICS std::vector<std::shared_ptr<INodeHierarchy>> GetNodeHierarchies() abstract;
			virtual FQ_GRAPHICS std::vector<std::shared_ptr<IAnimation>> GetAnimations() abstract;
			virtual FQ_GRAPHICS std::vector<std::shared_ptr<IUVAnimation>> GetUVAnimations() abstract;
			virtual FQ_GRAPHICS std::vector<std::shared_ptr<IMaterial>> GetMaterials() abstract;
			virtual FQ_GRAPHICS std::vector<std::shared_ptr<IParticleMaterial>> GetParticleMaterials() abstract;
			virtual FQ_GRAPHICS std::vector<std::shared_ptr<IDecalMaterial>> GetDecalMaterials() abstract;

			virtual FQ_GRAPHICS std::shared_ptr<IStaticMesh> GetStaticMeshOrNull(std::string key) abstract;
			virtual FQ_GRAPHICS std::shared_ptr<ISkinnedMesh> GetSkinnedMeshOrNull(std::string key) abstract;
			virtual FQ_GRAPHICS std::shared_ptr<INodeHierarchy> GetNodeHierarchyOrNull(std::string key) abstract;
			virtual FQ_GRAPHICS std::shared_ptr<IAnimation> GetAnimationOrNull(std::string key) abstract;
			virtual FQ_GRAPHICS std::shared_ptr<IUVAnimation> GetUVAnimationOrNull(std::string key) abstract;
			virtual FQ_GRAPHICS std::shared_ptr<IMaterial> GetMaterialOrNull(const std::string& key) abstract;
			virtual FQ_GRAPHICS std::shared_ptr<IParticleMaterial> GetParticleMaterialOrNull(const std::string& key) abstract;
			virtual FQ_GRAPHICS std::shared_ptr<IDecalMaterial> GetDecalMaterialOrNull(const std::string& key) abstract;

			virtual FQ_GRAPHICS void DeleteStaticMesh(std::string key) abstract;
			virtual FQ_GRAPHICS void DeleteSkinnedMesh(std::string key) abstract;
			virtual FQ_GRAPHICS void DeleteNodeHierarchy(std::string key) abstract;
			virtual FQ_GRAPHICS void DeleteAnimation(std::string key) abstract;
			virtual FQ_GRAPHICS void DeleteUVAnimation(std::string key) abstract;
			virtual FQ_GRAPHICS void DeleteMaterial(const std::string& key) abstract;
			virtual FQ_GRAPHICS void DeleteParticleMaterial(const std::string& key) abstract;
			virtual FQ_GRAPHICS void DeleteDecalMaterial(const std::string& key) abstract;

			// RenderObject
			virtual FQ_GRAPHICS IStaticMeshObject* CreateStaticMeshObject(std::shared_ptr<IStaticMesh> staticMesh, std::vector<std::shared_ptr<IMaterial>> materials, const MeshObjectInfo& meshObjectInfo, const DirectX::SimpleMath::Matrix& transform) abstract;
			virtual FQ_GRAPHICS ISkinnedMeshObject* CreateSkinnedMeshObject(std::shared_ptr<ISkinnedMesh> skinnedMesh, std::vector<std::shared_ptr<IMaterial>> materials, const MeshObjectInfo& meshObjectInfo, const DirectX::SimpleMath::Matrix& transform) abstract;
			virtual FQ_GRAPHICS ITerrainMeshObject* CreateTerrainMeshObject(std::shared_ptr<IStaticMesh> staticMesh, const DirectX::SimpleMath::Matrix& transform) abstract;
			virtual FQ_GRAPHICS IParticleObject* CreateParticleObject(std::shared_ptr<IParticleMaterial> iParticleMaterial, const ParticleInfo& particleInfo, const DirectX::SimpleMath::Matrix& transform) abstract;
			virtual FQ_GRAPHICS IDecalObject* CreateDecalObject(std::shared_ptr<IDecalMaterial> iDecalMaterial, const DecalInfo& decalInfo, const DirectX::SimpleMath::Matrix& transform) abstract;
			virtual FQ_GRAPHICS ITrailObject* CreateTrailObject(std::shared_ptr<IParticleMaterial> iParticleMaterial, const TrailInfo& trailInfo, const DirectX::SimpleMath::Matrix& transform) abstract;
			virtual FQ_GRAPHICS IProbeObject* CreateProbeObject(std::shared_ptr<IStaticMesh> staticMesh, const DirectX::SimpleMath::Matrix& transform, int index) abstract;

			virtual FQ_GRAPHICS void DeleteStaticMeshObject(IStaticMeshObject* staticMeshObject) abstract;
			virtual FQ_GRAPHICS void DeleteSkinnedMeshObject(ISkinnedMeshObject* skinnedMeshObject) abstract;
			virtual FQ_GRAPHICS void DeleteTerrainMeshObject(ITerrainMeshObject* meshObject) abstract;
			virtual FQ_GRAPHICS void DeleteParticleObject(IParticleObject* particleObject) abstract;
			virtual FQ_GRAPHICS void DeleteDecalObject(IDecalObject* decalObject) abstract;
			virtual FQ_GRAPHICS void DeleteTrailObject(ITrailObject* trailObject) abstract;
			virtual FQ_GRAPHICS void DeleteProbeObject(IProbeObject* probeObject) abstract;

			virtual FQ_GRAPHICS void SetTerrainMeshObject(ITerrainMeshObject* meshObject, const TerrainMaterialInfo& material) abstract;

			virtual FQ_GRAPHICS std::set<IStaticMeshObject*> GetStaticMeshObjects() const abstract;

			// staticMeshObject getter/setter

			/// Primitive
			// Debug Draw
			virtual FQ_GRAPHICS void DrawSphere(const debug::SphereInfo& sphereInfo) abstract;
			virtual FQ_GRAPHICS void DrawBox(const debug::AABBInfo& aabbInfo) abstract;
			virtual FQ_GRAPHICS void DrawOBB(const debug::OBBInfo& obbInfo) abstract;
			virtual FQ_GRAPHICS void DrawFrustum(const debug::FrustumInfo& frustumInfo) abstract;
			virtual FQ_GRAPHICS void DrawGrid(const debug::GridInfo& gridInfo) abstract;
			virtual FQ_GRAPHICS void DrawRing(const debug::RingInfo& ringInfo) abstract;
			virtual FQ_GRAPHICS void DrawRay(const debug::RayInfo& rayInfo) abstract;
			virtual FQ_GRAPHICS void DrawPolygon(const debug::PolygonInfo& polygonInfo) abstract;
			virtual FQ_GRAPHICS void DrawSphereEx(const debug::SphereInfoEx& sphereInfoEx) abstract;
			virtual FQ_GRAPHICS void DrawRingEx(const debug::RingInfoEx& ringInfoEx) abstract;

			/// Option (그래픽 옵션 On/Off, 불가능하면 선택 못하게 하는 등 이제 그런 게 필요하지 않을까)
			virtual FQ_GRAPHICS void SetPipelineType(EPipelineType pipelineType) abstract;
			virtual FQ_GRAPHICS void SetIsDrawDebugLightProbe(bool isDrawDebugLightProbe) abstract;
			virtual FQ_GRAPHICS void SetIsRenderDebug(bool bIsRenderDebug) abstract;
			virtual FQ_GRAPHICS void SetIsRenderObjects(bool isRenderObjects) abstract;

			/// UI
			// text
			virtual FQ_GRAPHICS void SetDefaultFontSize(const unsigned short fontSize) abstract;
			virtual FQ_GRAPHICS void SetDefaultFontColor(const DirectX::SimpleMath::Color& color) abstract;
			virtual FQ_GRAPHICS void SetDefaultFont(const std::wstring& path) abstract;
			virtual FQ_GRAPHICS void AddFont(const std::wstring& path) abstract; // ttf
			virtual FQ_GRAPHICS ITextObject* CreateText(TextInfo textInfo) abstract;
			virtual FQ_GRAPHICS void DeleteText(fq::graphics::ITextObject* textObject) abstract;
			virtual FQ_GRAPHICS void DeleteFont(const std::wstring& path) abstract;
			// image
			virtual FQ_GRAPHICS IImageObject* CreateImageObject(const UIInfo& uiInfo) abstract;
			virtual FQ_GRAPHICS void DeleteImageObject(IImageObject* imageObject) abstract;

			/// SkyBox
			virtual FQ_GRAPHICS void SetSkyBox(const std::wstring& path, bool bUseIBL = false, float envScale = 1.f) abstract;
			virtual FQ_GRAPHICS void SetIBLTexture(const std::wstring& diffuse, const std::wstring& specular, const std::wstring& brdfLUT) abstract;

			/// Light
			virtual FQ_GRAPHICS void AddLight(const unsigned int id, const LightInfo& lightInfo) abstract;
			virtual FQ_GRAPHICS void UpdateLight(const unsigned int id, const LightInfo& lightInfo) abstract;
			virtual FQ_GRAPHICS void DeleteLight(const unsigned int id) abstract;

			virtual FQ_GRAPHICS void UseShadow(const unsigned int id, bool bUseShadow) abstract;
			virtual FQ_GRAPHICS void SetLightMapTexture(const std::vector<std::filesystem::path>& paths) abstract;
			virtual FQ_GRAPHICS void SetLightMapDirectionTexture(const std::vector<std::filesystem::path>& paths) abstract;

			// Light Probe
			virtual FQ_GRAPHICS int AddLightProbe(const DirectX::SimpleMath::Vector3& position) abstract;
			virtual FQ_GRAPHICS void DeleteLightProbe(int index) abstract;
			virtual FQ_GRAPHICS void SetLightProbe(int index, const DirectX::SimpleMath::Vector3& position) abstract;

			virtual FQ_GRAPHICS void BakeLightProbe(bool isAll) abstract;
			virtual FQ_GRAPHICS void SaveLightProbes(const std::string& fileName) abstract;
			virtual FQ_GRAPHICS void LoadLightProbes(const std::string& fileName) abstract;

			virtual FQ_GRAPHICS unsigned short AddCubeProbe(const DirectX::SimpleMath::Vector3& position) abstract;
			virtual FQ_GRAPHICS void SaveCubeProbeTexture(bool isAll, const unsigned short width, const unsigned short height) abstract;
			virtual FQ_GRAPHICS void DeleteCubeProbe(unsigned short index) abstract;

			virtual FQ_GRAPHICS void SetLightProbeIntensity(float intensity) abstract;

			/// Camera
			virtual FQ_GRAPHICS void SetCamera(const CameraInfo& cameraInfo) abstract;
			virtual FQ_GRAPHICS void UpdateCamera(const fq::common::Transform& cameraTransform) abstract;
			virtual FQ_GRAPHICS void UpdateColCamera(const fq::common::Transform& cameraTransform) abstract;

			/// Picking
			virtual FQ_GRAPHICS void* GetPickingObject(const short mouseX, const short mouseY) abstract;

			// PostProcessing
			virtual FQ_GRAPHICS bool GetIsUsePostProcessing() const abstract;
			virtual FQ_GRAPHICS void SetIsUsePostProcessing(bool bUsePostProcessing) abstract;
			virtual FQ_GRAPHICS void  SetPostProcessingInfo(const PostProcessingInfo& info) abstract;
			virtual FQ_GRAPHICS const PostProcessingInfo& GetPostProcessingInfo() const abstract;

			/// For IMGUI(D3D11)
			virtual FQ_GRAPHICS ID3D11Device* GetDivice() abstract;
			virtual FQ_GRAPHICS ID3D11DeviceContext* GetDeviceContext() abstract;
			virtual FQ_GRAPHICS ID3D11ShaderResourceView* GetSRV() abstract;

			/// Log
			virtual FQ_GRAPHICS std::shared_ptr<spdlog::logger> SetUpLogger(std::vector<spdlog::sink_ptr> sinks) abstract;
		};

		class EngineExporter
		{
		public:
			FQ_GRAPHICS IFQGraphics* GetEngine();
			FQ_GRAPHICS void DeleteEngine(IFQGraphics* engine);
		};
	}

#ifdef __cplusplus
}
#endif