#pragma once
#include "Component.h"
#include "../FQCommon/FQCommonGraphics.h"
#include "../FQCommon/IFQRenderObject.h"

namespace fq::graphics { class ITerrainMeshObject; }

namespace fq::game_module
{
	class Terrain : public Component
	{
	public:
		Terrain();
		virtual ~Terrain();

		/// <summary>
		/// 복사본을 반환합니다 
		/// </summary>
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

		void SetTerrainLayers(const std::vector<fq::graphics::TerrainLayer>& terrainLayers);
		std::vector<fq::graphics::TerrainLayer> GetTerrainLayers() const;

		void SetAlphaMap(const std::string& alphaMapPath);
		std::string GetAlphaMap() const;

		void SetHeightMap(const std::string& heightMap);
		std::string GetHeightMap() const;

		void SetTerrainMeshObject(fq::graphics::ITerrainMeshObject* terrainMeshObject);
		fq::graphics::ITerrainMeshObject* GetTerrainMeshObject() const;

		void SetHeightScale(float heightScale);
		float GetHeightScale() const;
		void SetHeight(float height);
		float GetHeight() const;
		void SetWidth(float widht);
		float GetWidth() const;

		// NavMesh로 사용할 지 안 할지
		bool GetIsUseNavMesh() const { return mIsUseNavMesh; }
		void SetIsUseNavMesh(bool val) { mIsUseNavMesh = val; }

		float GetTextureWidth() const { return mTextureWidth; }
		void SetTextureWidth(float val) { mTextureWidth = val; }
		float GetTextureHeight() const { return mTextureHeight; }
		void SetTextureHeight(float val) { mTextureHeight = val; }
	private:
		entt::meta_handle GetHandle() override;

	private:
		fq::graphics::ITerrainMeshObject* mTerrainMeshObject;

		bool mIsUseNavMesh;
		std::vector<fq::graphics::TerrainLayer> mTerrainLayers; // 최대 4
		std::string mAlPhaMapPath; // R에는 BaseColor1, G에는 2, B에는 3, A에는 4
		std::string mHeightMapPath; 

		float mHeightScale;
		float mWidth;
		float mHeight;
		float mTextureWidth;
		float mTextureHeight;
	};


}