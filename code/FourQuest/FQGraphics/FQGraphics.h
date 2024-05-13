#pragma once

#include "IFQGraphics.h"

#include <map>
#include <memory>
#include <string>
#include <filesystem>

namespace fq::graphics
{
	class FQGraphics : public IFQGraphics
	{
	public:
		FQGraphics();
		virtual ~FQGraphics();

		/// Framework
		virtual bool Initialize(const HWND hWnd, const unsigned short width, const unsigned short height, EPipelineType pipelineType) override;
		virtual bool Update(float deltaTime) override;

		virtual bool BeginRender() override;
		virtual bool Render() override;
		virtual bool EndRender() override;

		virtual bool Finalize() override;

		/// 창 크기 변환
		virtual bool SetWindowSize(const unsigned short width, const unsigned short height) override; // 툴의 에디터를 포함한 전체 윈도우 영역
		virtual bool SetViewportSize(const unsigned short width, const unsigned short height) override; // 툴에서 씬을 그리는 영역

		// RenderObject
		void ConvertModel(std::string fbxFile, std::string fileName) override;
		const fq::common::Model& CreateModel(std::string path, std::filesystem::path textureBasePath = "") override;
		const fq::common::Model& GetModel(std::string path) override;
		void DeleteModel(std::string path) override;

		IStaticMeshObject* CreateStaticMeshObject(MeshObjectInfo info) override;
		void DeleteStaticMeshObject(IStaticMeshObject* iStaticMeshObject) override;

		ISkinnedMeshObject* CreateSkinnedMeshObject(MeshObjectInfo info) override;
		void AddAnimation(ISkinnedMeshObject* iSkinnedMeshObject, AnimationInfo info) override;
		void DeleteSkinnedMeshObject(ISkinnedMeshObject* iSkinnedMeshObject) override;

		virtual ITerrainMeshObject* CreateTerrainMeshObject(MeshObjectInfo info) override;
		virtual void DeleteTerrainMeshObject(ITerrainMeshObject* meshObject) override;
		virtual void SetTerrainMeshObject(ITerrainMeshObject* meshObject, fq::common::TerrainMaterial material) override;

		// Debug Draw
		virtual void DrawSphere(const debug::SphereInfo& sphereInfo) override;
		virtual void DrawBox(const debug::AABBInfo& aabbInfo) override;
		virtual void DrawOBB(const debug::OBBInfo& obbInfo) override;
		virtual void DrawFrustum(const debug::FrustumInfo& frustumInfo) override;
		virtual void DrawGrid(const debug::GridInfo& gridInfo) override;
		virtual void DrawRing(const debug::RingInfo& ringInfo) override;
		virtual void DrawRay(const debug::RayInfo& rayInfo) override;
		virtual void DrawPolygon(const debug::PolygonInfo& polygonInfo) override;

		/// Gizmo && Background

		/// Option (그래픽 옵션 On/Off, 불가능하면 선택 못하게 하는 등 이제 그런 게 필요하지 않을까)
		virtual void SetPipelineType(EPipelineType pipelineType) override;

		/// Primitive

		/// Animation

		/// Bounding Volume

		/// UI
		virtual void AddFont(const std::wstring& path) override;
		virtual void DeleteFont(const std::wstring& path) override;

		virtual void SetDefaultFont(const std::wstring& path) override;
		virtual void SetDefaultFontColor(const DirectX::SimpleMath::Color& color) override;
		virtual void SetDefaultFontSize(const unsigned short fontSize) override;

		virtual void DrawText(const std::wstring& text, const DirectX::SimpleMath::Rectangle& drawRect, unsigned short fontSize, const std::wstring& fontPath, const DirectX::SimpleMath::Color& color) override;

		/// SkyBox
		virtual void SetSkyBox(const std::wstring& path) override;

		/// Light
		// Todo: 동적 라이트, static 라이트 구별을 하면 좋을 것 같다.
		virtual void AddLight(const unsigned int id, const LightInfo& lightInfo) override;
		virtual void UpdateLight(const unsigned int id, const LightInfo& lightInfo) override;
		virtual void DeleteLight(const unsigned int id) override;

		virtual void UseShadow(const unsigned int id, bool bUseShadow);

		/// Camera
		virtual void SetCamera(const CameraInfo& cameraInfo) override;
		virtual void UpdateCamera(const fq::common::Transform& cameraTransform) override;
		virtual void UpdateColCamera(const fq::common::Transform& cameraTransform) override;

		/// Picking
		virtual void* GetPickingObject(const short mouseX, const short mouseY) override;

		/// For IMGUI(D3D11)
		ID3D11Device* GetDivice() override;
		ID3D11DeviceContext* GetDeviceContext() override;
		ID3D11ShaderResourceView* GetSRV() override;

		/// spdlog 설정
		virtual FQ_GRAPHICS std::shared_ptr<spdlog::logger> SetUpLogger(std::vector<spdlog::sink_ptr> sinks) override;

	private:
		std::shared_ptr<class D3D11Device> mDevice;
		std::shared_ptr<class D3D11ResourceManager> mResourceManager;
		std::shared_ptr<class D3D11ModelManager> mModelManager;
		std::shared_ptr<class D3D11ObjectManager> mObjectManager;
		std::shared_ptr<class D3D11JobManager> mJobManager;
		std::shared_ptr<class D3D11RenderManager> mRenderManager;
		std::shared_ptr<class D3D11CameraManager> mCameraManager;
		std::shared_ptr<class D3D11LightManager> mLightManager;
		std::shared_ptr<class D3D11DebugDrawManager> mDebugDrawManager;

		std::shared_ptr<class D3D11PickingManager> mPickingManager;
		std::shared_ptr<class D3D11CullingManager> mCullingManager;
		std::shared_ptr<class UIManager> mUIManager;
	};
}

