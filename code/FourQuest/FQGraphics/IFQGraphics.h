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
			virtual FQ_GRAPHICS bool Initialize(const HWND hWnd, const unsigned short width, const unsigned short height, EPipelineType pipelineType = EPipelineType::Forward) abstract;
			virtual FQ_GRAPHICS bool Update(float deltaTime) abstract;

			virtual FQ_GRAPHICS bool BeginRender() abstract;
			virtual FQ_GRAPHICS bool Render() abstract;
			virtual FQ_GRAPHICS bool EndRender() abstract;

			virtual FQ_GRAPHICS bool Finalize() abstract;

			/// â ũ�� ��ȯ
			virtual FQ_GRAPHICS bool SetWindowSize(const unsigned short width, const unsigned short height) abstract; // ���� �����͸� ������ ��ü ������ ����
			virtual FQ_GRAPHICS bool SetViewportSize(const unsigned short width, const unsigned short height) abstract; // ������ ���� �׸��� ����

			// ���� ������Ʈ
			virtual FQ_GRAPHICS void ConvertModel(std::string fbxFile, std::string path) abstract;
			virtual FQ_GRAPHICS const fq::common::Model& CreateModel(std::string path, std::filesystem::path textureBasePath = "") abstract;
			virtual FQ_GRAPHICS const fq::common::Model& GetModel(std::string path) abstract;
			virtual FQ_GRAPHICS void DeleteModel(std::string path) abstract;

			virtual FQ_GRAPHICS IStaticMeshObject* CreateStaticMeshObject(MeshObjectInfo info) abstract;
			virtual FQ_GRAPHICS void DeleteStaticMeshObject(IStaticMeshObject* meshObject) abstract;

			virtual FQ_GRAPHICS ISkinnedMeshObject* CreateSkinnedMeshObject(MeshObjectInfo info) abstract;
			virtual FQ_GRAPHICS void AddAnimation(ISkinnedMeshObject* iSkinnedMeshObject, AnimationInfo info) abstract;
			virtual FQ_GRAPHICS void DeleteSkinnedMeshObject(ISkinnedMeshObject* iSkinnedMeshObject) abstract;

			// Debug Draw
			virtual FQ_GRAPHICS void DrawSphere(const debug::SphereInfo& sphereInfo) abstract;
			virtual FQ_GRAPHICS void DrawBox(const debug::AABBInfo& aabbInfo) abstract;
			virtual FQ_GRAPHICS void DrawOBB(const debug::OBBInfo& obbInfo) abstract;
			virtual FQ_GRAPHICS void DrawFrustum(const debug::FrustumInfo& frustumInfo) abstract;
			virtual FQ_GRAPHICS void DrawGrid(const debug::GridInfo& gridInfo) abstract;
			virtual FQ_GRAPHICS void DrawRing(const debug::RingInfo& ringInfo) abstract;
			virtual FQ_GRAPHICS void DrawRay(const debug::RayInfo& rayInfo) abstract;
			virtual FQ_GRAPHICS void DrawPolygon(const debug::PolygonInfo& polygonInfo) abstract;

			/// Gizmo && Background

			/// Option (�׷��� �ɼ� On/Off, �Ұ����ϸ� ���� ���ϰ� �ϴ� �� ���� �׷� �� �ʿ����� ������)
			virtual FQ_GRAPHICS void SetPipelineType(EPipelineType pipelineType) abstract;

			/// Primitive

			/// Animation

			/// Bounding Volume

			/// UI

			/// Light
			virtual FQ_GRAPHICS void AddLight(const unsigned int id, const LightInfo& lightInfo) abstract;
			virtual FQ_GRAPHICS void UpdateLight(const unsigned int id, const LightInfo& lightInfo) abstract;
			virtual FQ_GRAPHICS void DeleteLight(const unsigned int id) abstract;

			/// Camera
			virtual FQ_GRAPHICS void SetCamera(const CameraInfo& cameraInfo) abstract;
			virtual FQ_GRAPHICS void UpdateCamera(const fq::common::Transform& cameraTransform) abstract;

			/// Picking
			//virtual FQ_GRAPHICS size_t GetPickingObjectID(UINT16 mouseX, UINT16 mouseY) abstract;

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