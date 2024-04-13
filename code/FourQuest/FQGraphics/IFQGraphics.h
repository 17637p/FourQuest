#pragma once

#include <windows.h>
//#include <directxtk/SimpleMath.h>

#define FQ_GRAPHICS 

#ifdef FQ_GRAPHICS
//#define FQ_GRAPHICS __declspec(dllexport)
#else
//#define FQ_GRAPHICS __declspec(dllimport)
#endif

/*
#ifdef __cplusplus
extern "C" {
#endif*/

namespace fq_graphics
{
	struct CameraInfo
	{
		float _fieldOfView = 0;
		float _nearPlain = 0;
		float _farPlain = 0;
		bool _isPerspective = false;
	};

	/// <summary>
	/// FPGraphics 인터페이스
	/// 언리얼 보니까 꼭 인터페이스로 나눠서 dll로 빼야하는 걸까 싶지만
	/// 이렇게 했을 때 종속성이 줄어들어서 관리가 편했던 건 사실이라 이렇게 하기로 했다.
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

		/// Gizmo && Background

		/// Option (그래픽 옵션 On/Off, 불가능하면 선택 못하게 하는 등 이제 그런 게 필요하지 않을까)

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
}

/*
#ifdef __cplusplus
}
#endif */