#pragma once

#include "dllExport.h"

namespace fq
{
	namespace game_engine
	{
		/// <summary>
		/// ���� �������̽� 
		/// </summary>
		class IEngine
		{
		public:
			virtual ~IEngine() {}

			virtual FQ_ENGNIE_API void Initialize() abstract;

			virtual FQ_ENGNIE_API void Process() abstract;

			virtual FQ_ENGNIE_API void Finalize() abstract;
		};

		/// <summary>
		/// ���� ���� Exporter
		/// </summary>
		class Exporter
		{
		public:
			/// <summary>
			/// ���ӿ����� �Ҵ��ϰ� ��ȯ
			/// </summary>
			/// <returns>�Ҵ��� ���ӿ���</returns>
			static FQ_ENGNIE_API IEngine* GetGameEngine();

			/// <summary>
			/// �������� �Ҵ��ϰ� ��ȯ
			/// </summary>
			/// <returns>�Ҵ��� ������</returns>
			static FQ_ENGNIE_API IEngine* GetToolEngine();

			/// <summary>
			/// �Ҵ�� ���ӿ����� ����
			/// </summary>
			/// <returns></returns>
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
