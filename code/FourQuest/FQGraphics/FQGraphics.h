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

		// RenderObject & Animation
		virtual void WriteModel(std::string path, const fq::common::Model& modelData) override;
		virtual void ConvertModel(std::string fbxFile, std::string fileName) override;
		virtual const fq::common::Model& CreateModel(std::string path, std::filesystem::path textureBasePath = "") override;
		virtual const fq::common::Model& GetModel(std::string path) override;
		virtual void DeleteModel(std::string path) override;
		virtual std::vector<std::shared_ptr<IMaterial>> GetMaterials() const override;

		virtual IStaticMeshObject* CreateStaticMeshObject(MeshObjectInfo info) override;
		virtual void AddAnimation(IStaticMeshObject* iStaticMeshObject, AnimationInfo info) override;
		virtual void DeleteStaticMeshObject(IStaticMeshObject* iStaticMeshObject) override;

		virtual ISkinnedMeshObject* CreateSkinnedMeshObject(MeshObjectInfo info) override;
		virtual void AddAnimation(ISkinnedMeshObject* iSkinnedMeshObject, AnimationInfo info) override;
		virtual void DeleteSkinnedMeshObject(ISkinnedMeshObject* iSkinnedMeshObject) override;

		virtual ITerrainMeshObject* CreateTerrainMeshObject(const MeshObjectInfo& info) override;
		virtual void DeleteTerrainMeshObject(ITerrainMeshObject* meshObject) override;
		virtual void SetTerrainMeshObject(ITerrainMeshObject* meshObject, const TerrainMaterialInfo& material) override;

		// Render Object Resource Interface version

		// render resource class, resource class들은 구체적인 맴버를 가져야 하므로 그래픽스에서만 생성 가능하게 해야함
		virtual std::shared_ptr<IModel> CreateModel(ModelInfo); // 매쉬와 애니메이션 본 구조 메테리얼에 대한 컴포지션 클래스 인터페이스
		virtual std::shared_ptr<IStaticMesh> CreateStaticMesh(StaticMeshInfo);
		virtual std::shared_ptr<ISkinnedMesh> CreateSkinnedMesh(SkinnedMeshInfo);
		virtual std::shared_ptr<IStaticAnimation> CreateStaticAnimation(StaticAnimationInfo);
		virtual std::shared_ptr<ISkinnedAnimation> CreateSkinnedAnimation(SkinnedAnimationInfo);
		virtual std::shared_ptr<IBoneHierarchy> CreateBoneHierarchy(BoneHierarchyInfo);
		virtual std::shared_ptr<IStandardMaterial> CreateStandardMaterial(const StandardMaterialInfo& standardMaterialInfo) override;
		virtual std::shared_ptr<IDecalMaterial> CreateDecalMaterial(const DecalMaterialInfo& standardMaterialInfo) override;
		virtual std::shared_ptr<IParticleMaterial> CreateParticleMaterial(const ParticleMaterialInfo& standardMaterialInfo) override;

		// draw resource class, 굳이 랜더 오브젝트를 구성하지 않고도 그리기 호출이 가능하도록 열어둔 인터페이스, 제한적인 기능과 최적화가 지원되지 않음
		virtual void DrawModel(std::shared_ptr<IModel>, DirectX::SimpleMath::Matrix transform); // 단순 그리기 용도.?
		virtual void DrawStaticMesh(std::shared_ptr<IStaticMesh>, std::vector<std::vector<IMaterial>>, DirectX::SimpleMath::Matrix transform);
		virtual void DrawStaticMesh(std::shared_ptr<IStaticMesh>, std::vector<std::vector<IMaterial>>, std::shared_ptr<IStaticAnimation>, DirectX::SimpleMath::Matrix transform, float timePos);
		virtual void DrawSkinnedMesh(std::shared_ptr<ISkinnedMesh>, std::vector<std::vector<IMaterial>>, DirectX::SimpleMath::Matrix transform);
		virtual void DrawSkinnedMesh(std::shared_ptr<ISkinnedMesh>, std::vector<std::vector<IMaterial>>, std::shared_ptr<ISkinnedAnimation>, std::shared_ptr<IBoneHierarchy>, DirectX::SimpleMath::Matrix transform, float timePos);

		// render object class, 랜더 오브젝트는 인터페이스 클래스로부터 생성될 예정이라 생성 삭제를 그래픽스 내부에서 할 필요가 없음
		virtual void DrawStaticMeshObject(std::shared_ptr<IStaticMeshObject> staticMeshObject);
		virtual void DrawSKinnedMeshObject(std::shared_ptr<ISkinnedMeshObject> staticMeshObject);
		virtual void DrawParticleObject(std::shared_ptr<IParticleObject> decalObject);
		virtual void DrawDecalObject(std::shared_ptr<IDecalObject> decalObject);

		/// Primitive
		// Debug Draw
		virtual void DrawSphere(const debug::SphereInfo& sphereInfo) override;
		virtual void DrawBox(const debug::AABBInfo& aabbInfo) override;
		virtual void DrawOBB(const debug::OBBInfo& obbInfo) override;
		virtual void DrawFrustum(const debug::FrustumInfo& frustumInfo) override;
		virtual void DrawGrid(const debug::GridInfo& gridInfo) override;
		virtual void DrawRing(const debug::RingInfo& ringInfo) override;
		virtual void DrawRay(const debug::RayInfo& rayInfo) override;
		virtual void DrawPolygon(const debug::PolygonInfo& polygonInfo) override;

		// VFX
		IParticleObject* CreateParticleObject(const ParticleInfo& particleInfo);
		void DeleteParticleObject(IParticleObject* particleObject);

		IDecalObject* CreateDecalObject(const DecalInfo& decalInfo);
		void DeleteDecalObject(IDecalObject* decalObjectInterface);
		void DrawDecalObject(IDecalObject* decalObject, std::shared_ptr<IMaterial> material, const DirectX::SimpleMath::Matrix& transform) override;

		/// Option (그래픽 옵션 On/Off, 불가능하면 선택 못하게 하는 등 이제 그런 게 필요하지 않을까)
		virtual void SetPipelineType(EPipelineType pipelineType) override;

		/// UI
		virtual void SetDefaultFontSize(const unsigned short fontSize) override;
		virtual void SetDefaultFontColor(const DirectX::SimpleMath::Color& color) override;
		virtual void SetDefaultFont(const std::wstring& path) override;
		virtual void AddFont(const std::wstring& path) override;
		virtual void DrawText(const std::wstring& text, const DirectX::SimpleMath::Rectangle& drawRect, unsigned short fontSize /*= 50*/, const std::wstring& fontPath /*= L"Verdana"*/, const DirectX::SimpleMath::Color& color /*= { 1, 0, 0, 1 }*/) override;
		virtual void DeleteFont(const std::wstring& path) override;
		// image
		virtual IImageObject* CreateImageObject(const UIInfo& uiInfo) override;
		virtual void DeleteImageObject(IImageObject* imageObject) override;

		/// SkyBox
		virtual void SetSkyBox(const std::wstring& path) override;
		virtual void SetIBLTexture(const std::wstring& diffuse, const std::wstring& specular, const std::wstring& brdfLUT) override;

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
		std::shared_ptr<class D3D11ParticleManager> mParticleManager;
		std::shared_ptr<class D3D11DecalManager> mDecalManager;

		std::shared_ptr<class D3D11PickingManager> mPickingManager;
		std::shared_ptr<class D3D11CullingManager> mCullingManager;
		std::shared_ptr<class UIManager> mUIManager;
	};
}

