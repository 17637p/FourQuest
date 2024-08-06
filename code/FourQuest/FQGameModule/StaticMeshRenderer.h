#pragma once

#include "Component.h"

#include "../FQCommon/IFQRenderObject.h"
#include "../FQCommon/FQCommonGraphics.h"

namespace fq::game_module
{
	class StaticMeshRenderer : public Component
	{
		using StaticMeshObject = fq::graphics::IStaticMeshObject;
	public:
		/// <summary>
		/// 생성자
		/// </summary>
		StaticMeshRenderer();
		~StaticMeshRenderer();

		/// <summary>
		/// 복사본을 반환합니다 
		/// </summary>
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

		/// <summary>
		/// 스태틱 메쉬를 반환합니다
		/// </summary>
		StaticMeshObject* GetStaticMeshObject() const { return mStaticMeshObject; }

		/// <summary>
		/// 스태틱 메쉬를 설정합니다
		/// </summary>
		/// <param name="mesh"></param>
		void SetStaticMeshObject(StaticMeshObject* mesh) { mStaticMeshObject = mesh; }

		/// <summary>
		/// 모델의 경로를 반환합니다
		/// </summary>
		/// <returns></returns>
		std::string GetModelPath()const { return mModelPath; }

		/// <summary>
		/// 모델경로를 설정합니다
		/// </summary>
		/// <param name="path"></param>
		void SetModelPath(std::string path) { mModelPath = path; }

		/// <summary>
		/// 텍스처 폴더 경로를 설정합니다 
		/// </summary>
		std::string GetTexturePath() const { return mTexturePath; }

		/// <summary>
		/// 텍스터 폴더 경로를 반환합니다
		/// </summary>
		void SetTexturePath(std::string path) { mTexturePath = path; }

		/// <summary>
		/// 메쉬 이름을 반환합니다
		/// </summary>
		/// <returns></returns>
		std::string GetMeshName()const { return mMeshName; }

		/// <summary>
		/// 메쉬 이름을 설정합니다
		/// </summary>
		/// <param name="name"></param>
		void SetMeshName(std::string name) { mMeshName = name; }

		/// <summary>
		/// 소유하고있는 메테리얼 이름들을 반환합니다
		/// </summary>
		/// <returns></returns>
		const std::vector<std::string>& GetMaterials()const { return mMaterialNames; }

		/// <summary>
		/// 메테리얼을 설정합니다
		/// </summary>
		/// <param name="materials"></param>
		void SetMaterials(std::vector<std::string> materials) { mMaterialNames = std::move(materials); }

		void UpdateMaterialInfoByMaterialInterface();
		const std::vector<fq::graphics::MaterialInfo>& GetMaterialInfos() const;
		void SetMaterialInfos(std::vector<fq::graphics::MaterialInfo> materialInfos);

		std::vector<std::shared_ptr<fq::graphics::IMaterial>> GetMaterialInterfaces() const { return mMaterialInterfaces; }
		void SetMaterialInterfaces(std::vector<std::shared_ptr<fq::graphics::IMaterial>> materialInterfaces);

		/// <summary>
		/// 메쉬정보를 설정합니다
		/// </summary>
		/// <param name="info">메쉬 정보</param>
		void SetMeshObjectInfomation(fq::graphics::MeshObjectInfo info) { mMeshInfomation = std::move(info); if (mStaticMeshObject != nullptr) { mStaticMeshObject->SetMeshObjectInfo(mMeshInfomation); } }

		/// <summary>
		/// 메쉬정보를 반환합니다
		/// </summary>
		/// <returns>메쉬 정보</returns>
		fq::graphics::MeshObjectInfo GetMeshObjectInfomation()const { return mMeshInfomation; }

		// Navigation 사용 여부
		bool GetIsNavigationMeshUsed() const { return mIsNavigationMeshUsed; }
		void SetIsNavigationMeshUsed(bool val) { mIsNavigationMeshUsed = val; }

		void SetMaterialPaths(const std::vector<std::string>& materialPaths) { mMaterialPaths = materialPaths; }
		const std::vector<std::string>& GetMaterialPaths() const { return mMaterialPaths; }

		// 라이트맵 관련
		void SetLightmapUVScaleOffset(const DirectX::SimpleMath::Vector4& scaleOffset);
		const DirectX::SimpleMath::Vector4& GetLightmapUVScaleOffset() const;

		void SetLightmapIndex(unsigned int lightmapIndex);
		int GetLightmapIndex() const;

		void SetIsStatic(bool bIsStatic);
		bool GetIsStatic() const;

	private:
		entt::meta_handle GetHandle() override;

	private:
		bool mIsNavigationMeshUsed; // Navigation Mesh의 장애물로 사용할 것인가

		StaticMeshObject* mStaticMeshObject;
		fq::graphics::MeshObjectInfo mMeshInfomation;

		std::string mModelPath;
		std::string mTexturePath;
		std::string mMeshName;
		std::vector<std::string> mMaterialNames; // 컨버트 완료 후 지울 예정
		std::vector<fq::graphics::MaterialInfo> mMaterialInfos; // 컨버트 완료 후 지울 예정
		std::vector<std::shared_ptr<fq::graphics::IMaterial>> mMaterialInterfaces;

		// 추가
		std::vector<std::string> mMaterialPaths; // 별도로 직렬화된 데이터로 인터페이스 가져올 예정

		// 라이트맵 관련 인자
		DirectX::SimpleMath::Vector4 mLightmapScaleOffset;
		int mLightmapIndex;
		bool mbIsStatic;
	};

}