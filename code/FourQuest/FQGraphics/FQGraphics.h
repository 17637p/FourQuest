#pragma once

#include "IFQGraphics.h"

#include <memory>
#include <string>

namespace fq::graphics
{
	class Renderer;
	class IStaticMeshObject;

	class FQGraphics : public IFQGraphics
	{
	public:
		FQGraphics();
		virtual ~FQGraphics();

		/// Framework
		virtual bool Initialize(const HWND hWnd, const unsigned short width, const unsigned short height) override;
		virtual bool Update(float deltaTime) override;

		virtual bool BeginRender() override;
		virtual bool Render() override;
		virtual bool EndRender() override;

		virtual bool Finalize() override;

		/// 창 크기 변환
		virtual bool SetWindowSize(const unsigned short width, const unsigned short height) override; // 툴의 에디터를 포함한 전체 윈도우 영역
		virtual bool SetViewportSize(const unsigned short width, const unsigned short height) override; // 툴에서 씬을 그리는 영역

		// RenderObject
		bool CreateStaticMesh(std::string key, const fq::common::Mesh& meshData);
		bool CreateMaterial(std::string key, const fq::common::Material& matrialData, std::filesystem::path basePath);

		IStaticMeshObject* CreateStaticMeshObject(MeshObjectInfo info);
		void DeleteMeshObject(IStaticMeshObject* meshObject);

		/// Gizmo && Background

		/// Option (그래픽 옵션 On/Off, 불가능하면 선택 못하게 하는 등 이제 그런 게 필요하지 않을까)

		/// Primitive

		/// Animation

		/// Bounding Volume

		/// UI

		/// Light

		/// Camera
		//virtual FQ_GRAPHICS void SetCamera(CameraInfo cameraInfo) override;

		/// Picking
		//virtual FQ_GRAPHICS size_t GetPickingObjectID(UINT16 mouseX, UINT16 mouseY) override;

		/// For IMGUI(D3D11)
		//virtual FQ_GRAPHICS void* GetDivice() override;
		//virtual FQ_GRAPHICS void* GetDeviceContext() override;
		//virtual FQ_GRAPHICS void* GetSRV() override;

	private:
		// d3d device, context를 관리하는 클래스 하나
		// 매니저 집합으로 그래픽스를 구성하자
		std::shared_ptr<Renderer> mRenderer;
		std::shared_ptr<class D3D11ObjectManager> mObjectManager;
		std::shared_ptr<class D3D11JobManager> mJobManager;
		std::shared_ptr<class D3D11RenderManager> mRenderManager;
	};
}

