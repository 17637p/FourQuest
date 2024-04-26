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
		Component* Clone(Component* clone /* = nullptr */)const override;

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
		std::string GetModelPath()const { return mMeshInfomation.ModelPath; }

		/// <summary>
		/// 모델경로를 설정합니다
		/// </summary>
		void SetModelPath(std::string path);

		/// <summary>
		/// 메쉬 이름을 반환합니다
		/// </summary>
		/// <returns></returns>
		std::string GetMeshName()const { return mMeshInfomation.MeshName; }

		/// <summary>
		/// 메쉬 이름을 설정합니다
		/// </summary>
		/// <param name="name"></param>
		void SetMeshName(std::string name);

		/// <summary>
		/// 소유하고있는 메테리얼 이름들을 반환합니다
		/// </summary>
		/// <returns></returns>
		const std::vector<std::string>& GetMaterials()const { return mMeshInfomation.MaterialNames; }

		/// <summary>
		/// 메테리얼을 설정합니다
		/// </summary>
		void SetMaterials(std::vector<std::string> materials);

		/// <summary>
		/// 메쉬정보를 설정합니다
		/// </summary>
		/// <param name="info">메쉬 정보</param>
		void SetMeshObjectInfomation(fq::graphics::MeshObjectInfo info);

		/// <summary>
		/// 메쉬정보를 반환합니다
		/// </summary>
		/// <returns>메쉬 정보</returns>
		fq::graphics::MeshObjectInfo GetMeshObjectInfomation()const { return mMeshInfomation; }

	private:
		entt::meta_handle GetHandle() override;

	private:
		SkinnedMeshObject* mSkinnedMeshObject;
		fq::graphics::MeshObjectInfo mMeshInfomation;
	};


}