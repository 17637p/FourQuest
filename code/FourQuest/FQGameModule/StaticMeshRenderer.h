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
		/// ������
		/// </summary>
		StaticMeshRenderer();
		~StaticMeshRenderer();

		/// <summary>
		/// ���纻�� ��ȯ�մϴ�
		/// </summary>
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone = nullptr)const override;

		/// <summary>
		/// ����ƽ �޽��� ��ȯ�մϴ�
		/// </summary>
		StaticMeshObject* GetStaticMeshObject() const { return mStaticMeshObject; }
		
		/// <summary>
		/// ����ƽ �޽��� �����մϴ�
		/// </summary>
		/// <param name="mesh"></param>
		void SetStaticMeshObject(StaticMeshObject* mesh) { mStaticMeshObject = mesh; }
	
		/// <summary>
		/// ���� ��θ� ��ȯ�մϴ�
		/// </summary>
		/// <returns></returns>
		std::string GetModelPath()const { return mMeshInfomation.ModelPath;}

		/// <summary>
		/// �𵨰�θ� �����մϴ�
		/// </summary>
		/// <param name="path"></param>
		void SetModelPath(std::string path);

		/// <summary>
		/// �޽� �̸��� ��ȯ�մϴ�
		/// </summary>
		/// <returns></returns>
		std::string GetMeshName()const { return mMeshInfomation.MeshName; }

		/// <summary>
		/// �޽� �̸��� �����մϴ�
		/// </summary>
		/// <param name="name"></param>
		void SetMeshName(std::string name);

		/// <summary>
		/// �����ϰ��ִ� ���׸��� �̸����� ��ȯ�մϴ�
		/// </summary>
		/// <returns></returns>
		const std::vector<std::string>& GetMaterials()const { return mMeshInfomation.MaterialNames; }

		/// <summary>
		/// ���׸����� �����մϴ�
		/// </summary>
		/// <param name="materials"></param>
		void SetMaterials(std::vector<std::string> materials);

		/// <summary>
		/// �޽������� �����մϴ�
		/// </summary>
		/// <param name="info">�޽� ����</param>
		void SetMeshObjectInfomation(fq::graphics::MeshObjectInfo info);

		/// <summary>
		/// �޽������� ��ȯ�մϴ�
		/// </summary>
		/// <returns>�޽� ����</returns>
		fq::graphics::MeshObjectInfo GetMeshObjectInfomation()const { return mMeshInfomation; }

	private:
		entt::meta_handle GetHandle() override;

	private:
		StaticMeshObject* mStaticMeshObject;
		fq::graphics::MeshObjectInfo mMeshInfomation;
	};

}