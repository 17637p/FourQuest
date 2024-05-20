#include "WndProc.h"

#include <string>
#include <vector>
#include <imgui.h>

#include "WindowSystem.h"
#include "../FQGameModule/InputManager.h"

LRESULT CALLBACK fq::game_engine::WndProc::GameWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
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
		case WM_DROPFILES: // 드랍 시 이벤트.    
		{
			HDROP hDrop = (HDROP)wParam;

			std::vector<std::wstring>& dropFiles = WindowSystem::DropFiles;
			TCHAR* refFiles = nullptr;
			UINT refFilesLen = 0;
			POINT refPoint{};

			UINT fileCount = DragQueryFile(hDrop, 0xFFFFFFFF, refFiles, refFilesLen);

			if (fileCount > 0)
			{
				for (UINT i = 0; i < fileCount; i++)
				{
					int fileLength = DragQueryFile(hDrop, i, NULL, 0) + 1;
					TCHAR* fileName = new TCHAR[fileLength];
					memset(fileName, 0, sizeof(TCHAR) * fileLength);

					DragQueryFile(hDrop, i, fileName, fileLength);

					std::wstring newFile = fileName;
					dropFiles.push_back(newFile);

					delete[] fileName;
				}
			}
		}
		break;
		case WM_MOUSEWHEEL:
		{
			SHORT wheelDeta = GET_WHEEL_DELTA_WPARAM(wParam);

			fq::game_module::InputManager::DeltaMouseWheel += wheelDeta;
		}
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}