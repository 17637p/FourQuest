#pragma once

#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")

#include <memory>

namespace fq::game_engine
{
	class GameProcess;

	class Hierarchy;
	class Inspector;
	class FileDialog;
	class DebugViewer;

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
		
		// Window ฐüทร
		std::unique_ptr<Hierarchy>  mHierarchy;
		std::unique_ptr<Inspector>	mInspector;
		std::unique_ptr<FileDialog> mFileDialog;
		std::unique_ptr<DebugViewer> mDeubgViewer;

		ID3D11Device* mDevice = nullptr; 
		ID3D11DeviceContext* mDeviceContext = nullptr; 
		IDXGISwapChain* mSwapChain = nullptr;
		ID3D11RenderTargetView* mRenderTargetView = nullptr;
	};


}