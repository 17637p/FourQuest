#include "FQGameEnginePCH.h"
#include "IEngine.h"
#include "GameEngine.h"
#include "ToolEngine.h"
#include "WindowSystem.h"

fq::game_engine::IEngine* fq::game_engine::Exporter::mEngine = nullptr;

FQ_ENGNIE_API fq::game_engine::IEngine* fq::game_engine::Exporter::GetGameEngine()
{
	mEngine = new GameEngine();

	return mEngine;
}

FQ_ENGNIE_API fq::game_engine::IEngine* fq::game_engine::Exporter::GetToolEngine()
{
	mEngine = new ToolEngine();

	return mEngine;
}

FQ_ENGNIE_API void fq::game_engine::Exporter::DeleteEngine()
{
	if (mEngine)
	{
		delete mEngine;
	}
}

LRESULT CALLBACK fq::game_engine::WndProc::GameWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		//case WM_SIZE:
		//	if (wParam == SIZE_MINIMIZED) return 0;
		//	{
		//	}
		//	break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK fq::game_engine::WndProc::ToolWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
		return true;

	switch (message)
	{
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		case WM_SIZE:
		{
			WindowSystem::Width = LOWORD(lParam);
			WindowSystem::Height = HIWORD(lParam);
		}
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}
