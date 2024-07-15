#pragma once

#include "BaseWindow.h"
#include "TimeManager.h"

#include <memory>
#include <map>

#include <FQCommon.h>
#include <FQCommonGraphics.h>
#include <IFQRenderResource.h>
#include <IFQRenderObject.h>
#include <IFQGraphics.h>

namespace fq::graphics { class IFQGraphics; }
namespace fq::graphics { class EngineExporter; }

namespace fq::graphics { class IStaticMeshObject; }
namespace fq::graphics { class ISkinnedMeshObject; }

class PostProcessingDemo : public BaseWindow<PostProcessingDemo>
{
public:
	PostProcessingDemo();
	~PostProcessingDemo();

	bool Init(HINSTANCE hInstance);
	void Loop();
	void Finalize();

	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;

private:
	// Default Framework
	void Update();
	void Render();
	void debugRender();

	void calculateFrameStats();

	void createModel(std::string modelPath, std::filesystem::path textureBasePath, DirectX::SimpleMath::Matrix transform = DirectX::SimpleMath::Matrix::Identity);

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

	std::vector<fq::graphics::IStaticMeshObject*> mStaticMeshObjects;
	std::vector<fq::graphics::ISkinnedMeshObject*> mSkinnedMeshObjects;
};

