#pragma once

#include "BaseWindow.h"
#include "TimeManager.h"

#include <memory>

#include <FQCommon.h>
#include <FQCommonGraphics.h>

namespace fq::graphics { class IFQGraphics; }
namespace fq::graphics { class EngineExporter; }

namespace fq::graphics { class ITerrainMeshObject; }
namespace fq::graphics { class IStaticMeshObject; }
namespace fq::graphics { class IProbeObject; }

class TerrainDemo : public BaseWindow<TerrainDemo>
{
public:
	TerrainDemo();
	~TerrainDemo();

	bool Init(HINSTANCE hInstance);
	void Loop();
	void Finalize();

	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;

private:
	// Default Framework
	void Update();
	void Render();
	void debugRender();

	void createTerrain(std::string modelPath, DirectX::SimpleMath::Matrix transform = DirectX::SimpleMath::Matrix::Identity);

	//Probe Test
	void createModel(std::string modelPath, std::filesystem::path textureBasePath, DirectX::SimpleMath::Matrix transform, bool isUseLightProbe);
	void createProbeObject(std::string modelPath, std::filesystem::path textureBasePath, DirectX::SimpleMath::Matrix transformm, int index);
	void materialUpdate();
	void renderObjectInit();
	void renderObjectUpdate();
	void convertFBXModelAll(std::filesystem::path readFolderPath, std::filesystem::path outFolderPath);

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

	// Terrain
	fq::graphics::TerrainMaterialInfo terrainMaterial;
	std::vector<fq::graphics::ITerrainMeshObject*> mTerrainMeshObjects;

	// Probe Test
	std::vector<fq::graphics::IStaticMeshObject*> mStaticMeshObjects;
	std::vector<fq::graphics::IProbeObject*> mProbeObjects;
	std::vector<DirectX::SimpleMath::Vector3> lightProbePositions;
};

