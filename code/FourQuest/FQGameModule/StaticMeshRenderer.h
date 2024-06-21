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
		std::string GetModelPath()const { return mMeshInfomation.ModelPath;}

		/// <summary>
		/// 모델경로를 설정합니다
		/// </summary>
		/// <param name="path"></param>
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
		/// <param name="materials"></param>
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

		/// <summary>
		/// 아웃라인 색깔을 반환합니다.
		/// </summary>
		DirectX::SimpleMath::Color GetOutlineColor() const { return mOutlineColor; }
		
		/// <summary>
		/// 아웃라인 색깔을 설정합니다.
		/// </summary>
		void SetOutlineColor(DirectX::SimpleMath::Color color);

		// Navigation 사용 여부
		bool GetIsNavigationMeshUsed() const { return mIsNavigationMeshUsed; }
		void SetIsNavigationMeshUsed(bool val) { mIsNavigationMeshUsed = val; }

	private:
		entt::meta_handle GetHandle() override;

	private:
		bool mIsNavigationMeshUsed; // Navigation Mesh의 장애물로 사용할 것인가

		StaticMeshObject* mStaticMeshObject;
		fq::graphics::MeshObjectInfo mMeshInfomation;
		DirectX::SimpleMath::Color mOutlineColor;
	};

}