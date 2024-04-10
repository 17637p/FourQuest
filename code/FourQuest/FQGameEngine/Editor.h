#pragma once

#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")

#include <memory>

#include "EditorProcess.h"

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

		void createDeviceD3D();
		void createRenderTarget();
		void cleanupRenderTarget();

		void RenderWindow();

		void Render();

	private:
		void initializeImGui();

	private:
		GameProcess* mGameProcess;
		std::unique_ptr<EditorProcess> mEditorProcess;

		ID3D11Device* mDevice = nullptr; 
		ID3D11DeviceContext* mDeviceContext = nullptr; 
		IDXGISwapChain* mSwapChain = nullptr;
		ID3D11RenderTargetView* mRenderTargetView = nullptr;
	};


}