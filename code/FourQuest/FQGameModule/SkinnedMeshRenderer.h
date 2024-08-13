#pragma once
#include "Component.h"
#include "../FQCommon/IFQRenderObject.h"
#include "../FQCommon/FQCommonGraphics.h"

namespace fq::game_module
{
	class SkinnedMeshRenderer :public Component
	{
		using SkinnedMeshObject = fq::graphics::ISkinnedMeshObject;
	public:
		SkinnedMeshRenderer();
		~SkinnedMeshRenderer();

		/// <summary>
		/// 복사본을 반환합니다 
		/// </summary>
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

		/// <summary>
		/// 스키닝 메쉬를 반환합니다 
		/// </summary>
		SkinnedMeshObject* GetSkinnedMeshObject() const { return mSkinnedMeshObject; }

		/// <summary>
		/// 스키닝 메쉬를 설정합니다
		/// </summary>
		void SetSkinnedMeshObject(SkinnedMeshObject* mesh) { mSkinnedMeshObject = mesh; }

		/// <summary>
		/// 모델의 경로를 반환합니다
		/// </summary>
		/// <returns></returns>
		std::string GetModelPath()const { return mModelPath; }

		/// <summary>
		/// 모델경로를 설정합니다
		/// </summary>
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
		fq::graphics::MeshObjectInfo GetMeshObjectInfomation() const { return mMeshInfomation; }

		/// <summary>
		/// 메쉬정보를 설정합니다
		/// </summary>
		/// <param name="info">메쉬 정보</param>
		void SetMeshObjectInfomation(const fq::graphics::MeshObjectInfo& info);

		std::vector<std::shared_ptr<fq::graphics::IMaterial>> GetMaterialInterfaces() const { return mMaterialInterfaces; }
		void SetMaterialInterfaces(std::vector<std::shared_ptr<fq::graphics::IMaterial>> materialInterfaces);
		
		const std::vector<std::string>& GetMaterialPaths() const { return mMaterialPaths; }
		void SetMaterialPaths(const std::vector<std::string>& materialPaths) { mMaterialPaths = materialPaths; }

		// 그리기 여부 설정
		bool GetIsRender() const { return mbIsRender; }
		void SetIsRender(bool bIsRender);

	private:
		entt::meta_handle GetHandle() override;

	private:
		SkinnedMeshObject* mSkinnedMeshObject;
		fq::graphics::MeshObjectInfo mMeshInfomation;

		std::string mModelPath;
		std::string mTexturePath;
		std::string mMeshName;
		std::vector<std::string> mMaterialNames;
		std::vector<fq::graphics::MaterialInfo> mMaterialInfos;
		std::vector<std::shared_ptr<fq::graphics::IMaterial>> mMaterialInterfaces;

		std::vector<std::string> mMaterialPaths; // 별도로 직렬화된 데이터로 인터페이스 가져올 예정

		bool mbIsRender;
	};
}