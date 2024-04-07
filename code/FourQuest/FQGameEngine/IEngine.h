#pragma once

#include "dllExport.h"

namespace fq
{
	namespace game_engine
	{
		/// <summary>
		/// 엔진 인터페이스 
		/// </summary>
		class IEngine
		{
		public:
			virtual ~IEngine() {}

			virtual FQ_ENGNIE_API void Initialize() abstract;

			virtual FQ_ENGNIE_API void Process() abstract;

			virtual FQ_ENGNIE_API void Finalize() abstract;
		};

		class Exporter
		{
		public:
			static FQ_ENGNIE_API IEngine* GetGameEngine();

			static FQ_ENGNIE_API IEngine* GetToolEngine();

			static FQ_ENGNIE_API void DeleteEngine();
			 
		private:
			static IEngine* mEngine;
		};

		struct FQ_ENGNIE_API WndProc
		{
			static LRESULT CALLBACK GameWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
			static LRESULT CALLBACK ToolWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
		};
	}
}
