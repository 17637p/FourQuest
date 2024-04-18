#pragma once

#include <windows.h>
#include <memory>

#pragma comment(lib, "d3d11.lib")
//#include <directxtk/SimpleMath.h>

#ifdef FQ_GRAPHICS_EXPORT
#define FQ_GRAPHICS __declspec(dllexport)
#else
#define FQ_GRAPHICS __declspec(dllimport)
#endif

#ifdef __cplusplus
extern "C" {
#endif

namespace fq::graphics
{
	struct CameraInfo
	{
		float _fieldOfView = 0;
		float _nearPlain = 0;
		float _farPlain = 0;
		bool _isPerspective = false;
	};

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
		virtual FQ_GRAPHICS bool Initialize(const HWND hWnd, const unsigned short width, const unsigned short height) abstract;
		virtual FQ_GRAPHICS bool Update(float deltaTime) abstract;

		virtual FQ_GRAPHICS bool BeginRender() abstract;
		virtual FQ_GRAPHICS bool Render() abstract;
		virtual FQ_GRAPHICS bool EndRender() abstract;

		virtual FQ_GRAPHICS bool Finalize() abstract;

		/// â ũ�� ��ȯ
		virtual FQ_GRAPHICS bool SetWindowSize(const unsigned short width, const unsigned short height) abstract; // ���� �����͸� ������ ��ü ������ ����
		virtual FQ_GRAPHICS bool SetViewportSize(const unsigned short width, const unsigned short height) abstract; // ������ ���� �׸��� ����

		/// Gizmo && Background

		/// Option (�׷��� �ɼ� On/Off, �Ұ����ϸ� ���� ���ϰ� �ϴ� �� ���� �׷� �� �ʿ����� ������)

		/// Primitive

		/// Animation

		/// Bounding Volume

		/// UI

		/// Light

		/// Camera
		//virtual FQ_GRAPHICS void SetCamera(CameraInfo cameraInfo) abstract;

		/// Picking
		//virtual FQ_GRAPHICS size_t GetPickingObjectID(UINT16 mouseX, UINT16 mouseY) abstract;

		/// For IMGUI(D3D11)
		//virtual FQ_GRAPHICS void* GetDivice() abstract;
		//virtual FQ_GRAPHICS void* GetDeviceContext() abstract;
		//virtual FQ_GRAPHICS void* GetSRV() abstract;
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