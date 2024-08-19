#include <memory>

/// Demo
//#include "DefaultDemo.h"
#include "Process.h"
#include "TerrainDemo.h"
#include "UIDemo.h"
#include "RenderObjectDemo.h"
#include "EffectDemo.h"
#include "PostProcessingDemo.h"
#include "LightMapDemo.h"
#include "AlphaVertexAnimationDemo.h"

#ifdef _DEBUG
#pragma comment(lib, "../x64/Debug/FQGraphics.lib")
//#pragma comment(lib, "../x64/Debug/ImpGraphicsEngine.lib")
#else
#pragma comment(lib, "../x64/Release/FQGraphics.lib")
//#pragma comment(lib, "../x64/Release/ImpGraphicsEngine.lib")
#endif // DEBUG

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_crtBreakAlloc = 101010; //_CrtSetBreakAlloc(101010); 
#endif

	// std::unique_ptr<Process> demo = std::make_unique<Process>();
	// std::unique_ptr<TerrainDemo> demo = std::make_unique<TerrainDemo>();
	// std::unique_ptr<UIDemo> demo = std::make_unique<UIDemo>();
	std::unique_ptr<RenderObjectDemo> demo = std::make_unique<RenderObjectDemo>();
	// std::unique_ptr<EffectDemo> demo = std::make_unique<EffectDemo>();
	// std::unique_ptr<EffectDemo> demo = std::make_unique<EffectDemo>();
	// std::unique_ptr<PostProcessingDemo> demo = std::make_unique<PostProcessingDemo>();
	// std::unique_ptr<PostProcessingDemo> demo = std::make_unique<PostProcessingDemo>();
	// std::unique_ptr<LightMapDemo> demo = std::make_unique<LightMapDemo>();
	// std::unique_ptr<AlphaVertexAnimationDemo> demo = std::make_unique<AlphaVertexAnimationDemo>();

	demo->Init(hInstance);
	demo->Loop();
	demo->Finalize();

	return 0;
}