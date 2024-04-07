#pragma once

#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")

namespace fq::game_engine
{
	class GameProcess;

	class Editor
	{
	public:
		Editor();
		~Editor();

		void Initialize(GameProcess* process);
		void Finalize();

		void NewFrame();

		//tmp
		void createDeviceD3D();
		void createRenderTarget();
		void cleanupRenderTarget();
		void Render();


	private:
		void initializeImGui();

	private:
		GameProcess* mGameProcess;
		
		ID3D11Device* mDevice;
		ID3D11DeviceContext* mDeviceContext;
		IDXGISwapChain* mSwapChain;
		ID3D11RenderTargetView* mRenderTargetView;


	};


}