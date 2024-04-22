#pragma once

#include <Windows.h>
#include <d3d11.h>

namespace fq::game_engine
{
	class GameProcess;
	class EditorProcess;

	/// <summary>
	/// ImGui 관련 처리를 담당하는 클래스
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
		/// ImGui 관련 기능을 초기화합니다 
		/// </summary>
		/// <param name="game">게임프로세스</param>
		/// <param name="editor">에디터 프로세스</param>
		void Initialize(HWND hwnd, ID3D11Device* device, ID3D11DeviceContext* deviceContext);

		/// <summary>
		/// Imgui를 새로운 프레임으로 갱신합니다
		/// 이 함수를 호출하고 ImGui 관련 창을 생성해야합니다
		/// </summary>
		void NewFrame();

		/// <summary>
		/// ImGui창을 랜더링합니다 
		/// 이 함수를 호출하기전에 랜더타겟을 클리어하고 게임의 랜더링 처리를합니다
		/// 이 함수를 호출하고 swapchain present함수를 호출합니다
		/// </summary>
		void RenderImGui();

		/// <summary>
		/// Imgui 관련 기능을 종료합니다
		/// </summary>
		void Finalize();

		/// <summary>
		/// ImGui 창의 스타일을 변경합니다
		/// </summary>
		void SetStyle(Style style);
	};
}