#pragma once

#include "BaseWindow.h"
#include "TimeManager.h"

#include <memory>

#include <FQCommon.h>
#include <FQCommonGraphics.h>

namespace fq::graphics { class IFQGraphics; }
namespace fq::graphics { class EngineExporter; }

class DefaultDemo : public BaseWindow<DefaultDemo>
{
public:
	DefaultDemo();
	~DefaultDemo();

	bool Init(HINSTANCE hInstance);
	void Loop();
	void Finalize();

	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;

private:
	// Default Framework
	void Update();
	void Render();
	void debugRender();

private:
	/// �⺻������ �ʿ��� ������ 
	TimeManager mTimeManager;

	const int mWindowPosX;
	const int mWindowPosY;

	// ȭ�� ũ��
	int mScreenWidth;
	int mScreenHeight;

	bool mResizing;

	/// Camera
	fq::common::Transform mCameraTransform;

	/// Graphics
	// Engine
	fq::graphics::IFQGraphics* mTestGraphics;
	std::shared_ptr<fq::graphics::EngineExporter> mEngineExporter;
};

