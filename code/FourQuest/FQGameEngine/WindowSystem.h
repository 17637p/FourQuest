#pragma once

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

	public:
		WindowSystem();
		~WindowSystem();

		/// <summary>
		/// ������ â�� �����Ѵ�.
		/// </summary>
		void Initialize();

		/// <summary>
		/// ������ â ũ�� ���� Ȯ��
		/// </summary>
		/// <returns>���� ����</returns>
		bool IsResizedWindow();

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

	private:
		UINT mScreenWidth;
		UINT mScreenHeight;
		UINT mScreenLeft;
		UINT mScreenTop;
		HWND mHWND;
		HINSTANCE mHInstance;
	};


}