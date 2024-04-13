#include "BaseWindow.h"

#pragma once

namespace fq_graphics { class Renderer; }

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
	/// ---------- ó�� ������ �� �ʿ��� ��� �� ----------
	// ������ �ʱ� ��ġ
	const int mWindowPosX;
	const int mWindowPosY;

	// ȭ�� ũ��
	const int mScreenWidth;
	const int mScreenHeight;

	bool mResizing;

	// Graphics
	fq_graphics::Renderer* mTestGrphics;
};

