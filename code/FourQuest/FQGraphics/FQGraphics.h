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

		/// â ũ�� ��ȯ
		virtual bool SetWindowSize(const unsigned short width, const unsigned short height) override; // ���� �����͸� ������ ��ü ������ ����
		virtual bool SetViewportSize(const unsigned short width, const unsigned short height) override; // ������ ���� �׸��� ����

		/// Gizmo && Background

		/// Option (�׷��� �ɼ� On/Off, �Ұ����ϸ� ���� ���ϰ� �ϴ� �� ���� �׷� �� �ʿ����� ������)

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

