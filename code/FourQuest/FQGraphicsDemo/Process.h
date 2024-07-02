#pragma once

#include "BaseWindow.h"
#include "TimeManager.h"

#include <memory>
#include <vector>
#include <map>
#include <string>
#include <filesystem>

#include <directxtk\SimpleMath.h>
#include <FQCommon.h>

#include "../FQLoader/ModelLoader.h"
#include "../FQCommon/FQCommonGraphics.h"

namespace fq::graphics { class Renderer; }
namespace fq::graphics { class IFQGraphics; }
namespace fq::graphics { class EngineExporter; }
namespace fq::graphics { class IStaticMeshObject; }
namespace fq::graphics { class ISkinnedMeshObject; }
namespace fq::graphics { class IParticleObject; }
namespace fq::graphics { class IDecalObject; }
namespace fq::graphics { class ITrailObject; }

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
	void debugRender();
	void shadowTest();

	void createModel(std::string modelPath, DirectX::SimpleMath::Matrix transform = DirectX::SimpleMath::Matrix::Identity);
	void createModel(std::string modelPath, std::vector<fq::graphics::AnimationInfo> animInfos, DirectX::SimpleMath::Matrix transform = DirectX::SimpleMath::Matrix::Identity);

	void calculateFrameStats();

	void convertFBXModelAll(std::filesystem::path readFolderPath, std::filesystem::path outFolderPath);
	void convertFBXModelAll(std::filesystem::path folderPath);

	void particleInit();
	void particleUpdate();

	void materialUpdate();

	void socketUpdate();

	void decalInit();
	void decalUpdate();

	void trailInit();
	void trailUpdate();

private:
	/// ---------- 처음 실행할 때 필요한 상수 값 ----------
	// 윈도우 초기 위치
	TimeManager mTimeManager;

	const int mWindowPosX;
	const int mWindowPosY;

	// 화면 크기
	int mScreenWidth;
	int mScreenHeight;

	bool mResizing;

	// Graphics
	fq::graphics::IFQGraphics* mTestGraphics;
	std::shared_ptr<fq::graphics::EngineExporter> mEngineExporter;

	std::vector<std::shared_ptr<fq::graphics::IStaticMeshObject>> mStaticMeshObjects;
	std::vector<std::shared_ptr<fq::graphics::ISkinnedMeshObject>> mSkinnedMeshObjects;
	std::vector<fq::graphics::IParticleObject*> mParticleObjects;
	std::vector<fq::graphics::std::shared_ptr<IDecalObject>> mDecalObjects;
	std::vector<fq::graphics::ITrailObject*> mTrailObjects;

	std::shared_ptr<fq::graphics::IStaticMeshObject> mSocketStaticMeshObject;
	std::shared_ptr<fq::graphics::ISkinnedMeshObject> mSoketSkinnedMeshObject;
	DirectX::SimpleMath::Matrix mSocketInitTransform;
	// Camera
	fq::common::Transform cameraTransform;
	fq::common::Transform cameraTransform2;
};

