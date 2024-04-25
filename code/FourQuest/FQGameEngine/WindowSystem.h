#pragma once

#include <windows.h>
#include <string>
#include <vector>

namespace fq::game_engine
{
	/// <summary>
	/// ������ â�� ���� ó���� ���
	/// </summary>
	class WindowSystem
	{
	public:
		static UINT Width;
		static UINT Height;
		static std::vector<std::wstring> DropFiles;

	public:
		WindowSystem();
		~WindowSystem();

		/// <summary>
		/// ������ â�� �����Ѵ�.
		/// </summary>
		void Initialize();

		/// <summary>
		/// ������ ���� ������ â�� �����մϴ�
		/// </summary>
		void InitializeEditorType();

		/// <summary>
		/// ������ â ũ�� ���� Ȯ��
		/// </summary>
		/// <returns>���� ����</returns>
		bool IsResizedWindow()const ;

		/// <summary>
		/// ������ â ũ�� ������ ȣ���Ѵ�.
		/// </summary>
		void OnResize();

		/// <summary>
		/// ������ ����
		/// </summary>
		void Finalize();

		/// <summary>
		/// ������ �ڵ� ��ȯ
		/// </summary>
		/// <returns>������ �ڵ�</returns>
		HWND GetHWND()const { return mHWND; }
		
		/// <summary>
		///  HINSTANCE ��ȯ
		/// </summary>
		/// <returns>HINSTANCE</returns>
		HINSTANCE GetHINSTANCE()const { return mHInstance; }

		/// <summary>
		/// ������ ���� ���� ��ȯ
		/// </summary>
		/// <returns>������ ����</returns>
		UINT GetScreenWidth()const { return mScreenWidth; }
		
		/// <summary>
		/// ������ â ���� ���� ��ȯ
		/// </summary>
		/// <returns>������ ����</returns>
		UINT GetScreenHeight()const { return mScreenHeight; }
		
		/// <summary>
		/// ������ â ���� ��ǥ ��ȯ
		/// </summary>
		/// <returns>������ ���G ��ǥ</returns>
		UINT GetScreenLeft()const { return mScreenLeft; }

		/// <summary>
		/// ������ â ��� ��ǥ ��ȯ
		/// </summary>
		/// <returns>������ ��� ��ǥ</returns>
		UINT GetScreenTop()const { return mScreenTop; }

	private:
		UINT mScreenWidth;
		UINT mScreenHeight;
		UINT mScreenLeft;
		UINT mScreenTop;
		HWND mHWND;
		HINSTANCE mHInstance;
	};
}