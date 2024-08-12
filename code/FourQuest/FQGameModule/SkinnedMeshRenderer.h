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
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

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
		std::string GetModelPath()const { return mModelPath; }

		/// <summary>
		/// �𵨰�θ� �����մϴ�
		/// </summary>
		void SetModelPath(std::string path) { mModelPath = path; }

		/// <summary>
		/// �޽� �̸��� ��ȯ�մϴ�
		/// </summary>
		/// <returns></returns>
		std::string GetMeshName()const { return mMeshName; }

		/// <summary>
		/// �޽� �̸��� �����մϴ�
		/// </summary>
		/// <param name="name"></param>
		void SetMeshName(std::string name) { mMeshName = name; }

		std::vector<std::shared_ptr<fq::graphics::IMaterial>> GetMaterialInterfaces() const { return mMaterialInterfaces; }
		void SetMaterialInterfaces(std::vector<std::shared_ptr<fq::graphics::IMaterial>> materialInterfaces) { mMaterialInterfaces = materialInterfaces; }

		/// <summary>
		/// �޽������� �����մϴ�
		/// </summary>
		/// <param name="info">�޽� ����</param>
		void SetMeshObjectInfomation(fq::graphics::MeshObjectInfo info) { mMeshInfomation = std::move(info); if (mSkinnedMeshObject != nullptr) { mSkinnedMeshObject->SetMeshObjectInfo(mMeshInfomation); } }

		/// <summary>
		/// �޽������� ��ȯ�մϴ�
		/// </summary>
		/// <returns>�޽� ����</returns>
		fq::graphics::MeshObjectInfo GetMeshObjectInfomation() const { return mMeshInfomation; }

		void SetMaterialPaths(const std::vector<std::string>& materialPaths) { mMaterialPaths = materialPaths; }
		const std::vector<std::string>& GetMaterialPaths() const { return mMaterialPaths; }

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

		std::vector<std::string> mMaterialPaths; // ������ ����ȭ�� �����ͷ� �������̽� ������ ����
	};


}