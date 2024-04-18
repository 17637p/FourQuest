#pragma once

#include <windows.h>

#include "dllExport.h"

namespace fq::game_engine
{
	struct FQ_ENGNIE_API WndProc
	{
		static LRESULT CALLBACK GameWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
		static LRESULT CALLBACK ToolWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	};
}

