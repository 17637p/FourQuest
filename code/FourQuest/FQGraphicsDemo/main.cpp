#include <memory>

#include "Process.h"

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

	std::unique_ptr<Process> mainWindow = std::make_unique<Process>();

	mainWindow->Init(hInstance);
	mainWindow->Loop();
	mainWindow->Finalize();

	return 0;
}