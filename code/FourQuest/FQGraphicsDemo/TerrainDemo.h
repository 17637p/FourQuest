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
	void createModel(std::string modelPath, DirectX::SimpleMath::Matrix transform, bool isUseLightProbe);

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

	// Terrain
	fq::graphics::TerrainMaterialInfo terrainMaterial;
	std::vector<fq::graphics::ITerrainMeshObject*> mTerrainMeshObjects;

	// Probe Test
	std::vector<fq::graphics::IStaticMeshObject*> mStaticMeshObjects;
	std::vector<DirectX::SimpleMath::Vector3> lightProbePositions;
};

