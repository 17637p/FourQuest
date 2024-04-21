#pragma once

#include <memory>
#include <vector>

#include "IRenderObject.h"

namespace fq::graphics
{
	class StaticMesh;
	class SkinnedMesh;
	class Material;

	class StaticMeshObject : public IStaticMeshObject
	{
	public:
		StaticMeshObject(std::shared_ptr<StaticMesh> staticMesh,
			std::vector<std::shared_ptr<Material>> materials,
			DirectX::SimpleMath::Matrix transform);
		~StaticMeshObject() = default;

		virtual void UpdateTransform(const DirectX::SimpleMath::Matrix& transform) override;
		virtual inline const DirectX::SimpleMath::Matrix& GetTransform() const override;

		inline const std::shared_ptr<StaticMesh>& GetStaticMesh() const;
		inline const std::vector<std::shared_ptr<Material>>& GetMaterials() const;

	private:
		std::shared_ptr<StaticMesh> mStaticMesh;
		std::vector<std::shared_ptr<Material>> mMaterials;
		DirectX::SimpleMath::Matrix mTransform;
	};

#pragma region inlineFunc
	inline const std::shared_ptr<StaticMesh>& StaticMeshObject::GetStaticMesh() const
	{
		return mStaticMesh;
	}
	inline const std::vector<std::shared_ptr<Material>>& StaticMeshObject::GetMaterials() const
	{
		return mMaterials;
	}
	inline const DirectX::SimpleMath::Matrix& StaticMeshObject::GetTransform() const
	{
		return mTransform;
	}
#pragma endregion

	class SkinnedMeshObject : public ISkinnedMeshObject
	{
	public:
		SkinnedMeshObject(std::shared_ptr<SkinnedMesh> SkinnedMesh,
			std::vector<std::shared_ptr<Material>> materials,
			DirectX::SimpleMath::Matrix transform);
		~SkinnedMeshObject() = default;

		virtual void UpdateTransform(const DirectX::SimpleMath::Matrix& transform) override;
		virtual void UpdateBoneMatrix(const std::vector<DirectX::SimpleMath::Matrix>& boneMatrices) override;
		virtual inline const DirectX::SimpleMath::Matrix& GetTransform() const override;

		inline const std::shared_ptr<SkinnedMesh>& GetSkinnedMesh() const;
		inline const std::vector<std::shared_ptr<Material>>& GetMaterials() const;
		inline const std::vector<DirectX::SimpleMath::Matrix> GetBoneMatrices() const;

	private:
		std::shared_ptr<SkinnedMesh> mSkinnedMesh;
		std::vector<std::shared_ptr<Material>> mMaterials;
		DirectX::SimpleMath::Matrix mTransform;
		std::vector<DirectX::SimpleMath::Matrix> mBoneMatrices;
	};

	inline const std::shared_ptr<SkinnedMesh>& SkinnedMeshObject::GetSkinnedMesh() const
	{
		return mSkinnedMesh;
	}
	inline const std::vector<std::shared_ptr<Material>>& SkinnedMeshObject::GetMaterials() const
	{
		return mMaterials;
	}
	inline const DirectX::SimpleMath::Matrix& SkinnedMeshObject::GetTransform() const
	{
		return mTransform;
	}
	inline const std::vector<DirectX::SimpleMath::Matrix> SkinnedMeshObject::GetBoneMatrices() const
	{
		return mBoneMatrices;
	}
}

