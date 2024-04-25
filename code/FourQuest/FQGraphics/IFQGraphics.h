#pragma once

#include <windows.h>
#include <memory>
#include <string>
#include <filesystem>

#include "d3d11.h"

#include "../FQCommon/IFQRenderObject.h"

#include "../FQCommon/FQCommonLoader.h"
#include "../FQCommon/FQCommonGraphics.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")
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
			virtual FQ_GRAPHICS bool Initialize(const HWND hWnd, const unsigned short width, const unsigned short height) abstract;
			virtual FQ_GRAPHICS bool Update(float deltaTime) abstract;

			virtual FQ_GRAPHICS bool BeginRender() abstract;
			virtual FQ_GRAPHICS bool Render() abstract;
			virtual FQ_GRAPHICS bool EndRender() abstract;

			virtual FQ_GRAPHICS bool Finalize() abstract;

			/// 창 크기 변환
			virtual FQ_GRAPHICS bool SetWindowSize(const unsigned short width, const unsigned short height) abstract; // 툴의 에디터를 포함한 전체 윈도우 영역
			virtual FQ_GRAPHICS bool SetViewportSize(const unsigned short width, const unsigned short height) abstract; // 툴에서 씬을 그리는 영역

			// 랜더 오브젝트
			virtual FQ_GRAPHICS void ConvertModel(std::string fbxFile, std::string path) abstract;
			virtual FQ_GRAPHICS const fq::common::Model& CreateModel(std::string path, std::filesystem::path textureBasePath = "") abstract;
			virtual FQ_GRAPHICS const fq::common::Model& GetModel(std::string path) abstract;
			virtual FQ_GRAPHICS void DeleteModel(std::string path) abstract;

			virtual FQ_GRAPHICS IStaticMeshObject* CreateStaticMeshObject(MeshObjectInfo info) abstract;
			virtual FQ_GRAPHICS void DeleteStaticMeshObject(IStaticMeshObject* meshObject) abstract;

			virtual FQ_GRAPHICS ISkinnedMeshObject* CreateSkinnedMeshObject(MeshObjectInfo info) abstract;
			virtual FQ_GRAPHICS void DeleteSkinnedMeshObject(ISkinnedMeshObject* iSkinnedMeshObject) abstract;
			/// Gizmo && Background

			/// Option (그래픽 옵션 On/Off, 불가능하면 선택 못하게 하는 등 이제 그런 게 필요하지 않을까)

			/// Primitive

			/// Animation

			/// Bounding Volume

			/// UI

			/// Light
			virtual FQ_GRAPHICS void AddLight(const unsigned int id, const LightInfo& lightInfo) abstract;
			virtual FQ_GRAPHICS void SetLight(const unsigned int id, const LightInfo& lightInfo) abstract;
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