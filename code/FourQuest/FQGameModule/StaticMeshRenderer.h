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
		~StaticMeshRenderer() = default;

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
		/// �޽������� ��ȯ�մϴ�
		/// </summary>
		/// <returns>�޽� ����</returns>
		fq::graphics::MeshObjectInfo GetMeshObjectInfomation()const { return mMeshInfomation; }

		/// <summary>
		/// �޽������� �����մϴ�
		/// </summary>
		/// <param name="info">�޽� ����</param>
		void SetMeshObjectInfomation(const fq::graphics::MeshObjectInfo& info);

		// ���͸��� �������̽� Getter/Setter
		std::vector<std::shared_ptr<fq::graphics::IMaterial>> GetMaterialInterfaces() const { return mMaterialInterfaces; }
		void SetMaterialInterfaces(std::vector<std::shared_ptr<fq::graphics::IMaterial>> materialInterfaces);

		// Navigation ��� ����
		bool GetIsNavigationMeshUsed() const { return mIsNavigationMeshUsed; }
		void SetIsNavigationMeshUsed(bool val) { mIsNavigationMeshUsed = val; }

		const std::vector<std::string>& GetMaterialPaths() const { return mMaterialPaths; }
		void SetMaterialPaths(const std::vector<std::string>& materialPaths) { mMaterialPaths = materialPaths; }

		// ����Ʈ�� ���� Getter/Setter
		const DirectX::SimpleMath::Vector4& GetLightmapUVScaleOffset() const;
		void SetLightmapUVScaleOffset(const DirectX::SimpleMath::Vector4& scaleOffset);

		int GetLightmapIndex() const;
		void SetLightmapIndex(unsigned int lightmapIndex);

		bool GetIsStatic() const;
		void SetIsStatic(bool bIsStatic);

		const DirectX::SimpleMath::Matrix& GetPrevApplyTransform() const { return mPrevApplyTransform; }
		void SetPrevApplyTransform(const DirectX::SimpleMath::Matrix& transform) { mPrevApplyTransform = transform; }

		// �׸��� ���� ����
		bool GetIsRender() const { return mbIsRender; }
		void SetIsRender(bool bIsRender);

		// ���͸��� �ν��Ͻ� ����
		const fq::graphics::MaterialInstanceInfo& GetMaterialInstanceInfo() const { return mMaterialInstanceInfo; }
		void SetMaterialInstanceInfo(const fq::graphics::MaterialInstanceInfo& info);

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
		DirectX::SimpleMath::Matrix mPrevApplyTransform; // ���� �� ��ü���� ó���� ����

		// �׸��� ����
		bool mbIsRender;

		fq::graphics::MaterialInstanceInfo mMaterialInstanceInfo;
	};

}