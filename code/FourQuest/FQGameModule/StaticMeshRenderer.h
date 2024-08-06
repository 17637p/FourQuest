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
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

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
		std::string GetModelPath()const { return mModelPath; }

		/// <summary>
		/// �𵨰�θ� �����մϴ�
		/// </summary>
		/// <param name="path"></param>
		void SetModelPath(std::string path) { mModelPath = path; }

		/// <summary>
		/// �ؽ�ó ���� ��θ� �����մϴ� 
		/// </summary>
		std::string GetTexturePath() const { return mTexturePath; }

		/// <summary>
		/// �ؽ��� ���� ��θ� ��ȯ�մϴ�
		/// </summary>
		void SetTexturePath(std::string path) { mTexturePath = path; }

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

		/// <summary>
		/// �����ϰ��ִ� ���׸��� �̸����� ��ȯ�մϴ�
		/// </summary>
		/// <returns></returns>
		const std::vector<std::string>& GetMaterials()const { return mMaterialNames; }

		/// <summary>
		/// ���׸����� �����մϴ�
		/// </summary>
		/// <param name="materials"></param>
		void SetMaterials(std::vector<std::string> materials) { mMaterialNames = std::move(materials); }

		void UpdateMaterialInfoByMaterialInterface();
		const std::vector<fq::graphics::MaterialInfo>& GetMaterialInfos() const;
		void SetMaterialInfos(std::vector<fq::graphics::MaterialInfo> materialInfos);

		std::vector<std::shared_ptr<fq::graphics::IMaterial>> GetMaterialInterfaces() const { return mMaterialInterfaces; }
		void SetMaterialInterfaces(std::vector<std::shared_ptr<fq::graphics::IMaterial>> materialInterfaces);

		/// <summary>
		/// �޽������� �����մϴ�
		/// </summary>
		/// <param name="info">�޽� ����</param>
		void SetMeshObjectInfomation(fq::graphics::MeshObjectInfo info) { mMeshInfomation = std::move(info); if (mStaticMeshObject != nullptr) { mStaticMeshObject->SetMeshObjectInfo(mMeshInfomation); } }

		/// <summary>
		/// �޽������� ��ȯ�մϴ�
		/// </summary>
		/// <returns>�޽� ����</returns>
		fq::graphics::MeshObjectInfo GetMeshObjectInfomation()const { return mMeshInfomation; }

		// Navigation ��� ����
		bool GetIsNavigationMeshUsed() const { return mIsNavigationMeshUsed; }
		void SetIsNavigationMeshUsed(bool val) { mIsNavigationMeshUsed = val; }

		void SetMaterialPaths(const std::vector<std::string>& materialPaths) { mMaterialPaths = materialPaths; }
		const std::vector<std::string>& GetMaterialPaths() const { return mMaterialPaths; }

		// ����Ʈ�� ����
		void SetLightmapUVScaleOffset(const DirectX::SimpleMath::Vector4& scaleOffset);
		const DirectX::SimpleMath::Vector4& GetLightmapUVScaleOffset() const;

		void SetLightmapIndex(unsigned int lightmapIndex);
		int GetLightmapIndex() const;

		void SetIsStatic(bool bIsStatic);
		bool GetIsStatic() const;

	private:
		entt::meta_handle GetHandle() override;

	private:
		bool mIsNavigationMeshUsed; // Navigation Mesh�� ��ֹ��� ����� ���ΰ�

		StaticMeshObject* mStaticMeshObject;
		fq::graphics::MeshObjectInfo mMeshInfomation;

		std::string mModelPath;
		std::string mTexturePath;
		std::string mMeshName;
		std::vector<std::string> mMaterialNames; // ����Ʈ �Ϸ� �� ���� ����
		std::vector<fq::graphics::MaterialInfo> mMaterialInfos; // ����Ʈ �Ϸ� �� ���� ����
		std::vector<std::shared_ptr<fq::graphics::IMaterial>> mMaterialInterfaces;

		// �߰�
		std::vector<std::string> mMaterialPaths; // ������ ����ȭ�� �����ͷ� �������̽� ������ ����

		// ����Ʈ�� ���� ����
		DirectX::SimpleMath::Vector4 mLightmapScaleOffset;
		int mLightmapIndex;
		bool mbIsStatic;
	};

}