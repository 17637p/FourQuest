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
		virtual bool Initialize(const HWND hWnd, const unsigned short width, const unsigned short height) override;
		virtual bool Update(float deltaTime) override;

		virtual bool BeginRender() override;
		virtual bool Render() override;
		virtual bool EndRender() override;

		virtual bool Finalize() override;

		/// â ũ�� ��ȯ
		virtual bool SetWindowSize(const unsigned short width, const unsigned short height) override; // ���� �����͸� ������ ��ü ������ ����
		virtual bool SetViewportSize(const unsigned short width, const unsigned short height) override; // ������ ���� �׸��� ����

		// RenderObject
		void ConvertModel(std::string fbxFile, std::string fileName) override;
		const fq::common::Model& CreateModel(std::string path, std::filesystem::path textureBasePath = "") override;
		const fq::common::Model& GetModel(std::string path) override;
		void DeleteModel(std::string path) override;

		IStaticMeshObject* CreateStaticMeshObject(MeshObjectInfo info) override;
		void DeleteStaticMeshObject(IStaticMeshObject* iStaticMeshObject) override;

		ISkinnedMeshObject* CreateSkinnedMeshObject(MeshObjectInfo info) override;
		void DeleteSkinnedMeshObject(ISkinnedMeshObject* iSkinnedMeshObject) override;

		/// Gizmo && Background

		/// Option (�׷��� �ɼ� On/Off, �Ұ����ϸ� ���� ���ϰ� �ϴ� �� ���� �׷� �� �ʿ����� ������)

		/// Primitive

		/// Animation

		/// Bounding Volume

		/// UI

		/// Light
		// Todo: ���� ����Ʈ, static ����Ʈ ������ �ϸ� ���� �� ����.
		virtual void AddLight(const unsigned int id, const LightInfo& lightInfo) override;
		virtual void SetLight(const unsigned int id, const LightInfo& lightInfo) override;
		virtual void DeleteLight(const unsigned int id) override;

		/// Camera
		virtual void SetCamera(const CameraInfo& cameraInfo) override;
		virtual void UpdateCamera(const fq::common::Transform& cameraTransform) override;

		/// Picking
		//virtual FQ_GRAPHICS size_t GetPickingObjectID(UINT16 mouseX, UINT16 mouseY) override;

		/// For IMGUI(D3D11)
		ID3D11Device* GetDivice() override;
		ID3D11DeviceContext* GetDeviceContext() override;
		ID3D11ShaderResourceView* GetSRV() override;

	private:
		std::shared_ptr<class D3D11Device> mDevice;
		std::shared_ptr<class D3D11ResourceManager> mResourceManager;
		std::shared_ptr<class D3D11ObjectManager> mObjectManager;
		std::shared_ptr<class D3D11JobManager> mJobManager;
		std::shared_ptr<class D3D11RenderManager> mRenderManager;
		
		std::shared_ptr<class D3D11CameraManager> mCameraManager;
		std::shared_ptr<class D3D11LightManager> mLightManager;
	};
}

