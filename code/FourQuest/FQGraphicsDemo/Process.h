#include "BaseWindow.h"

#pragma once
class Process : public BaseWindow<Process>
{
public:
	Process();
	~Process();

	bool Init(HINSTANCE hInstance);
	void Loop();
	void Finalize();

	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;

private:
	void Update();
	void Render();

private:
	//ImpGraphics::IImpGraphicsEngine* m_pRenderer;
	//ImpGraphics::IImp3DRenderer* m_pRenderer;

	//std::unique_ptr<GameTimer> m_timer;

private:
	/// ---------- 처음 실행할 때 필요한 상수 값 ----------
	// 윈도우 초기 위치
	const int m_windowPosX;
	const int m_windowPosY;

	// 화면 크기
	const int m_screenWidth;
	const int m_screenHeight;

	bool m_resizing;
};

