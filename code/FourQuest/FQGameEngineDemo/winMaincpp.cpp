#define WIN32_LEAN_AND_MEAN       
#include <windows.h>
#include <crtdbg.h>
#include <memory.h>
#include "Application.h"

// �ܼ� â ����
//#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console")

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR lpCmdLine,
	_In_ int nCmdShow)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	// �޸� �����˻��ϱ�
	//_CrtSetBreakAlloc(2732580);

	Application* app = new Application();

	if (!app->Initialize(Application::Mode::Tool))
	{
		app->Process();
	}
	app->Finalize();

	delete app;

	return 0;
}
