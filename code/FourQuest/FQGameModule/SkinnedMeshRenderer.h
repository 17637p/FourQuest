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
		/// ���纻�� ��ȯ�մϴ�
		/// </summary>
		Component* Clone(Component* clone /* = nullptr */)const override;

		/// <summary>
		/// ��Ű�� �޽��� ��ȯ�մϴ� 
		/// </summary>
		SkinnedMeshObject* GetSkinnedMeshObject() const { return mSkinnedMeshObject; }

		/// <summary>
		/// ��Ű�� �޽��� �����մϴ�
		/// </summary>
		void SetSkinnedMeshObject(SkinnedMeshObject* mesh) { mSkinnedMeshObject = mesh; }

		/// <summary>
		/// ���� ��θ� ��ȯ�մϴ�
		/// </summary>
		/// <returns></returns>
		std::string GetModelPath()const { return mMeshInfomation.ModelPath; }

		/// <summary>
		/// �𵨰�θ� �����մϴ�
		/// </summary>
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
		SkinnedMeshObject* mSkinnedMeshObject;
		fq::graphics::MeshObjectInfo mMeshInfomation;
	};


}