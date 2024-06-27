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
		/// FPGraphics �������̽�
		/// dll�� ����� ���� �ð��� ���̰� ���Ӽ��� �پ��� ���� ������ ��������.
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

			/// â ũ�� ��ȯ
			virtual FQ_GRAPHICS bool SetWindowSize(const unsigned short width, const unsigned short height) abstract; // ���� �����͸� ������ ��ü ������ ����
			virtual FQ_GRAPHICS bool SetViewportSize(const unsigned short width, const unsigned short height) abstract; // ������ ���� �׸��� ����

			// ���� ������Ʈ & Animation
			virtual FQ_GRAPHICS void WriteModel(std::string path, const fq::common::Model& modelData) abstract;
			virtual FQ_GRAPHICS void ConvertModel(std::string fbxFile, std::string path) abstract;
			virtual FQ_GRAPHICS const fq::common::Model& CreateModel(std::string path, std::filesystem::path textureBasePath = "") abstract;
			virtual FQ_GRAPHICS const fq::common::Model& GetModel(std::string path) abstract;
			virtual FQ_GRAPHICS void DeleteModel(std::string path) abstract;
			virtual FQ_GRAPHICS std::set<std::shared_ptr<IMaterial>> GetMaterials() const abstract;

			virtual FQ_GRAPHICS IStaticMeshObject* CreateStaticMeshObject(MeshObjectInfo info) abstract;
			virtual FQ_GRAPHICS void AddAnimation(IStaticMeshObject* iStaticMeshObject, AnimationInfo info) abstract;
			virtual FQ_GRAPHICS void DeleteStaticMeshObject(IStaticMeshObject* meshObject) abstract;

			virtual FQ_GRAPHICS ISkinnedMeshObject* CreateSkinnedMeshObject(MeshObjectInfo info) abstract;
			virtual FQ_GRAPHICS void AddAnimation(ISkinnedMeshObject* iSkinnedMeshObject, AnimationInfo info) abstract;
			virtual FQ_GRAPHICS void DeleteSkinnedMeshObject(ISkinnedMeshObject* iSkinnedMeshObject) abstract;

			virtual FQ_GRAPHICS ITerrainMeshObject* CreateTerrainMeshObject(const MeshObjectInfo& info) abstract;
			virtual FQ_GRAPHICS void DeleteTerrainMeshObject(ITerrainMeshObject* meshObject) abstract;
			virtual FQ_GRAPHICS void SetTerrainMeshObject(ITerrainMeshObject* meshObject, const TerrainMaterialInfo& material) abstract;

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

			// Material

			virtual FQ_GRAPHICS std::shared_ptr<IMaterial> CreateMaterial(const MaterialInfo& materialInfo) abstract;
			virtual FQ_GRAPHICS std::shared_ptr<IParticleMaterial> CreateMaterial(const ParticleMaterialInfo& materialInfo) abstract;
			virtual FQ_GRAPHICS std::shared_ptr<IDecalMaterial> CreateMaterial(const DecalMaterialInfo& decalMaterialInfo) abstract;

			virtual FQ_GRAPHICS std::shared_ptr<IMaterial> CreateNamedMaterial(const std::string& key, const MaterialInfo& materialInfo) abstract;
			virtual FQ_GRAPHICS std::shared_ptr<IParticleMaterial> CreateNamedMaterial(const std::string& key, const ParticleMaterialInfo& materialInfo) abstract;
			virtual FQ_GRAPHICS std::shared_ptr<IDecalMaterial> CreateNamedMaterial(const std::string& key, const DecalMaterialInfo& decalMaterialInfo) abstract;

			virtual FQ_GRAPHICS std::shared_ptr<IMaterial> GetNamedMaterialOrNull(const std::string& key) abstract;
			virtual FQ_GRAPHICS std::shared_ptr<IParticleMaterial> GetNamedParticleMaterialOrNull(const std::string& key) abstract;
			virtual FQ_GRAPHICS std::shared_ptr<IDecalMaterial> GetNamedDecalMaterialOrNull(const std::string& key) abstract;

			virtual FQ_GRAPHICS void  DeleteMaterial(std::shared_ptr<IMaterial> iMaterial) abstract;
			virtual FQ_GRAPHICS void  DeleteMaterial(std::shared_ptr<IParticleMaterial> iParticleMaterial) abstract;
			virtual FQ_GRAPHICS void  DeleteMaterial(std::shared_ptr<IDecalMaterial> iDecalMaterial) abstract;

			virtual FQ_GRAPHICS void  DeleteNamedMaterial(const std::string& key) abstract;
			virtual FQ_GRAPHICS void  DeleteNamedParticleMaterial(const std::string& key) abstract;
			virtual FQ_GRAPHICS void  DeleteNamedDecalMaterial(const std::string& key) abstract;

			// VFX
			virtual FQ_GRAPHICS IParticleObject* CreateParticleObject(const DirectX::SimpleMath::Matrix& transform, const ParticleInfo& particleInfo, std::shared_ptr<IParticleMaterial> iParticleMaterial) abstract;
			virtual FQ_GRAPHICS void DeleteParticleObject(IParticleObject* particleObject) abstract;

			virtual FQ_GRAPHICS IDecalObject* CreateDecalObject(const DirectX::SimpleMath::Matrix& transform, const DecalInfo& decalInfo, std::shared_ptr<IDecalMaterial> iDecalMaterial) abstract;
			virtual FQ_GRAPHICS void DeleteDecalObject(IDecalObject* decalObjectInterface) abstract;

			virtual FQ_GRAPHICS ITrailObject* CreateTrailObject(const DirectX::SimpleMath::Matrix& trasform, const TrailInfo& trailInfo, std::shared_ptr<IParticleMaterial> iParticleMaterial) abstract;
			virtual FQ_GRAPHICS void DeleteTrailObject(ITrailObject* trailObjectInterface) abstract;

			/// Option (�׷��� �ɼ� On/Off, �Ұ����ϸ� ���� ���ϰ� �ϴ� �� ���� �׷� �� �ʿ����� ������)
			virtual FQ_GRAPHICS void SetPipelineType(EPipelineType pipelineType) abstract;

			/// UI
			// text
			virtual FQ_GRAPHICS void SetDefaultFontSize(const unsigned short fontSize) abstract;
			virtual FQ_GRAPHICS void SetDefaultFontColor(const DirectX::SimpleMath::Color& color) abstract;
			virtual FQ_GRAPHICS void SetDefaultFont(const std::wstring& path) abstract;
			virtual FQ_GRAPHICS void AddFont(const std::wstring& path) abstract; // ttf
			virtual FQ_GRAPHICS void DrawText(const std::wstring& text, const DirectX::SimpleMath::Rectangle& drawRect, unsigned short fontSize /*= 50*/, const std::wstring& fontPath /*= L"Verdana"*/, const DirectX::SimpleMath::Color& color /*= { 1, 0, 0, 1 }*/) abstract;
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

			/// Camera
			virtual FQ_GRAPHICS void SetCamera(const CameraInfo& cameraInfo) abstract;
			virtual FQ_GRAPHICS void UpdateCamera(const fq::common::Transform& cameraTransform) abstract;
			virtual FQ_GRAPHICS void UpdateColCamera(const fq::common::Transform& cameraTransform) abstract;

			/// Picking
			virtual FQ_GRAPHICS void* GetPickingObject(const short mouseX, const short mouseY) abstract;

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