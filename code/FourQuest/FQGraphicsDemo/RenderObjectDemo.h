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

class RenderObjectDemo : public BaseWindow<RenderObjectDemo>
{
public:
	RenderObjectDemo();
	~RenderObjectDemo();

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

	void createModel(std::string modelPath, std::filesystem::path textureBasePath, DirectX::SimpleMath::Matrix transform = DirectX::SimpleMath::Matrix::Identity, bool bIsCreateHierarchy = false);

private:
	/// 기본적으로 필요한 변수들 
	TimeManager mTimeManager;

	const int mWindowPosX;
	const int mWindowPosY;

	// 화면 크기
	int mScreenWidth;
	int mScreenHeight;

	bool mResizing;

	/// Camera
	fq::common::Transform mCameraTransform;

	/// Graphics
	// Engine
	fq::graphics::IFQGraphics* mTestGraphics;
	std::shared_ptr<fq::graphics::EngineExporter> mEngineExporter;

	struct SkinnedMeshRender
	{
		fq::graphics::ISkinnedMeshObject* SkinnedMeshObject;
		std::map<std::string, std::shared_ptr<fq::graphics::IAnimation>> Animations;
		std::shared_ptr<fq::graphics::INodeHierarchyInstance> NodeHierarchyInstance;
	};

	struct StaticMeshRenderer
	{
		fq::graphics::IStaticMeshObject* StaticMeshObject;
		std::map<std::string, std::shared_ptr<fq::graphics::IAnimation>> Animations;
		std::shared_ptr<fq::graphics::INodeHierarchyInstance> NodeHierarchyInstance;
	};

	std::vector<fq::graphics::IStaticMeshObject*> mStaticMeshObjects;
	std::vector<fq::graphics::ISkinnedMeshObject*> mSkinnedMeshObjects;
	std::vector<SkinnedMeshRender> mSkinnedMeshRenderers;
	std::vector<StaticMeshRenderer> mStaticMeshRenderers;
};

