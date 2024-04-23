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
		Component* Clone(Component* clone /* = nullptr */)const override;

		/// <summary>
		/// 스태틱 메쉬를 반환합니다
		/// </summary>
		/// <returns></returns>
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
		/// 메쉬정보를 설정합니다
		/// </summary>
		/// <param name="info"></param>
		void SetMeshObjectInfomation(fq::graphics::MeshObjectInfo info);

	private:
		entt::meta_handle GetHandle() override;

	private:
		StaticMeshObject* mStaticMeshObject;
		fq::graphics::MeshObjectInfo mMeshInfomation;
	};

}