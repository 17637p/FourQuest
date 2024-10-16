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
		~StaticMeshRenderer() = default;

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
		/// 메쉬정보를 반환합니다
		/// </summary>
		/// <returns>메쉬 정보</returns>
		fq::graphics::MeshObjectInfo GetMeshObjectInfomation()const { return mMeshInfomation; }

		/// <summary>
		/// 메쉬정보를 설정합니다
		/// </summary>
		/// <param name="info">메쉬 정보</param>
		void SetMeshObjectInfomation(const fq::graphics::MeshObjectInfo& info);

		// 머터리얼 인터페이스 Getter/Setter
		std::vector<std::shared_ptr<fq::graphics::IMaterial>> GetMaterialInterfaces() const { return mMaterialInterfaces; }
		void SetMaterialInterfaces(std::vector<std::shared_ptr<fq::graphics::IMaterial>> materialInterfaces);

		// Navigation 사용 여부
		bool GetIsNavigationMeshUsed() const { return mIsNavigationMeshUsed; }
		void SetIsNavigationMeshUsed(bool val) { mIsNavigationMeshUsed = val; }

		const std::vector<std::string>& GetMaterialPaths() const { return mMaterialPaths; }
		void SetMaterialPaths(const std::vector<std::string>& materialPaths) { mMaterialPaths = materialPaths; }

		// 라이트맵 관련 Getter/Setter
		const DirectX::SimpleMath::Vector4& GetLightmapUVScaleOffset() const;
		void SetLightmapUVScaleOffset(const DirectX::SimpleMath::Vector4& scaleOffset);

		int GetLightmapIndex() const;
		void SetLightmapIndex(unsigned int lightmapIndex);

		bool GetIsStatic() const;
		void SetIsStatic(bool bIsStatic);

		const DirectX::SimpleMath::Matrix& GetPrevApplyTransform() const { return mPrevApplyTransform; }
		void SetPrevApplyTransform(const DirectX::SimpleMath::Matrix& transform) { mPrevApplyTransform = transform; }

		// 그리기 여부 설정
		bool GetIsRender() const { return mbIsRender; }
		void SetIsRender(bool bIsRender);

		// 머터리얼 인스턴스 정보
		const fq::graphics::MaterialInstanceInfo& GetMaterialInstanceInfo() const { return mMaterialInstanceInfo; }
		void SetMaterialInstanceInfo(const fq::graphics::MaterialInstanceInfo& info);

		// 랜더링 시 행렬 사용 여부, false 시 단위 행렬을 전달 받음
		void SetUseTransform(bool bUseTransform) { mbIsUseTransform = bUseTransform; }
		bool GetUseTransform() const { return mbIsUseTransform; }

	private:
		entt::meta_handle GetHandle() override;

	private:
		bool mIsNavigationMeshUsed; // Navigation Mesh의 장애물로 사용할 것인가

		StaticMeshObject* mStaticMeshObject;
		fq::graphics::MeshObjectInfo mMeshInfomation;

		std::string mModelPath;
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
		DirectX::SimpleMath::Matrix mPrevApplyTransform; // 추후 모델 자체에서 처리할 예정

		// 그리기 여부
		bool mbIsRender;

		fq::graphics::MaterialInstanceInfo mMaterialInstanceInfo;

		bool mbIsUseTransform;
	};

}