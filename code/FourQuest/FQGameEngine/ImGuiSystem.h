#pragma once

#include <Windows.h>
#include <d3d11.h>

namespace fq::game_engine
{
	class GameProcess;
	class EditorProcess;

	/// <summary>
	/// ImGui ���� ó���� ����ϴ� Ŭ����
	/// </summary>
	class ImGuiSystem
	{
	public:
		enum class Style
		{
			Default,
			Dark,
			Light,
			Classic
		};

		ImGuiSystem();
		~ImGuiSystem();

		/// <summary>
		/// ImGui ���� ����� �ʱ�ȭ�մϴ� 
		/// </summary>
		/// <param name="game">�������μ���</param>
		/// <param name="editor">������ ���μ���</param>
		void Initialize(HWND hwnd, ID3D11Device* device, ID3D11DeviceContext* deviceContext);

		/// <summary>
		/// Imgui�� ���ο� ���������� �����մϴ�
		/// �� �Լ��� ȣ���ϰ� ImGui ���� â�� �����ؾ��մϴ�
		/// </summary>
		void NewFrame();

		/// <summary>
		/// ImGuiâ�� �������մϴ� 
		/// �� �Լ��� ȣ���ϱ����� ����Ÿ���� Ŭ�����ϰ� ������ ������ ó�����մϴ�
		/// �� �Լ��� ȣ���ϰ� swapchain present�Լ��� ȣ���մϴ�
		/// </summary>
		void RenderImGui();

		/// <summary>
		/// Imgui ���� ����� �����մϴ�
		/// </summary>
		void Finalize();

		/// <summary>
		/// ImGui â�� ��Ÿ���� �����մϴ�
		/// </summary>
		void SetStyle(Style style);
	};
}