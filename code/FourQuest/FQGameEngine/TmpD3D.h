#pragma once

#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")

#include <memory>

#include "EditorProcess.h"

namespace fq::game_engine
{
	class GameProcess;

	class TmpD3D
	{
	public:
		TmpD3D();
		~TmpD3D();

		void Initialize(GameProcess* process);

		void createDeviceD3D();
		void createRenderTarget();
		void cleanupRenderTarget();

		void Clear();

		void Present();

		ID3D11Device* GetDevice() { return mDevice; }
		ID3D11DeviceContext* GetDC() { return mDeviceContext; }
	private:
		void clearGarbage();


	private:
		GameProcess* mGameProcess;

		ID3D11Device* mDevice = nullptr; 
		ID3D11DeviceContext* mDeviceContext = nullptr; 
		IDXGISwapChain* mSwapChain = nullptr;
		ID3D11RenderTargetView* mRenderTargetView = nullptr;
	};


}