#pragma once

#include "IFQGraphics.h"

#include <memory>

namespace fq_graphics
{
	class Renderer;

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
		std::shared_ptr<Renderer> mRenderer;
	};
}

