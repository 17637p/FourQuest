#include <memory>

/// Demo
#include "Process.h"
#include "TerrainDemo.h"

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

	//std::unique_ptr<Process> demo = std::make_unique<Process>();
	std::unique_ptr<TerrainDemo> demo = std::make_unique<TerrainDemo>();

	demo->Init(hInstance);
	demo->Loop();
	demo->Finalize();

	return 0;
}